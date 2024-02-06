#ifndef DATA_MODEL_SQLITE_DATABASE_H
#define DATA_MODEL_SQLITE_DATABASE_H

#include <string>
#include <memory>
#include <string_view>
#include <filesystem>

#include <sqlite3.h>

namespace sqlite {
    class Blob;
    class Collation;
    class AggregateFunction;
    class ScalarFunction;
    class Statement;
    class UpdateHookListener;

    class Database {
    public:
        Database(const std::filesystem::path &database, int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, const std::string &vfs = std::string());
        ~Database();

        Database(const Database &other) = delete;
        Database &operator =(const Database &other) = delete;

        inline sqlite3 *handle() const { return m_handle; }
        inline operator sqlite3 *() const { return m_handle; }

        std::string filename(const std::string &databaseName) const;
        bool readonly(const std::string &databaseName) const;
        bool isInAutocommitMode() const;
        int changes() const;
        sqlite3_int64 lastInsertRowid() const;

        void interrupt();

        void releaseMemory();

        void createCollation(std::unique_ptr<Collation> &&collation);
        void createFunction(std::unique_ptr<AggregateFunction> &&function);
        void createFunction(std::unique_ptr<ScalarFunction> &&function);

        std::unique_ptr<Blob> openBlob(const std::string &database, const std::string &table, const std::string &column, sqlite3_int64 row, int flags);

        std::unique_ptr<Statement> prepare(const std::string_view &sql, unsigned int flags = 0, const char **tail = nullptr);

        inline bool flattenNestedTransactions() const {
            return m_flattenNestedTransactions;
        }

        inline void setFlattenNestedTransactions(bool flattenNestedTransactions) {
            m_flattenNestedTransactions = flattenNestedTransactions;
        }

        void createSavepoint(const std::string &savepoint);
        void rollbackToSavepoint(const std::string &savepoint);
        void releaseSavepoint(const std::string &savepoint);

        void setUpdateHookListener(UpdateHookListener* updateHookListener);

        void busyTimeout(int timeout);

    private:
        static int callbackCollationCompare(void *handle, int string1Len, const void *string1, int string2Len, const void *string2);
        static void callbackCollationDestroy(void *handle);

        static void callbackFunctionValue(sqlite3_context *context, int valueCount, sqlite3_value **values);
        static void callbackFunctionStep(sqlite3_context *context, int valueCount, sqlite3_value **values);
        static void callbackFunctionFinal(sqlite3_context *context);
        static void callbackFunctionDestroy(void *handle);

        static void updateHookThunk(void* context, int event, char const* database, char const* tableName, sqlite3_int64 rowId);

        sqlite3 *m_handle;
        bool m_flattenNestedTransactions;
        unsigned int m_transactionDepth;
    };
}

#endif

