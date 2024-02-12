#ifndef MASTER_DATABASE_EXT_H
#define MASTER_DATABASE_EXT_H

#include <master_database_ext_export.h>

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_api_routines sqlite3_api_routines;

#if defined(__cplusplus)
extern "C" {
#endif

int MASTER_DATABASE_EXT_EXPORT sqlite3_masterdatabaseext_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
);

#if defined(__cplusplus)
}
#endif

#endif

