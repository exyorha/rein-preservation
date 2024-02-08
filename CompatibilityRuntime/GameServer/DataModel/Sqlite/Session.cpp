#include <DataModel/Sqlite/Session.h>
#include <DataModel/Sqlite/Error.h>
#include <DataModel/Sqlite/Database.h>

namespace sqlite {

    Changeset::Changeset() noexcept : m_data(nullptr), m_size(0) {

    }

    Changeset::Changeset(void *data, size_t size) : m_data(data), m_size(size) {

    }

    Changeset::~Changeset() {
        sqlite3_free(m_data);
    }

    Changeset::Changeset(Changeset &&other) noexcept : Changeset() {
        *this = std::move(other);
    }

    Changeset &Changeset::operator =(Changeset &&other) noexcept {
        if(this != &other) {
            if(m_data) {
                sqlite3_free(m_data);
                m_data = nullptr;
            }

            m_data = other.m_data;
            m_size = other.m_size;

            other.m_data = nullptr;
            other.m_size = 0;
        }

        return *this;
    }

    ChangesetIterator::ChangesetIterator() noexcept : m_iterator(nullptr) {

    }

    ChangesetIterator::ChangesetIterator(void *data, size_t size) {
        auto result = sqlite3changeset_start(&m_iterator, size, data);
        if(result != SQLITE_OK)
            sqlite::Error::raiseError(result, nullptr);
    }

    ChangesetIterator::ChangesetIterator(const Changeset &changeset) : ChangesetIterator(changeset.data(), changeset.size()) {

    }

    ChangesetIterator::~ChangesetIterator() {
        if(m_iterator) {
            sqlite3changeset_finalize(m_iterator);
        }
    }

    ChangesetIterator::ChangesetIterator(ChangesetIterator &&other) noexcept : ChangesetIterator() {
        *this = std::move(other);
    }

    ChangesetIterator &ChangesetIterator::operator =(ChangesetIterator &&other) noexcept {
        if(this != &other) {
            if(m_iterator)
                sqlite3changeset_finalize(m_iterator);

            m_iterator = other.m_iterator;
            other.m_iterator = nullptr;
        }

        return *this;
    }

    bool ChangesetIterator::next(ChangesetOperation &op) {
        auto result = sqlite3changeset_next(m_iterator);
        if(result == SQLITE_DONE)
            return false;

        if(result != SQLITE_ROW)
            sqlite::Error::raiseError(result, nullptr);

        result = sqlite3changeset_op(m_iterator, &op.tableName, &op.numberOfColumns, &op.operation, &op.indirect);
        if(result != SQLITE_OK) {
            sqlite::Error::raiseError(result, nullptr);
        }

        return true;
    }

    sqlite3_value *ChangesetIterator::oldValue(int column) const {
        sqlite3_value *value;

        auto result = sqlite3changeset_old(m_iterator, column, &value);
        if(result != SQLITE_OK)
            sqlite::Error::raiseError(result, nullptr);

        return value;
    }
    sqlite3_value *ChangesetIterator::newValue(int column) const {
        sqlite3_value *value;

        auto result = sqlite3changeset_new(m_iterator, column, &value);
        if(result != SQLITE_OK)
            sqlite::Error::raiseError(result, nullptr);

        return value;
    }

    ChangesetPrimaryKeyDefinition ChangesetIterator::getPrimaryKey() const {
        ChangesetPrimaryKeyDefinition def;

        auto result = sqlite3changeset_pk(m_iterator, &def.primaryKeyMask, &def.numberOfColumns);
        if(result != SQLITE_OK)
            sqlite::Error::raiseError(result, nullptr);

        return def;
    }

    Session::Session(sqlite3 *db, const char *databaseName) {
        auto result = sqlite3session_create(db, databaseName, &m_session);
        if(result != SQLITE_OK)
            sqlite::Error::raiseError(result, nullptr);
    }

    Session::Session(Database &db, const char *databaseName) : Session(db.handle(), databaseName) {

    }

    Session::~Session() {
        sqlite3session_delete(m_session);
    }

    Changeset Session::changeset() const {
        int size;
        void *data;

        auto result = sqlite3session_changeset(m_session, &size, &data);
        if(result != SQLITE_OK)
            sqlite::Error::raiseError(result, nullptr);

        return Changeset(data, size);
    }

    Changeset Session::patchset() const {
        int size;
        void *data;

        auto result = sqlite3session_patchset(m_session, &size, &data);
        if(result != SQLITE_OK)
            sqlite::Error::raiseError(result, nullptr);

        return Changeset(data, size);
    }
}
