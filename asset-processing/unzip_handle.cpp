#include "unzip_handle.h"

#include <algorithm>
#include <unzip.h>

zlib_filefunc64_def UnzipHandle::m_fileFunctions{
    .zopen64_file = ioOpen,
    .zread_file = ioRead,
    .zwrite_file = nullptr,
    .ztell64_file = ioTell,
    .zseek64_file = ioSeek,
    .zclose_file  = ioClose,
    .zerror_file = ioError

};

UnzipHandle::FileStreamHandle::FileStreamHandle(const FileFunctionsOpenParameters &parameters) : m_offset(parameters.offset), m_length(parameters.length) {
#ifdef _WIN32
    m_handle = _wfopen(parameters.filename->c_str(), L"rb");
#else
    m_handle = fopen64(parameters.filename->c_str(), "rb");
#endif

    if(!m_handle) {
        throw std::runtime_error("failed to open the file");
    }

    fseeko64(m_handle, 0, SEEK_END);

    auto fileLength = ftello64(m_handle);

    if(m_offset > fileLength) {
        m_offset = 0;
        m_length = 0;
    } else {
        m_length = std::min<ZPOS64_T>(m_length, fileLength - m_offset);
    }

    fseeko(m_handle, m_offset, SEEK_SET);
}

UnzipHandle::FileStreamHandle::~FileStreamHandle() {
    fclose(m_handle);
}

void *UnzipHandle::ioOpen(void *opaque, const void* filename, int mode) {
    (void)opaque;
    (void)mode;

    try {
        return new FileStreamHandle(*reinterpret_cast<const FileFunctionsOpenParameters *>(filename));
    } catch(...) {
        return nullptr;
    }
}

int UnzipHandle::ioClose(void *opaque, void *stream) {
    (void)opaque;

    delete static_cast<FileStreamHandle *>(stream);

    return 0;
}

uLong UnzipHandle::ioRead(void *opaque, void *stream, void* buf, uLong size) {
    (void)opaque;

    const auto &handle = *static_cast<FileStreamHandle *>(stream);

    auto currentPos = ftello64(handle);
    if(currentPos < handle.offset() || currentPos >= handle.offset() + handle.length()) {
        return 0;
    }

    size = std::min<uLong>(size, handle.offset() + handle.length() - currentPos);

    return fread(buf, 1, size, handle);
}

ZPOS64_T UnzipHandle::ioTell(void *opaque, void *stream) {
    (void)opaque;

    const auto &handle = *static_cast<FileStreamHandle *>(stream);

    auto currentPos = ftello64(handle);

    if(currentPos < handle.offset())
        return 0;

    return std::min<uLong>(currentPos - handle.offset(), handle.length());
}

long UnzipHandle::ioSeek(void *opaque, voidpf stream, ZPOS64_T offset, int origin) {

    (void)opaque;

    const auto &handle = *static_cast<FileStreamHandle *>(stream);

    ZPOS64_T finalOffset;
    switch(origin) {
        case ZLIB_FILEFUNC_SEEK_CUR:
            finalOffset = ioTell(opaque, stream) + offset;
            break;

        case ZLIB_FILEFUNC_SEEK_END:
            finalOffset = handle.length() + offset;
            break;

        default:
        case ZLIB_FILEFUNC_SEEK_SET:
            finalOffset = offset;
            break;
    }

    finalOffset = std::clamp<ZPOS64_T>(finalOffset, 0, handle.length()) + handle.offset();

    return fseeko64(handle, finalOffset, SEEK_SET);
}

int UnzipHandle::ioError(void *opaque, void *stream) {
    (void)opaque;

    const auto &handle = *static_cast<FileStreamHandle *>(stream);

    return ferror(handle);
}

UnzipHandle::UnzipHandle() noexcept : m_handle(nullptr), m_fileOpen(false) {

}

UnzipHandle::UnzipHandle(const std::filesystem::path &filename, ZPOS64_T offset, ZPOS64_T length) : UnzipHandle() {
    FileFunctionsOpenParameters params = {
        .filename = &filename,
        .offset = offset,
        .length = length
    };
    m_handle = unzOpen2_64(&params, &m_fileFunctions);
    if(!m_handle) {
        throw std::runtime_error("unzOpen64 has failed");
    }
}

UnzipHandle::~UnzipHandle() {
    if(m_handle) {
        if(m_fileOpen) {
            unzCloseCurrentFile(m_handle);
            m_fileOpen = false;
        }

        unzClose(m_handle);
    }
}

