#include <DataModel/Sqlite/Database.h>
#include <DataModel/Sqlite/Error.h>
#include <DataModel/Sqlite/Collation.h>
#include <DataModel/Sqlite/Blob.h>
#include <DataModel/Sqlite/ScalarFunction.h>
#include <DataModel/Sqlite/AggregateFunction.h>
#include <DataModel/Sqlite/Context.h>
#include <DataModel/Sqlite/Value.h>
#include <DataModel/Sqlite/Statement.h>
#include <DataModel/Sqlite/UpdateHookListener.h>

namespace sqlite {

    Database::Database(const std::filesystem::path &database, int flags, const std::string &vfs) : m_flattenNestedTransactions(false), m_transactionDepth(0) {

        int status = sqlite3_open_v2(
            reinterpret_cast<const char *>(database.generic_u8string().c_str()),
            &m_handle, flags, vfs.empty() ? nullptr : vfs.c_str());

        if (status != SQLITE_OK) {
            struct HandleHolder {
                HandleHolder(sqlite3 *_handle) : handle(_handle) {}
                ~HandleHolder() {
                    sqlite3_close_v2(handle);
                }

                sqlite3 *handle;
            } handleHolder(m_handle);

            Error::raiseError(status, m_handle);
        }

        sqlite3_extended_result_codes(m_handle, 1);
    }

