#ifndef UNZIP_HANDLE_H
#define UNZIP_HANDLE_H

#include <string>
#include <filesystem>

#include <unzip.h>

struct UnzipFileInfo final : public unz_file_info64 {
    std::string fileName;
    std::string extraField;
    std::string comment;
};

class UnzipHandle {
public:
    UnzipHandle() noexcept;
    explicit UnzipHandle(const std::filesystem::path &filename, ZPOS64_T offset, ZPOS64_T length);
    ~UnzipHandle();

    UnzipHandle(const UnzipHandle &other) = delete;
    UnzipHandle &operator =(const UnzipHandle &other) = delete;

    UnzipHandle(UnzipHandle &&other) noexcept;
    UnzipHandle &operator =(UnzipHandle &&other) noexcept;

    bool goToFirst();
    bool goToNext();
    bool locateFile(const char *filename, int caseSensitive = 1);

    UnzipFileInfo getCurrentFileInfo() const;

    inline bool isFileOpen() const {
        return m_fileOpen;
    }

    unz64_file_pos getDirectoryPos() const;
    void seekDirectoryPos(const unz64_file_pos &pos);

    void openCurrentFile(int *method = nullptr, int *level = nullptr, bool raw = false, const char *password = nullptr);
    void closeCurrentFile();
    ZPOS64_T getCurrentFileDataOffset() const;

    ZPOS64_T getCurrentFilePosition() const;
    bool currentFileIsAtEnd() const;
    unsigned int readCurrentFile(void *buf, unsigned int length);

private:
    struct FileFunctionsOpenParameters {
        const std::filesystem::path *filename;
        ZPOS64_T offset;
        ZPOS64_T length;
    };

    class FileStreamHandle {
    public:
        explicit FileStreamHandle(const FileFunctionsOpenParameters &parameters);
        ~FileStreamHandle();

        FileStreamHandle(const FileStreamHandle &other) = delete;
        FileStreamHandle &operator =(const FileStreamHandle &other) = delete;

        inline operator FILE *() const {
            return m_handle;
        }

        inline ZPOS64_T offset() const {
            return m_offset;
        }

        inline ZPOS64_T length() const {
            return m_length;
        }

    private:
        FILE *m_handle;
        ZPOS64_T m_offset;
        ZPOS64_T m_length;
    };

    static inline void checkException(int error) {
        if(error != UNZ_OK) {
            raiseException(error);
        }
    }

    static bool translateNavigationResult(int error);

    [[noreturn]] static void raiseException(int error);

    static void *ZCALLBACK ioOpen(void *opaque, const void* filename, int mode);
    static int ZCALLBACK ioClose(void *opaque, void *stream);
    static uLong ZCALLBACK ioRead(void *opaque, void *stream, void* buf, uLong size);
    static ZPOS64_T ZCALLBACK ioTell(void *opaque, void *stream);
    static long ZCALLBACK ioSeek(void *opaque, voidpf stream, ZPOS64_T offset, int origin);
    static int ZCALLBACK ioError(void *opaque, void *stream);

    unzFile m_handle;
    bool m_fileOpen;

    static zlib_filefunc64_def m_fileFunctions;
};

#endif

