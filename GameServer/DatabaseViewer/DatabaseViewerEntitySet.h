#ifndef DATABASE_VIEWER_ENTITY_SET_H
#define DATABASE_VIEWER_ENTITY_SET_H

#include <vector>

#include "DatabaseViewer/DatabaseViewerEntity.h"

class DatabaseViewerEntitySet {
public:
    explicit DatabaseViewerEntitySet();
    ~DatabaseViewerEntitySet();

    DatabaseViewerEntitySet(const DatabaseViewerEntitySet &other) = delete;
    DatabaseViewerEntitySet &operator =(const DatabaseViewerEntitySet &other) = delete;

    DatabaseViewerEntitySet(DatabaseViewerEntitySet &&other) noexcept;
    DatabaseViewerEntitySet &operator =(DatabaseViewerEntitySet &&other) noexcept;

    inline const std::vector<DatabaseViewerEntity> &entities() const {
        return m_entities;
    }

    inline std::vector<DatabaseViewerEntity> &entities() {
        return m_entities;
    }

    const DatabaseViewerEntity *findEntityByName(const std::string_view &name) const;

private:
    std::vector<DatabaseViewerEntity> m_entities;
};

#endif