UnzipHandle::UnzipHandle(UnzipHandle &&other) noexcept : UnzipHandle() {

    *this = std::move(other);
    m_fileOpen = other.m_fileOpen;
    other.m_fileOpen = false;
}

UnzipHandle &UnzipHandle::operator =(UnzipHandle &&other) noexcept {
    if(this != &other) {
        if(m_handle) {
            unzClose(m_handle);
            m_handle = nullptr;
            m_fileOpen = false;
        }

        m_handle = other.m_handle;
        m_fileOpen = other.m_fileOpen;
        other.m_handle = nullptr;
        other.m_fileOpen = false;
    }

    return *this;
}

bool UnzipHandle::translateNavigationResult(int error) {
    if(error == UNZ_OK)
        return true;
    else if(error == UNZ_END_OF_LIST_OF_FILE) {
        return false;
    }

    raiseException(error);
}

bool UnzipHandle::goToFirst() {
    if(m_fileOpen)
        throw std::logic_error("UnzipHandle: cannot navigate while a file is open");

    return translateNavigationResult(unzGoToFirstFile(m_handle));
}

bool UnzipHandle::goToNext() {
    if(m_fileOpen)
        throw std::logic_error("UnzipHandle: cannot navigate while a file is open");

    return translateNavigationResult(unzGoToNextFile(m_handle));
}

bool UnzipHandle::locateFile(const char *filename, int caseSensitive) {
    if(m_fileOpen)
        throw std::logic_error("UnzipHandle: cannot navigate while a file is open");

    return translateNavigationResult(unzLocateFile(m_handle, filename, caseSensitive));
}

void UnzipHandle::raiseException(int error) {
    throw std::runtime_error("unzip error " + std::to_string(error));
}

UnzipFileInfo UnzipHandle::getCurrentFileInfo() const {
    UnzipFileInfo info;

    auto result = unzGetCurrentFileInfo64(m_handle, &info, nullptr, 0, nullptr, 0, nullptr, 0);
    checkException(result);

    info.fileName.resize(info.size_filename);
    info.extraField.resize(info.size_file_extra);
    info.comment.resize(info.size_file_comment);
    result = unzGetCurrentFileInfo64(m_handle, &info, info.fileName.data(), info.fileName.size(), info.extraField.data(), info.extraField.size(),
                                     info.comment.data(), info.comment.size());
    checkException(result);

    return info;
}

void UnzipHandle::openCurrentFile(int *method, int *level, bool raw, const char *password) {
    int dummyMethod;
    int *pmethod = method;
    if(!pmethod) {
        pmethod = &dummyMethod;
    }

    if(m_fileOpen) {
        throw std::logic_error("UnzipHandle: a file is already open");
    }

    auto result = unzOpenCurrentFile3(m_handle, pmethod, level, raw, password);
    checkException(result);

    m_fileOpen = true;
}

void UnzipHandle::closeCurrentFile() {
    if(!m_fileOpen) {
        throw std::logic_error("UnzipHandle: no file is currently open");
    }

    auto result = unzCloseCurrentFile(m_handle);
    m_fileOpen = false;

    checkException(result);
}

ZPOS64_T UnzipHandle::getCurrentFileDataOffset() const {
    if(!m_fileOpen) {
        throw std::logic_error("UnzipHandle: no file is currently open");
    }

    return unzGetCurrentFileZStreamPos64(m_handle);
}

ZPOS64_T UnzipHandle::getCurrentFilePosition() const {
    if(!m_fileOpen) {
        throw std::logic_error("UnzipHandle: no file is currently open");
    }

    return unztell64(m_handle);
}

bool UnzipHandle::currentFileIsAtEnd() const {
    if(!m_fileOpen) {
        throw std::logic_error("UnzipHandle: no file is currently open");
    }

    return unzeof(m_handle) != 0;
}

unsigned int UnzipHandle::readCurrentFile(void *buf, unsigned int length) {
    if(!m_fileOpen) {
        throw std::logic_error("UnzipHandle: no file is currently open");
    }

    auto result = unzReadCurrentFile(m_handle, buf, length);
    if(result < 0)
        raiseException(result);

    return static_cast<unsigned int>(result);
}

unz64_file_pos UnzipHandle::getDirectoryPos() const {
    unz64_file_pos result;

    auto status = unzGetFilePos64(m_handle, &result);
    checkException(status);

    return result;
}

void UnzipHandle::seekDirectoryPos(const unz64_file_pos &pos) {
    return checkException(unzGoToFilePos64(m_handle, &pos));
}
