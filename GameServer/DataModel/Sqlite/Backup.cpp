#include <DataModel/Sqlite/Backup.h>
#include <DataModel/Sqlite/Database.h>
#include <DataModel/Sqlite/Error.h>

namespace sqlite {
    Backup::Backup(const Database &destination, const std::string &destinationName, const Database &source, const std::string &sourceName) :
        m_destinationHandle(destination) {

        m_handle = sqlite3_backup_init(destination, destinationName.c_str(), source, sourceName.c_str());
        if (m_handle == nullptr)
            Error::raiseError(sqlite3_errcode(m_destinationHandle), m_destinationHandle);
    }

    Backup::~Backup() noexcept {
        sqlite3_backup_finish(m_handle);
    }

    bool Backup::step(int pages) {
        int status = sqlite3_backup_step(m_handle, pages);

        if (status == SQLITE_DONE)
            return true;
        else if (status == SQLITE_OK)
            return false;

        Error::raiseError(status, m_destinationHandle);
    }

    int Backup::remaining() const {
        return sqlite3_backup_remaining(m_handle);
    }

    int Backup::pagecount() const {
        return sqlite3_backup_pagecount(m_handle);
    }
}

