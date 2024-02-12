#include <DataModel/TableChangesetWriter.h>
#include <DataModel/DatabaseJSONRepresentation.h>

#include <DataModel/Sqlite/Database.h>
#include <DataModel/Sqlite/Session.h>
#include <DataModel/Sqlite/Statement.h>

#include <stdexcept>

TableChangesetWriter::TableChangesetWriter(sqlite::Database &db,
                                           ::google::protobuf::Map<std::string, ::apb::api::data::DiffData> *rpcChangeset, const char *tableName) :
    m_db(db), m_tableName(tableName) {

    auto entityName = tableNameToEntityName(tableName);

    m_diff = &(*rpcChangeset)[entityName];

    m_updateWriter.writeArrayOpen();
    m_deleteWriter.writeArrayOpen();

    auto statement = m_db.prepare("SELECT name, type = 'boolean' AS is_boolean FROM pragma_table_info(?)");
    statement->bind(1, m_tableName);

    while(statement->step()) {
        const auto &name = m_rawColumnNames.emplace_back(statement->columnText(0));
        m_columnNames.emplace_back(columnNameToEntityFieldName(name));
        m_columnIsBoolean.emplace_back(statement->columnInt(1) != 0);
    }
}

TableChangesetWriter::~TableChangesetWriter() = default;

void TableChangesetWriter::finalize() {
    m_updateWriter.writeArrayClose();
    m_deleteWriter.writeArrayClose();

    *m_diff->mutable_update_records_json() = m_updateWriter.output();
    *m_diff->mutable_delete_keys_json() = m_deleteWriter.output();

    m_updateWriter.clear();
    m_updateWriter.reset();

    m_deleteWriter.clear();
    m_deleteWriter.reset();
}

void TableChangesetWriter::writeEntry(sqlite::ChangesetIterator &it, const sqlite::ChangesetOperation &op) {
    switch(op.operation) {
        case SQLITE_INSERT:
        {
            m_updateWriter.writeMapOpen();

            if(op.numberOfColumns != m_columnNames.size()) {
                throw std::runtime_error("the number of columns in the changeset didn't match the expected");
            }

            for(int column = 0; column < op.numberOfColumns; column++) {
                auto value = it.newValue(column);

                if(value == nullptr) {
                    throw std::runtime_error("unexpected absent value on INSERT");
                }

                m_updateWriter.writeString(m_columnNames[column]);
                writeSQLiteValue(m_updateWriter, value, m_columnIsBoolean[column]);
            }

            m_updateWriter.writeMapClose();
            break;
        }

        case SQLITE_UPDATE:
        {
            auto primaryKey = it.getPrimaryKey();

            if(!m_selectWholeRecord) {
                std::stringstream selectText;
                selectText << "SELECT * FROM " << m_tableName;

                bool first = true;

                for(int columnIndex = 0; columnIndex < primaryKey.numberOfColumns; columnIndex++) {
                    if(primaryKey.primaryKeyMask[columnIndex]) {
                        if(first)
                            selectText << " WHERE ";
                        else
                            selectText << " AND ";

                        first = false;

                        selectText << m_rawColumnNames[columnIndex] << " = ?";
                    }
                }

                m_selectWholeRecord = m_db.prepare(selectText.str());
            }

            m_selectWholeRecord->clearBindings();

            int nextBinding = 1;
            for(int columnIndex = 0; columnIndex < primaryKey.numberOfColumns; columnIndex++) {
                if(primaryKey.primaryKeyMask[columnIndex]) {
                    auto value = it.newValue(columnIndex);
                    if(!value)
                        value = it.oldValue(columnIndex);

                    if(!value)
                        throw std::runtime_error("a column participating in the primary key is absent from the changeset");

                    m_selectWholeRecord->bind(nextBinding, value);
                    nextBinding++;
                }
            }

            if(!m_selectWholeRecord->step())
                throw std::runtime_error("failed to retrieve the complete update record");

            m_updateWriter.writeMapOpen();

            int colCount = m_selectWholeRecord->columnCount();
            for(int colIndex = 0; colIndex < colCount; colIndex++) {
                m_updateWriter.writeString(columnNameToEntityFieldName(m_selectWholeRecord->columnName(colIndex)));
                writeSQLiteColumnValue(m_updateWriter, *m_selectWholeRecord, colIndex);
            }

            m_updateWriter.writeMapClose();

            m_selectWholeRecord->reset();

            break;
        }


        default:
            /*
             * TODO: it's not currently known how deletes should be represented
             */
            throw std::runtime_error("TableChangesetWriter::writeEntry: unimplemented change operation: " + std::to_string(op.operation));
    }
}

