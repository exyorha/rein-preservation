#ifndef DATABASE_TABLE_H
#define DATABASE_TABLE_H

#include <msgpack/object.hpp>
#include <msgpack/unpack_decl.hpp>

#include <optional>
#include <vector>

namespace MasterDatabase {

    class DatabaseTable {
    public:
        explicit DatabaseTable(msgpack::object_handle &&tableData);
        ~DatabaseTable();

        DatabaseTable(const DatabaseTable &other) = delete;
        DatabaseTable &operator =(const DatabaseTable &other) = delete;

        inline size_t size() const {
            return m_tableData->via.array.size;
        }

        inline const msgpack::object &row(size_t index) const {
            return m_tableData->via.array.ptr[index];
        }

    private:
        static bool unpackReferenceFunc(msgpack::type::object_type type, std::size_t length, void* user_data);

        std::optional<std::vector<char>> m_uncompressedData;
        msgpack::object_handle m_tableData;
    };

}

#endif
