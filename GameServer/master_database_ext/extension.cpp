#include <master_database_ext.h>

#include <exception>
#include <cstdio>

#include <sqlite3ext.h>

#include "GlobalContext.h"
#include "MasterDatabaseTable.h"
#include "MasterDatabaseCursor.h"
#include "MasterDatabaseControlTable.h"
#include "MasterDatabaseControlCursor.h"

SQLITE_EXTENSION_INIT1

template<typename T, typename... Args>
static int sqliteGuard(const char *name, char **pszErrMsg, const T &func, Args... args) {
    *pszErrMsg = nullptr;

    try {
        auto rc = func(args...);

        if(rc != SQLITE_OK)
            return rc;

        return rc;

    } catch(const std::bad_alloc &) {
        *pszErrMsg = sqlite3_mprintf("%s: out of memory", name);
        return SQLITE_NOMEM;
    } catch(const std::exception &e) {
        *pszErrMsg = sqlite3_mprintf("%s: exception: %s", name, e.what());
        return SQLITE_ERROR;
    } catch(...) {
        *pszErrMsg = sqlite3_mprintf("%s: exception", name);
        return SQLITE_ERROR;
    }
}

template<typename T, typename... Args>
static int sqliteGuard(const char *name, const T &func, Args... args) {
    char *errorMsg = nullptr;

    auto result = sqliteGuard(name, &errorMsg, func, args...);

    if(errorMsg) {
        fprintf(stderr, "master_database_ext: an exception has occurred, but couldn't be reported: %s\n", errorMsg);
    }

    return result;
}

static int masterDatabaseCreate(sqlite3 *db, void *pAux, int argc, const char *const*argv, sqlite3_vtab **ppVtab, char **pzErr) {
    return sqliteGuard("masterDatabaseCreate", pzErr, [](sqlite3 *db, void *pAux, int argc, const char *const*argv, sqlite3_vtab **ppVtab) -> int {
        auto global = static_cast<MasterDatabase::GlobalContext *>(pAux);

        *ppVtab = new MasterDatabase::MasterDatabaseTable(global, db, argc, argv);

        return SQLITE_OK;
    }, db, pAux, argc, argv, ppVtab);
}

static int masterDatabaseConnect(sqlite3 *db, void *pAux, int argc, const char *const*argv, sqlite3_vtab **ppVtab, char **pzErr) {
    return masterDatabaseCreate(db, pAux, argc, argv, ppVtab, pzErr);
}

static int masterDatabaseBestIndex(sqlite3_vtab *vtab, sqlite3_index_info *indexInfo) {
    return sqliteGuard("masterDatabaseBestIndex", [](sqlite3_vtab *vtab, sqlite3_index_info *indexInfo) -> int {
        static_cast<MasterDatabase::MasterDatabaseTable *>(vtab)->bestIndex(indexInfo);

        return SQLITE_OK;
    }, vtab, indexInfo);
}

static int masterDatabaseDisconnect(sqlite3_vtab *vtab) {

    delete static_cast<MasterDatabase::MasterDatabaseTable *>(vtab);

    return SQLITE_OK;
}

static int masterDatabaseOpen(sqlite3_vtab *vtab, sqlite3_vtab_cursor **cursor) {
    return sqliteGuard("masterDatabaseOpen", [](sqlite3_vtab *vtab, sqlite3_vtab_cursor **cursor) -> int {
        *cursor = new MasterDatabase::MasterDatabaseCursor(static_cast<MasterDatabase::MasterDatabaseTable *>(vtab));

        return SQLITE_OK;
    }, vtab, cursor);
}

static int masterDatabaseClose(sqlite3_vtab_cursor *cursor) {

    delete static_cast<MasterDatabase::MasterDatabaseCursor *>(cursor);

    return SQLITE_OK;
}

static int masterDatabaseFilter(sqlite3_vtab_cursor *cursor, int idxNum, const char *idxStr, int argc, sqlite3_value **argv) {
    return sqliteGuard("masterDatabaseFilter", [](sqlite3_vtab_cursor *cursor, int idxNum, const char *idxStr, int argc, sqlite3_value **argv) -> int {
        static_cast<MasterDatabase::MasterDatabaseCursor *>(cursor)->filter(idxNum, idxStr, argc, argv);

        return SQLITE_OK;
    }, cursor, idxNum, idxStr, argc, argv);
}

