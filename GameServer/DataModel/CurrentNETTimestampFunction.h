#ifndef DATA_MODEL_CURRENT_NET_TIMESTAMP_FUNCTION_H
#define DATA_MODEL_CURRENT_NET_TIMESTAMP_FUNCTION_H

#include <DataModel/Sqlite/ScalarFunction.h>

class Database;

class CurrentNETTimestampFunction final : public sqlite::ScalarFunction {
public:
    explicit CurrentNETTimestampFunction(Database &db);
    ~CurrentNETTimestampFunction() override;

    const char *name() const override;
    int arguments() const override;
    int textRepresentation() const override;

    void evaluate(sqlite::Context &context, const std::vector<sqlite::Value> &arguments) override;

private:
    Database &m_db;
};

#endif
