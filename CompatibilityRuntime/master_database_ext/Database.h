#ifndef DATABASE_H
#define DATABASE_H

#include <filesystem>
#include <memory>

#include <msgpack/unpack_decl.hpp>

#include "DatabaseImage.h"

namespace MasterDatabase {

    class DatabaseTable;

    class Database {
    public:
        explicit Database(const std::filesystem::path &filename);
        ~Database();

        Database(const Database &other) = delete;
        Database &operator =(const Database &other) = delete;

        std::shared_ptr<DatabaseTable> open(const std::string_view &name);

    private:
        static bool unpackReferenceFunc(msgpack::type::object_type type, std::size_t length, void* user_data);

        DatabaseImage m_image;
        size_t m_startOfTables;
        msgpack::object_handle m_tableOfTables;
        std::vector<std::shared_ptr<DatabaseTable>> m_validHandles;
    };

}

#endif
