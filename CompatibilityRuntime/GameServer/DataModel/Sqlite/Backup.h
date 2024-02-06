#ifndef DATA_MODEL_SQLITE_BACKUP_H
#define DATA_MODEL_SQLITE_BACKUP_H

#include <string>
#include <memory>

struct sqlite3_backup;
struct sqlite3;

namespace sqlite {
    class Database;

    class Backup {
    public:
        Backup(const Database &destination, const std::string &destinationName,
            const Database &source, const std::string &sourceName);
        ~Backup() noexcept;

        Backup(const Backup &other) = delete;
        Backup &operator =(const Backup &other) = delete;

        inline sqlite3_backup *handle() const { return m_handle; }
        inline operator sqlite3_backup *() const { return m_handle; }

        bool step(int pages = -1);

        int remaining() const;
        int pagecount() const;

    private:
        sqlite3_backup *m_handle;
        sqlite3 *m_destinationHandle;
    };
}

#endif

