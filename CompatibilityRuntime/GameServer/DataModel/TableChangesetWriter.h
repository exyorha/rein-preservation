#ifndef DATA_MODEL_TABLE_CHANGESET_WRITER_H
#define DATA_MODEL_TABLE_CHANGESET_WRITER_H

#include <service/DataService.pb.h>
#include "JSONWriter.h"

namespace sqlite {
    class Database;
    class ChangesetIterator;
    struct ChangesetOperation;
    class Statement;
}

class TableChangesetWriter {
public:
    TableChangesetWriter(sqlite::Database &db,
                         ::google::protobuf::Map<std::string, ::apb::api::data::DiffData> *rpcChangeset, const char *tableName);
    ~TableChangesetWriter();

    TableChangesetWriter(const TableChangesetWriter &other) = delete;
    TableChangesetWriter &operator =(const TableChangesetWriter &other) = delete;

    inline bool isSameTable(const char *tableName) const {
        return m_tableName == tableName;
    }

    void finalize();

    void writeEntry(sqlite::ChangesetIterator &it, const sqlite::ChangesetOperation &op);

private:
    sqlite::Database &m_db;
    std::string m_tableName;
    std::vector<std::string> m_columnNames;
    std::vector<std::string> m_rawColumnNames;
    std::vector<bool> m_columnIsBoolean;
    ::apb::api::data::DiffData *m_diff;
    JSONWriter m_updateWriter;
    JSONWriter m_deleteWriter;
    std::unique_ptr<sqlite::Statement> m_selectWholeRecord;
};


#endif
