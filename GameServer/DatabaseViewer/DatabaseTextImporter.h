#ifndef DATABASE_VIEWER_DATABASE_TEXT_IMPORTER_H
#define DATABASE_VIEWER_DATABASE_TEXT_IMPORTER_H

#include "DatabaseViewer/TextAssetParser.h"

#include <memory>

namespace sqlite {
    class Database;
    class Statement;
}

class DatabaseTextImporter final : public TextAssetParser {
public:
    explicit DatabaseTextImporter(sqlite::Database &db);
    ~DatabaseTextImporter();

protected:
    void addTextEntry(const std::string_view &language, const std::string_view &key, std::string &&value) override;

private:
    std::unique_ptr<sqlite::Statement> m_statement;
};

#endif

