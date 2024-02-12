#ifndef DATA_MODEL_SQLITE_SESSION_H
#define DATA_MODEL_SQLITE_SESSION_H

#include <sqlite3.h>

#include <cstring>

namespace sqlite {
    class Database;

    class Changeset {
    public:
        Changeset() noexcept;
        Changeset(void *data, size_t size);
        ~Changeset();

        Changeset(const Changeset &other) = delete;
        Changeset &operator =(const Changeset &other) = delete;

        Changeset(Changeset &&other) noexcept;
        Changeset &operator =(Changeset &&other) noexcept;

        inline void *data() const {
            return m_data;
        }

        inline size_t size() const {
            return m_size;
        }

    private:
        void *m_data;
        size_t m_size;
    };

    struct ChangesetOperation {
        const char *tableName;
        int numberOfColumns;
        int operation;
        int indirect;
    };

    struct ChangesetPrimaryKeyDefinition {
        unsigned char *primaryKeyMask;
        int numberOfColumns;
    };

    class ChangesetIterator {
    public:
        ChangesetIterator() noexcept;
        ChangesetIterator(void *data, size_t size);
        ChangesetIterator(const Changeset &changeset);
        ~ChangesetIterator();

        ChangesetIterator(const ChangesetIterator &other) = delete;
        ChangesetIterator &operator =(const ChangesetIterator &other) = delete;

        ChangesetIterator(ChangesetIterator &&other) noexcept;
        ChangesetIterator &operator =(ChangesetIterator &&other) noexcept;

        inline operator bool() const {
            return m_iterator;
        }

        bool next(ChangesetOperation &op);

        sqlite3_value *oldValue(int column) const;
        sqlite3_value *newValue(int column) const;

        ChangesetPrimaryKeyDefinition getPrimaryKey() const;

    private:
        sqlite3_changeset_iter *m_iterator;
    };

    class Session {
    public:

        Session(sqlite3 *db, const char *databaseName);
        Session(Database &db, const char *databaseName);
        ~Session();

        Session(const Session &other) = delete;
        Session &operator =(const Session &other) = delete;

        inline operator sqlite3_session *const() {
            return m_session;
        }

        Changeset changeset() const;
        Changeset patchset() const;

    private:
        sqlite3_session *m_session;
    };

}

#endif