static int masterDatabaseNext(sqlite3_vtab_cursor *cursor) {
    return sqliteGuard("masterDatabaseNext", [](sqlite3_vtab_cursor *cursor) -> int {

        static_cast<MasterDatabase::MasterDatabaseCursor *>(cursor)->next();

        return SQLITE_OK;
    }, cursor);

    return SQLITE_OK;
}

static int masterDatabaseEof(sqlite3_vtab_cursor *cursor) {
    return static_cast<MasterDatabase::MasterDatabaseCursor *>(cursor)->atEof();
}

static int masterDatabaseColumn(sqlite3_vtab_cursor *cursor, sqlite3_context *ctx, int i) {
    return sqliteGuard("masterDatabaseColumn", [](sqlite3_vtab_cursor *cursor, sqlite3_context *ctx, int i) -> int {
        static_cast<MasterDatabase::MasterDatabaseCursor *>(cursor)->column(ctx, i);

        return SQLITE_OK;
    }, cursor, ctx, i);
}

static int masterDatabaseRowid(sqlite3_vtab_cursor *cursor, sqlite_int64 *pRowId) {
    return sqliteGuard("masterDatabaseRowid", [](sqlite3_vtab_cursor *cursor, sqlite_int64 *pRowId) -> int {
        *pRowId = static_cast<MasterDatabase::MasterDatabaseCursor *>(cursor)->rowid();

        return SQLITE_OK;
    }, cursor, pRowId);
}

static const sqlite3_module masterDatabaseModule = {
    .iVersion = 0,
    .xCreate = masterDatabaseCreate,
    .xConnect = masterDatabaseConnect,
    .xBestIndex = masterDatabaseBestIndex,
    .xDisconnect = masterDatabaseDisconnect,
    .xDestroy = masterDatabaseDisconnect,
    .xOpen = masterDatabaseOpen,
    .xClose = masterDatabaseClose,
    .xFilter = masterDatabaseFilter,
    .xNext = masterDatabaseNext,
    .xEof = masterDatabaseEof,
    .xColumn = masterDatabaseColumn,
    .xRowid = masterDatabaseRowid,
};

static int masterDatabaseControlConnect(sqlite3 *db, void *pAux, int argc, const char *const*argv, sqlite3_vtab **ppVtab, char **pzErr) {
    return sqliteGuard("masterDatabaseControlConnect", pzErr, [](sqlite3 *db, void *pAux, int argc, const char *const*argv, sqlite3_vtab **ppVtab) -> int {
        auto global = static_cast<MasterDatabase::GlobalContext *>(pAux);

        *ppVtab = new MasterDatabase::MasterDatabaseControlTable(global, db);

        return SQLITE_OK;
    }, db, pAux, argc, argv, ppVtab);
}

static int masterDatabaseControlBestIndex(sqlite3_vtab *vtab, sqlite3_index_info *indexInfo) {
    return sqliteGuard("masterDatabaseControlBestIndex", [](sqlite3_vtab *vtab, sqlite3_index_info *indexInfo) -> int {
        static_cast<MasterDatabase::MasterDatabaseControlTable *>(vtab)->bestIndex(indexInfo);

        return SQLITE_OK;
    }, vtab, indexInfo);
}

static int masterDatabaseControlDisconnect(sqlite3_vtab *vtab) {

    delete static_cast<MasterDatabase::MasterDatabaseControlTable *>(vtab);

    return SQLITE_OK;
}

static int masterDatabaseControlOpen(sqlite3_vtab *vtab, sqlite3_vtab_cursor **cursor) {
    return sqliteGuard("masterDatabaseControlOpen", [](sqlite3_vtab *vtab, sqlite3_vtab_cursor **cursor) -> int {
        *cursor = new MasterDatabase::MasterDatabaseControlCursor(static_cast<MasterDatabase::MasterDatabaseControlTable *>(vtab));

        return SQLITE_OK;
    }, vtab, cursor);
}

static int masterDatabaseControlClose(sqlite3_vtab_cursor *cursor) {

    delete static_cast<MasterDatabase::MasterDatabaseControlCursor *>(cursor);

    return SQLITE_OK;
}

