#ifndef DATABASE_VIEWER_ENTITY_H
#define DATABASE_VIEWER_ENTITY_H

#include <string>
#include <memory>

class DatabaseViewerEntitySet;

class DatabaseViewerEntity {
public:
    explicit DatabaseViewerEntity(std::string &&entityName);
    ~DatabaseViewerEntity();

    DatabaseViewerEntity(const DatabaseViewerEntity &other) = delete;
    DatabaseViewerEntity &operator =(const DatabaseViewerEntity &other) = delete;

    DatabaseViewerEntity(DatabaseViewerEntity &&other) noexcept;
    DatabaseViewerEntity &operator =(DatabaseViewerEntity &&other) noexcept;

    inline const std::string &entityName() const {
        return m_entityName;
    }

    inline const DatabaseViewerEntitySet &childEntities() const {
        return *m_childEntities;
    }

    inline DatabaseViewerEntitySet &childEntities() {
        return *m_childEntities;
    }

private:
    std::string m_entityName;
    std::unique_ptr<DatabaseViewerEntitySet> m_childEntities;
};

#endif