    Database::~Database() {
        int status = sqlite3_close_v2(m_handle);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_handle);
    }

	void Database::setUpdateHookListener(UpdateHookListener* listener) {
		if (listener) {
			sqlite3_update_hook(m_handle, updateHookThunk, listener);
		}
		else {
			sqlite3_update_hook(m_handle, nullptr, nullptr);
		}
	}

	void Database::updateHookThunk(void* context, int event, char const* database, char const* tableName, sqlite3_int64 rowId) {
		static_cast<UpdateHookListener*>(context)->updateEvent(event, database, tableName, rowId);
	}

    std::unique_ptr<Blob> Database::openBlob(const std::string &database, const std::string &table, const std::string &column, sqlite3_int64 row, int flags) {
        std::unique_ptr<Blob> recipient = std::unique_ptr<Blob>(new Blob(m_handle));

        sqlite3_blob *blob;
        int status = sqlite3_blob_open(m_handle, database.c_str(), table.c_str(), column.c_str(), row, flags, &blob);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_handle);

        recipient->acceptHandle(blob);

        return recipient;
    }

    int Database::changes() const {
        return sqlite3_changes(m_handle);
    }

    void Database::createCollation(std::unique_ptr<Collation> &&collation) {
        int status = sqlite3_create_collation_v2(m_handle,
            collation->name(),
            collation->textRepresentation(),
            collation.get(),
            callbackCollationCompare,
            callbackCollationDestroy);

        if (status != SQLITE_OK)
            Error::raiseError(status, m_handle);

        // sqlite3 now owns collation

        (void)collation.release();
    }

    int Database::callbackCollationCompare(void *handle, int string1Len, const void *string1, int string2Len, const void *string2) {
        return static_cast<Collation *>(handle)->compare(string1Len, string1, string2Len, string2);
    }

    void Database::callbackCollationDestroy(void *handle) {
        delete static_cast<Collation *>(handle);
    }

    void Database::createFunction(std::unique_ptr<AggregateFunction> &&function) {
        // sqlite3 takes ownership of the function immediately

        auto rawFunction = function.release();
        int status = sqlite3_create_function_v2(m_handle, rawFunction->name(), rawFunction->arguments(), rawFunction->textRepresentation(),
            rawFunction, nullptr, callbackFunctionStep, callbackFunctionFinal, callbackFunctionDestroy);

        if (status != SQLITE_OK)
            Error::raiseError(status, m_handle);
    }

    void Database::createFunction(std::unique_ptr<ScalarFunction> &&function) {
        // sqlite3 takes ownership of the function immediately

        auto rawFunction = function.release();
        int status = sqlite3_create_function_v2(m_handle, rawFunction->name(), rawFunction->arguments(), rawFunction->textRepresentation(),
            rawFunction, callbackFunctionValue, nullptr, nullptr, callbackFunctionDestroy);

        if (status != SQLITE_OK)
            Error::raiseError(status, m_handle);
    }


    void Database::callbackFunctionValue(sqlite3_context *context, int valueCount, sqlite3_value **values) {
        auto function = static_cast<ScalarFunction *>(sqlite3_user_data(context));
        Context wrappedContext(context);
        std::vector<Value> wrappedValues(values, values + valueCount);
        function->evaluate(wrappedContext, wrappedValues);
    }

    void Database::callbackFunctionStep(sqlite3_context *context, int valueCount, sqlite3_value **values) {
        auto function = static_cast<AggregateFunction *>(sqlite3_user_data(context));
        Context wrappedContext(context);
        std::vector<Value> wrappedValues(values, values + valueCount);
        function->step(wrappedContext, wrappedValues);
    }

    void Database::callbackFunctionFinal(sqlite3_context *context) {
        auto function = static_cast<AggregateFunction *>(sqlite3_user_data(context));
        Context wrappedContext(context);
        function->final(wrappedContext);
    }

    void Database::callbackFunctionDestroy(void *handle) {
        delete static_cast<Function *>(handle);
    }

    std::u8string Database::filename(const std::string &databaseName) const {
        const char8_t *rawFilename = reinterpret_cast<const char8_t *>(sqlite3_db_filename(m_handle, databaseName.c_str()));
        if (rawFilename)
            return rawFilename;
        else
            return std::u8string();
    }

    bool Database::readonly(const std::string &databaseName) const {
        int result = sqlite3_db_readonly(m_handle, databaseName.c_str());
        if (result < 0) {
            Error::raiseError(SQLITE_ERROR, m_handle);
        }

        return result > 0;
    }

    void Database::releaseMemory() {
        int result = sqlite3_db_release_memory(m_handle);
        if (result != SQLITE_OK)
            Error::raiseError(result, m_handle);
    }

    bool Database::isInAutocommitMode() const {
        return sqlite3_get_autocommit(m_handle) != 0;
    }

    void Database::interrupt() {
        sqlite3_interrupt(m_handle);
    }

    sqlite3_int64 Database::lastInsertRowid() const {
        return sqlite3_last_insert_rowid(m_handle);
    }

    std::unique_ptr<Statement> Database::prepare(const std::string_view &sql, unsigned int flags, const char **tail) {
        std::unique_ptr<Statement> recipient = std::unique_ptr<Statement>(new Statement(m_handle));

        sqlite3_stmt *statement;
        int status = sqlite3_prepare_v3(m_handle, sql.data(), static_cast<int>(sql.size()), flags, &statement, tail);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_handle);

        if (!statement)
            return nullptr;

        recipient->acceptHandle(statement);

        return recipient;
    }

    void Database::createSavepoint(const std::string &savepoint) {
        if (m_transactionDepth == 0 || !m_flattenNestedTransactions) {
            auto statement = prepare("SAVEPOINT " + savepoint);
            statement->step();
        }

        m_transactionDepth++;
    }

    void Database::rollbackToSavepoint(const std::string &savepoint) {
        if (m_transactionDepth == 1 || !m_flattenNestedTransactions) {
            auto statement = prepare("ROLLBACK TO SAVEPOINT " + savepoint);
            statement->step();
        }

    }

    void Database::releaseSavepoint(const std::string &savepoint) {
        if (m_transactionDepth == 1 || !m_flattenNestedTransactions) {
            auto statement = prepare("RELEASE SAVEPOINT " + savepoint);
            statement->step();
        }

        m_transactionDepth--;
    }
	
	void Database::busyTimeout(int timeout) {
		auto result = sqlite3_busy_timeout(m_handle, timeout);
		if (result != SQLITE_OK) {
			Error::raiseError(result, m_handle);
		}
	}

    void Database::checkpoint(const char *databaseName, int mode, int *outLogSize, int *framesCheckpointed) {
        auto result = sqlite3_wal_checkpoint_v2(m_handle, databaseName, mode, outLogSize, framesCheckpointed);
        if(result != SQLITE_OK) {
			Error::raiseError(result, m_handle);
		}
    }
}
