#include <WebContentServer/WebContentServer.h>

#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/Buffer.h"
#include "LLServices/Networking/KeyValuePairs.h"

#include <system_error>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/stat.h>
#include <cstring>
#include <charconv>

const WebContentServer::MimeType WebContentServer::m_mimeTypes[]{
    { ".html", "text/html; charset=utf-8" },
    { ".e",    "application/octet-stream" },
    { ".bin",  "application/octet-stream" },
    { ".xml",  "text/xml" },
    { ".xsl",  "text/xsl" },
    { ".css",  "text/css" },
    { ".js",   "text/javascript" },
    { ".png",  "image/png" },
    { ".wav",  "audio/wav" },
};


WebContentServer::WebContentServer(std::unique_ptr<WebContentStorage> &&storage) : m_storage(std::move(storage)) {

}

WebContentServer::~WebContentServer() = default;


std::optional<WebContentLocation> WebContentServer::mapPath(const std::string_view &path) {
    for(const auto &overridePath: m_overridePaths) {
        if(path == overridePath.path) {
            return WebContentLocation{
                .containingFilePath = overridePath.filename,
                .fileOffset = 0,
                .fileLength = WebContentLocation::UntilEOF,
                .lastModified = 0
            };
        }
    }

    return m_storage->lookup(path);
}

bool WebContentServer::tryServePath(const std::string_view &path, LLServices::HttpRequest &&request) {

    auto fileLocation = mapPath(path);
    if(!fileLocation.has_value()) {
        return false;
    }

    struct FileDescriptorHolder {
        int fd = -1;

        ~FileDescriptorHolder() {
            if(fd >= 0) {
                close(fd);
            }
        }
    } fd;

#if defined(_WIN32)
    fd.fd = _wopen(fileLocation->containingFilePath.c_str(), O_RDONLY | O_BINARY);
#else
    fd.fd = open(fileLocation->containingFilePath.c_str(), O_RDONLY);
#endif

    if(fd.fd < 0) {
        return false;
    }

    std::string_view extension(path);
    auto basenameDelimiter = extension.find_last_of('/');
    if(basenameDelimiter != std::string_view::npos)
        extension = extension.substr(basenameDelimiter + 1);

    auto extensionDelimiter = extension.find_last_of('.');
    if(extensionDelimiter == std::string_view::npos)
        extension = std::string_view();
    else
        extension = extension.substr(extensionDelimiter);

    bool foundExtension = false;

    for(const auto &mime: m_mimeTypes) {
        if(mime.extension == extension) {
            request.outputHeaders().add("Content-Type", mime.contentType);
            foundExtension = true;
            break;
        }
    }

    if(!foundExtension) {
        request.outputHeaders().add("Content-Type", "application/octet-stream");
    }

    if(fileLocation->lastModified == 0 || fileLocation->fileLength == WebContentLocation::UntilEOF) {
        struct stat st;
        if(fstat(fd.fd, &st) < 0)
            throw std::system_error(errno, std::generic_category());

        if(fileLocation->lastModified == 0) {
            fileLocation->lastModified = st.st_mtime;
        }

        if(fileLocation->fileLength == WebContentLocation::UntilEOF) {
            if(fileLocation->fileOffset >= st.st_size) {
                fileLocation->fileLength = 0;
            } else {
                fileLocation->fileLength = st.st_size - fileLocation->fileOffset;
            }
        }
    }

    std::stringstream etag;
    etag << "\"";
    etag << fileLocation->lastModified;
    etag << "\"";

    auto etagString = etag.str();

    request.outputHeaders().add("ETag", etagString.c_str());
    request.outputHeaders().add("Accept-Ranges", "bytes");

    struct tm parts;
#ifdef _WIN32
    gmtime_s(&parts, &fileLocation->lastModified);
#else
    gmtime_r(&fileLocation->lastModified, &parts);
#endif

    char lastModified[64];
    evutil_date_rfc1123(lastModified, sizeof(lastModified), &parts);
    request.outputHeaders().add("Last-Modified", lastModified);

    auto ifMatch = request.inputHeaders().find("If-Match");
    if(ifMatch && std::string_view(ifMatch) != etagString && strcmp(ifMatch, "*") != 0) {
        request.sendError(412, "Precondition Failed");
        return true;
    }

    auto ifNoneMatch = request.inputHeaders().find("If-None-Match");
    if(ifNoneMatch && std::string_view(ifNoneMatch) == etagString) {
        request.sendError(304, "Not Modified");
        return true;
    }

    LLServices::Buffer buffer;

    bool partial = false;

    if(request.command() == EVHTTP_REQ_HEAD) {
        request.outputHeaders().add("Content-Length", std::to_string(fileLocation->fileLength).c_str());
    } else {

        auto ranges = request.inputHeaders().find("Range");
        if(ranges) {
            auto parsedRanges = parseRanges(ranges, fileLocation->fileLength);
            if(parsedRanges.empty()) {
                request.sendError(416, "Requested Range Not Satisfiable");
                return true;
            } else {
                partial = true;

                if(parsedRanges.size() == 1) {

                    /*
                     * Only one range.
                     */

                    const auto &range = parsedRanges.front();

                    request.outputHeaders().add("Content-Range", formatRange(range, fileLocation->fileLength).c_str());

                    if(evbuffer_add_file(buffer.get(), fd.fd, fileLocation->fileOffset + range.first, range.second - range.first) < 0)
                        throw std::runtime_error("evbuffer_add_file has failed");
                } else {

                    /*
                     * Multiple ranges
                     */

                    std::optional<std::string> contentType;
                    auto contentTypeHeader = request.outputHeaders().find("Content-Type");
                    if(contentTypeHeader) {
                        contentType.emplace(contentTypeHeader);
                    }

                    request.outputHeaders().remove("Content-Type");
                    request.outputHeaders().add("Content-Type", "multipart/byteranges; boundary=17a5f6a4a1a724c650a54b20e671a970");

                    for(const auto &range: parsedRanges) {
                        std::stringstream header;
                        header <<
                            "--17a5f6a4a1a724c650a54b20e671a970\r\n"
                            "Content-Range: " << formatRange(range, fileLocation->fileLength) << "\r\n";

                        if(contentType.has_value()) {
                            header << "Content-Type: " << *contentType << "\r\n\r\n";
                        }

                        buffer.append(header.str());

                        FileDescriptorHolder copy;
                        copy.fd = dup(fd.fd);
                        if(copy.fd < 0)
                            throw std::system_error(errno, std::generic_category());

                        if(evbuffer_add_file(buffer.get(), copy.fd, fileLocation->fileOffset + range.first, range.second - range.first) < 0)
                            throw std::runtime_error("evbuffer_add_file has failed");
                    }

                    buffer.append("--17a5f6a4a1a724c650a54b20e671a970--");
                }
            }
        } else {
            if(evbuffer_add_file(buffer.get(), fd.fd, fileLocation->fileOffset, fileLocation->fileLength) < 0)
                throw std::runtime_error("evbuffer_add_file has failed");

            /*
            * The fd is now managed by the buffer.
            */
            fd.fd = -1;
        }
    }

    if(partial) {
        request.sendReply(206, "Partial Content", buffer);

    } else {
        request.sendReply(200, "OK", buffer);
    }

    return true;
}

