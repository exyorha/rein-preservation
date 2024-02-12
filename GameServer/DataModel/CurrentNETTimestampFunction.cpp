#include <DataModel/CurrentNETTimestampFunction.h>

#include <DataModel/Sqlite/Context.h>

#include <sqlite3.h>

#include <chrono>

CurrentNETTimestampFunction::CurrentNETTimestampFunction() = default;

CurrentNETTimestampFunction::~CurrentNETTimestampFunction() = default;

const char *CurrentNETTimestampFunction::name() const {
    return "current_net_timestamp";
}

int CurrentNETTimestampFunction::arguments() const {
    return 0;

}

int CurrentNETTimestampFunction::textRepresentation() const {
    return SQLITE_UTF8;
}

void CurrentNETTimestampFunction::evaluate(sqlite::Context &context, const std::vector<sqlite::Value> &arguments) {
    sqlite3_result_int64(context, std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}
