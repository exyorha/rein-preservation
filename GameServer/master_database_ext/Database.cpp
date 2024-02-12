#include "Database.h"
#include "DatabaseTable.h"

#include <msgpack/unpack.hpp>
#include <msgpack/iterator.hpp>
#include <msgpack/adaptor/cpp17/string_view.hpp>
#include <msgpack/adaptor/pair.hpp>
#include <msgpack/adaptor/int.hpp>

namespace MasterDatabase {

    Database::Database(const std::filesystem::path &filename) : m_image(filename), m_startOfTables(0) {
        m_tableOfTables = msgpack::unpack(
            m_image.data(),
            m_image.size(),
            m_startOfTables,
            unpackReferenceFunc,
            this);
    }

    Database::~Database() = default;

    bool Database::unpackReferenceFunc(msgpack::type::object_type type, std::size_t length, void* user_data) {
        (void)type;
        (void)length;
        (void)user_data;

        return true;
    }

    std::shared_ptr<DatabaseTable> Database::open(const std::string_view &name) {
        if(m_tableOfTables->type != msgpack::type::MAP)
            throw std::runtime_error("the table of tables is not a map");

        const auto &tables = m_tableOfTables->via.map;

        for(const auto &table: tables) {
            if(table.key.as<std::string_view>() == name) {

                std::pair<uint32_t, uint32_t> location; // offset, size
                table.val.convert(location);

                if(m_startOfTables + location.first + location.second > m_image.size())
                    throw std::runtime_error("table overruns the image");

                auto tableData = msgpack::unpack(
                    m_image.data() + m_startOfTables + location.first,
                    location.second,
                    unpackReferenceFunc,
                    this);

                auto handle = std::make_shared<DatabaseTable>(std::move(tableData));

                m_validHandles.emplace_back(handle);

                return handle;
            }
        }

        return {};
    }
}