void WebContentServer::handle(const std::string_view &path, LLServices::HttpRequest &&request) {

    if(request.command() != EVHTTP_REQ_GET && request.command() != EVHTTP_REQ_HEAD) {
        request.sendError(405, "Method Not Allowed");
        return;
    }

    bool found = tryServePath(path, std::move(request));

    if(!found && m_fallbackPage.has_value()) {
        found = tryServePath(*m_fallbackPage, std::move(request));
    }

    if(!found) {
        request.sendError(404, "Not Found");
    }


}

std::string WebContentServer::formatRange(const std::pair<size_t, size_t> &range, size_t fileSize) {
    std::stringstream rangeSpec;
    rangeSpec << "bytes ";
    rangeSpec << range.first;
    rangeSpec << "-" << (range.second - 1);
    rangeSpec << "/" << fileSize;
    return rangeSpec.str();
}

void WebContentServer::overridePath(std::string &&path, std::filesystem::path &&filename) {
    m_overridePaths.emplace_back(OverridePath{
        .path = std::move(path),
        .filename = std::move(filename)
    });
}

std::vector<std::pair<size_t, size_t>> WebContentServer::parseRanges(std::string_view rangeHeader, size_t fileSize) {
    static constexpr std::string_view prefix = "bytes=";

    if(!rangeHeader.starts_with(prefix)) {
        return {};
    }

    std::vector<std::pair<size_t, size_t>> ranges;

    rangeHeader = rangeHeader.substr(prefix.size());

    size_t partDelim;
    while(true) {
        while(!rangeHeader.empty() && rangeHeader[0] == ' ') {
            rangeHeader = rangeHeader.substr(1);
        }

        if(rangeHeader.empty())
            return {};

        partDelim = rangeHeader.find(',');
        std::string_view thisPart, nextPart;

        if(partDelim == std::string_view::npos) {
            thisPart = rangeHeader;
        } else {
            thisPart = rangeHeader.substr(partDelim);
            nextPart = rangeHeader.substr(partDelim + 1);
        }

        auto dash = thisPart.find('-');
        if(dash == std::string::npos)
            return {};

        size_t firstByte;
        size_t lastByte;

        if(dash == 0) {
            auto bytesOffEnd = parseByteCount(thisPart.substr(dash + 1));

            lastByte = fileSize;

            if(bytesOffEnd > fileSize) {
                firstByte = 0;
            } else {
                firstByte = fileSize - bytesOffEnd;
            }
        } else {
            firstByte = parseByteCount(thisPart.substr(0, dash));

            if(dash == thisPart.size() - 1) {
                lastByte = fileSize;
            } else {
                lastByte = parseByteCount(thisPart.substr(dash + 1)) + 1;
            }

            lastByte = std::min<size_t>(lastByte, fileSize);
        }

        if(firstByte < lastByte) {
            ranges.emplace_back(firstByte, lastByte);
        }

        if(partDelim == std::string::npos)
            break;

        rangeHeader = nextPart;
    }

    return ranges;
}

size_t WebContentServer::parseByteCount(const std::string_view &piece) {
    size_t value;

    auto result = std::from_chars(piece.data(), piece.data() + piece.size(), value, 10);
    if(result.ec != std::errc() || result.ptr != piece.data() + piece.size() || piece.empty())
        throw std::runtime_error("failed to parse the range value");

    return value;
}
