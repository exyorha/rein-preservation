#include "DatabaseViewer/DatabaseViewerEntity.h"
#include "DatabaseViewer/DatabaseViewerEntitySet.h"

DatabaseViewerEntity::DatabaseViewerEntity(std::string &&entityName) :
    m_entityName(std::move(entityName)),
    m_childEntities(std::make_unique<DatabaseViewerEntitySet>()) {

}

DatabaseViewerEntity::~DatabaseViewerEntity() = default;

DatabaseViewerEntity::DatabaseViewerEntity(DatabaseViewerEntity &&other) noexcept = default;

DatabaseViewerEntity &DatabaseViewerEntity::operator =(DatabaseViewerEntity &&other) noexcept = default;
