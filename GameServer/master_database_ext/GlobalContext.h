#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include <atomic>
#include <filesystem>
#include <optional>

#include "Database.h"

namespace MasterDatabase {

    class DatabaseTable;

    class GlobalContext {
    public:
        GlobalContext();

    private:
        ~GlobalContext();

    public:
        GlobalContext(const GlobalContext &other) = delete;
        GlobalContext &operator =(const GlobalContext &other) = delete;

        void addReference() noexcept;
        void release() noexcept;

        static void freeFunc(void *ptr);

        inline const std::filesystem::path &databaseFile() const {
            return m_databaseFile;
        }

        inline void setDatabaseFile(const std::filesystem::path &databaseFile) {
            setDatabaseFile(databaseFile);
        }

        void setDatabaseFile(std::filesystem::path &&databaseFile);

        std::shared_ptr<DatabaseTable> open(const std::string_view &tableName);

    private:
        std::filesystem::path m_databaseFile;
        std::atomic<unsigned int> m_references;
        std::optional<Database> m_database;
    };


}

#endif

