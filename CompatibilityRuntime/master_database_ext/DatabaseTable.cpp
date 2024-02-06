#include "DatabaseTable.h"

#include <lz4.h>

#include <msgpack/unpack.hpp>

#include <msgpack/adaptor/int.hpp>

namespace MasterDatabase {
    DatabaseTable::DatabaseTable(msgpack::object_handle &&tableData) : m_tableData(std::move(tableData)) {
        if(m_tableData->type == msgpack::type::EXT) {
            const auto &ext = m_tableData->via.ext;

            if(ext.type() != 99) {
                throw std::runtime_error("the table data is an extension of incorrect type " + std::to_string(ext.type()));
            }

            size_t headerSize = 0;
            auto header = msgpack::unpack(ext.data(), ext.size, headerSize);
            if(header->type != msgpack::type::POSITIVE_INTEGER) {
                throw std::runtime_error("the LZ4-compressed data is malformed");
            }

            auto uncompressedLength = header->as<uint32_t>();

            m_uncompressedData.emplace(uncompressedLength);

            auto result = LZ4_decompress_safe(
                ext.data() + headerSize,
                m_uncompressedData->data(),
                ext.size - headerSize,
                m_uncompressedData->size());

            if(result < 0 || result != uncompressedLength)
                throw std::runtime_error("the LZ4-compressed data is malformed: " + std::to_string(result));

            m_tableData = msgpack::unpack(
                m_uncompressedData->data(),
                m_uncompressedData->size(),
                unpackReferenceFunc,
                this);
        }

        if(m_tableData->type != msgpack::type::ARRAY) {
            throw std::runtime_error("the table data is not an array, but " + std::to_string(m_tableData->type));
        }
    }

    DatabaseTable::~DatabaseTable() = default;

    bool DatabaseTable::unpackReferenceFunc(msgpack::type::object_type type, std::size_t length, void* user_data) {
        (void)type;
        (void)length;
        (void)user_data;

        return true;
    }
}
