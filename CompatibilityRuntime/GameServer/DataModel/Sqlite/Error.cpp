#include <DataModel/Sqlite/Error.h>

#include <sstream>

#include <sqlite3.h>

namespace sqlite {

    Error::Error(const std::string &message, int code) : std::runtime_error(message), m_code(code) {

    }

    Error::~Error() = default;

    void Error::raiseError(int status, sqlite3 *database) {
        std::stringstream error;

        error << "SQLite error:";

        int code = status;

        if (database && sqlite3_errcode(database) == status) {
            code = sqlite3_extended_errcode(database);
            error << sqlite3_errmsg(database) << " (code " << status << ", extended code " << sqlite3_extended_errcode(database) << ")";
        }
        else {
            error << sqlite3_errstr(status) << " (code " << status << ")";
        }

        throw Error(error.str(), code);
    }
}

