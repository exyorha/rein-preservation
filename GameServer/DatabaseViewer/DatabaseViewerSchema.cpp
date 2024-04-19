#include "DatabaseViewer/DatabaseViewerSchema.h"
#include "DatabaseViewer/DatabaseViewerSchemaParser.h"

#include "LLServices/JSON/StreamingJSONParser.h"

DatabaseViewerSchema::DatabaseViewerSchema(std::filesystem::path &&filename) : ReloadableResource(std::move(filename)) {

}

DatabaseViewerSchema::~DatabaseViewerSchema() = default;


void DatabaseViewerSchema::doReload(std::string &&content) {
    DatabaseViewerEntitySet entities;

    DatabaseViewerSchemaParser schemaParser(entities);
    LLServices::StreamingJSONParser parser(schemaParser);

    if(!parser.parseChunk(content) || !parser.finish())
        throw std::runtime_error("DatabaseViewerSchema::doReload: failed to parse");

    m_entities = std::move(entities);
}
