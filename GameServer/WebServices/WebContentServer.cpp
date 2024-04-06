#include "WebContentServer.h"

#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/URI.h"
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
    { "",      "text/html; charset=utf-8" },
    { ".e",    "application/octet-stream" },
    { ".bin",  "application/octet-stream" },
    { ".xml",  "text/xml" },
    { ".xsl",  "text/xsl" },
    { ".css",  "text/css" },
    { ".js",   "text/javascript" },
};


WebContentServer::WebContentServer(const std::filesystem::path &root) : m_root(root) {

}

WebContentServer::~WebContentServer() = default;


std::optional<std::filesystem::path> WebContentServer::mapPath(const std::string_view &path) {
    for(const auto &overridePath: m_overridePaths) {
        if(path == overridePath.path)
            return overridePath.filename;
    }

    auto justPath = path.substr(1);
    auto normalized = std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t *>(justPath.data()), justPath.size())).lexically_normal();

    if(normalized.empty() || !normalized.is_relative()) {
        return std::nullopt;
    }

    for(const auto &component: normalized) {
        if(component == "..") {
            return std::nullopt;
        }
    }

    return m_root / normalized;;
}


void WebContentServer::handle(const std::string_view &path, LLServices::HttpRequest &&request) {

    if(request.command() != EVHTTP_REQ_GET && request.command() != EVHTTP_REQ_HEAD) {
        request.sendError(405, "Method Not Allowed");
        return;
    }

    auto filePath = mapPath(path);
    if(!filePath.has_value()) {
        request.sendError(404, "Not Found");
        return;
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
    fd.fd = _wopen(filePath->c_str(), O_RDONLY | O_BINARY);
#else
    fd.fd = open(filePath->c_str(), O_RDONLY);
#endif

    if(fd.fd < 0) {
        request.sendError(404, "Not Found");

        return;
    }

    auto type = filePath->extension().generic_string();

    for(const auto &mime: m_mimeTypes) {
        if(mime.extension == type) {
            request.outputHeaders().add("Content-Type", mime.contentType);
            break;
        }
    }

    struct stat st;
    if(fstat(fd.fd, &st) < 0)
        throw std::system_error(errno, std::generic_category());

    std::stringstream etag;
    etag << "\"";
    etag << st.st_mtime;
#ifndef _WIN32
    etag << "_";
    etag << st.st_mtim.tv_nsec;
#endif
    etag << "\"";

    auto etagString = etag.str();

    request.outputHeaders().add("ETag", etagString.c_str());
    request.outputHeaders().add("Accept-Ranges", "bytes");

    struct tm parts;
#ifdef _WIN32
    gmtime_s(&parts, &st.st_mtime);
#else
    gmtime_r(&st.st_mtime, &parts);
#endif

    char lastModified[64];
    evutil_date_rfc1123(lastModified, sizeof(lastModified), &parts);
    request.outputHeaders().add("Last-Modified", lastModified);

    auto ifMatch = request.inputHeaders().find("If-Match");
    if(ifMatch && std::string_view(ifMatch) != etagString && strcmp(ifMatch, "*") != 0) {
        request.sendError(412, "Precondition Failed");
        return;
    }

    auto ifNoneMatch = request.inputHeaders().find("If-None-Match");
    if(ifNoneMatch && std::string_view(ifNoneMatch) == etagString) {
        request.sendError(304, "Not Modified");
        return;
    }

    LLServices::Buffer buffer;

    bool partial = false;

    if(request.command() == EVHTTP_REQ_HEAD) {
        request.outputHeaders().add("Content-Length", std::to_string(st.st_size).c_str());
    } else {

        auto ranges = request.inputHeaders().find("Range");
        if(ranges) {
            auto parsedRanges = parseRanges(ranges, st.st_size);
            if(parsedRanges.empty()) {
                request.sendError(416, "Requested Range Not Satisfiable");
                return;
            } else {
                partial = true;

                if(parsedRanges.size() == 1) {

                    /*
                     * Only one range.
                     */

                    const auto &range = parsedRanges.front();

                    request.outputHeaders().add("Content-Range", formatRange(range, st.st_size).c_str());

                    if(evbuffer_add_file(buffer.get(), fd.fd, range.first, range.second - range.first) < 0)
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
                            "Content-Range: " << formatRange(range, st.st_size) << "\r\n";

                        if(contentType.has_value()) {
                            header << "Content-Type: " << *contentType << "\r\n\r\n";
                        }

                        buffer.append(header.str());

                        FileDescriptorHolder copy;
                        copy.fd = dup(fd.fd);
                        if(copy.fd < 0)
                            throw std::system_error(errno, std::generic_category());

                        if(evbuffer_add_file(buffer.get(), copy.fd, range.first, range.second - range.first) < 0)
                            throw std::runtime_error("evbuffer_add_file has failed");
                    }

                    buffer.append("--17a5f6a4a1a724c650a54b20e671a970--");
                }
            }
        } else {
            if(evbuffer_add_file(buffer.get(), fd.fd, 0, -1) < 0)
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