static int masterDatabaseControlFilter(sqlite3_vtab_cursor *cursor, int idxNum, const char *idxStr, int argc, sqlite3_value **argv) {
    return sqliteGuard("masterDatabaseControlFilter", [](sqlite3_vtab_cursor *cursor, int idxNum, const char *idxStr, int argc, sqlite3_value **argv) -> int {
        static_cast<MasterDatabase::MasterDatabaseControlCursor *>(cursor)->filter(idxNum, idxStr, argc, argv);

        return SQLITE_OK;
    }, cursor, idxNum, idxStr, argc, argv);
}

static int masterDatabaseControlNext(sqlite3_vtab_cursor *cursor) {
    return sqliteGuard("masterDatabaseControlNext", [](sqlite3_vtab_cursor *cursor) -> int {

        static_cast<MasterDatabase::MasterDatabaseControlCursor *>(cursor)->next();

        return SQLITE_OK;
    }, cursor);

    return SQLITE_OK;
}

static int masterDatabaseControlEof(sqlite3_vtab_cursor *cursor) {
    return static_cast<MasterDatabase::MasterDatabaseControlCursor *>(cursor)->atEof();
}

static int masterDatabaseControlColumn(sqlite3_vtab_cursor *cursor, sqlite3_context *ctx, int i) {
    return sqliteGuard("masterDatabaseControlColumn", [](sqlite3_vtab_cursor *cursor, sqlite3_context *ctx, int i) -> int {
        static_cast<MasterDatabase::MasterDatabaseControlCursor *>(cursor)->column(ctx, i);

        return SQLITE_OK;
    }, cursor, ctx, i);
}

static int masterDatabaseControlRowid(sqlite3_vtab_cursor *cursor, sqlite_int64 *pRowId) {
    return sqliteGuard("masterDatabaseControlRowid", [](sqlite3_vtab_cursor *cursor, sqlite_int64 *pRowId) -> int {
        *pRowId = static_cast<MasterDatabase::MasterDatabaseControlCursor *>(cursor)->rowid();

        return SQLITE_OK;
    }, cursor, pRowId);
}

static int masterDatabaseControlUpdate(sqlite3_vtab *vtable, int argc, sqlite3_value **argv, sqlite3_int64 *pRowId) {
    return sqliteGuard("masterDatabaseControlUpdate", [](sqlite3_vtab *vtable, int argc, sqlite3_value **argv, sqlite3_int64 *pRowId) -> int {
        *pRowId = static_cast<MasterDatabase::MasterDatabaseControlTable *>(vtable)->update(argc, argv);

        return SQLITE_OK;
    }, vtable, argc, argv, pRowId);
}

static const sqlite3_module masterDatabaseControlModule = {
    .iVersion = 0,
    .xCreate = nullptr,
    .xConnect = masterDatabaseControlConnect,
    .xBestIndex = masterDatabaseControlBestIndex,
    .xDisconnect = masterDatabaseControlDisconnect,
    .xDestroy = masterDatabaseControlDisconnect,
    .xOpen = masterDatabaseControlOpen,
    .xClose = masterDatabaseControlClose,
    .xFilter = masterDatabaseControlFilter,
    .xNext = masterDatabaseControlNext,
    .xEof = masterDatabaseControlEof,
    .xColumn = masterDatabaseControlColumn,
    .xRowid = masterDatabaseControlRowid,
    .xUpdate = masterDatabaseControlUpdate
};

int sqlite3_masterdatabaseext_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
) {
    SQLITE_EXTENSION_INIT2(pApi);

    return sqliteGuard("sqlite3_masterdatabaseext_init", pzErrMsg, [](sqlite3 *db) -> int {

        auto context = new MasterDatabase::GlobalContext();

        context->addReference();
        int res = sqlite3_create_module_v2(db, "masterdatabase", &masterDatabaseModule,
                                        context, MasterDatabase::GlobalContext::freeFunc);
        if(res != SQLITE_OK)
            return res;

        context->addReference();
        return sqlite3_create_module_v2(db, "masterdatabase_ctrl", &masterDatabaseControlModule,
                                        context, MasterDatabase::GlobalContext::freeFunc);
    }, db);
}
