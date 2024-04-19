#ifndef DATABASE_VIEWER_DATABASE_VIEWER_SCHEMA_PARSER_H
#define DATABASE_VIEWER_DATABASE_VIEWER_SCHEMA_PARSER_H

#include "LLServices/JSON/StreamingJSONCallback.h"

#include <vector>

class DatabaseViewerEntitySet;
class DatabaseViewerEntity;

class DatabaseViewerSchemaParser final : public LLServices::StreamingJSONCallback {
public:
    explicit DatabaseViewerSchemaParser(DatabaseViewerEntitySet &rootEntitySet);
    ~DatabaseViewerSchemaParser();

    DatabaseViewerSchemaParser(const DatabaseViewerSchemaParser &other) = delete;
    DatabaseViewerSchemaParser &operator =(const DatabaseViewerSchemaParser &other) = delete;

    bool null() override;
    bool boolean(bool value) override;
    bool integer(int64_t value) override;
    bool doubleValue(double value) override;
    bool string(const std::string_view &value) override;
    bool startMap() override;
    bool mapKey(const std::string_view &key) override;
    bool endMap() override;
    bool startArray() override;
    bool endArray() override;

private:
    enum class State {
        Initial,
        InEntityMap,
        ReceivedEntityKey,
        InEntity
    };

    DatabaseViewerEntitySet &getCurrentEntitySet();

    State m_state;
    DatabaseViewerEntitySet &m_rootEntitySet;
    std::vector<DatabaseViewerEntity *> m_buildingEntities;
};

#endif

