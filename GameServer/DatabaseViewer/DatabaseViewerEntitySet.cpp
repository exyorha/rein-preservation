#include "DatabaseViewer/DatabaseViewerEntitySet.h"

DatabaseViewerEntitySet::DatabaseViewerEntitySet() = default;

DatabaseViewerEntitySet::~DatabaseViewerEntitySet() = default;

DatabaseViewerEntitySet::DatabaseViewerEntitySet(DatabaseViewerEntitySet &&other) noexcept = default;

DatabaseViewerEntitySet &DatabaseViewerEntitySet::operator =(DatabaseViewerEntitySet &&other) noexcept = default;

const DatabaseViewerEntity *DatabaseViewerEntitySet::findEntityByName(const std::string_view &name) const {
    for(const auto &entity: m_entities) {
        if(entity.entityName() == name) {
            return &entity;
        }
    }

    return nullptr;
}
