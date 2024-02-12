#ifndef DATA_MODEL_CHANGESET_CAPTURING_SCOPE_H
#define DATA_MODEL_CHANGESET_CAPTURING_SCOPE_H

#include <service/DataService.pb.h>

#include <DataModel/Sqlite/Session.h>

namespace sqlite {
    class Database;
}

class ChangesetCapturingScope {
public:
    explicit ChangesetCapturingScope(sqlite::Database &onDatabase);
    ~ChangesetCapturingScope();

    ChangesetCapturingScope(const ChangesetCapturingScope &other) = delete;
    ChangesetCapturingScope &operator =(const ChangesetCapturingScope &other) = delete;

    void serialize(::google::protobuf::Map<std::string, ::apb::api::data::DiffData> *changeset);

private:
    static int tableFilter(void *context, const char *tableName);

    sqlite::Database &m_database;
    sqlite::Session m_session;
};

#endif
