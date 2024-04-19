#ifndef DATABASE_VIEWER_DATABASE_VIEWER_SCHEMA_H
#define DATABASE_VIEWER_DATABASE_VIEWER_SCHEMA_H

#include "DatabaseViewer/ReloadableResource.h"

#include "DatabaseViewerEntitySet.h"

class DatabaseViewerSchema final : public ReloadableResource {
public:
    explicit DatabaseViewerSchema(std::filesystem::path &&filename);
    ~DatabaseViewerSchema();

    inline const DatabaseViewerEntitySet &entities() const {
        return m_entities;
    }

protected:
    void doReload(std::string &&content) override;

private:
    DatabaseViewerEntitySet m_entities;
};

#endif
