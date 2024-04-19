#include "DatabaseViewer/DatabaseViewerSchemaParser.h"
#include "DatabaseViewer/DatabaseViewerEntitySet.h"
#include "DatabaseViewer/DatabaseViewerEntity.h"

#include <stdexcept>

DatabaseViewerSchemaParser::DatabaseViewerSchemaParser(DatabaseViewerEntitySet &rootEntitySet) :
    m_state(State::Initial),
    m_rootEntitySet(rootEntitySet) {

}

DatabaseViewerSchemaParser::~DatabaseViewerSchemaParser() = default;

bool DatabaseViewerSchemaParser::null() {
    throw std::runtime_error("unexpected null");
}

bool DatabaseViewerSchemaParser::boolean(bool value) {
    throw std::runtime_error("unexpected boolean");
}

bool DatabaseViewerSchemaParser::integer(int64_t value) {
    throw std::runtime_error("unexpected integer");
}

bool DatabaseViewerSchemaParser::doubleValue(double value) {
    throw std::runtime_error("unexpected double");
}

bool DatabaseViewerSchemaParser::string(const std::string_view &value) {
    throw std::runtime_error("unexpected string");
}

bool DatabaseViewerSchemaParser::startMap() {
    if(m_state == State::Initial) {
        m_state = State::InEntityMap;
        return true;
    } else if(m_state == State::ReceivedEntityKey) {
        m_state = State::InEntity;
        return true;
    } else {
        throw std::runtime_error("unexpected map");
    }
}

bool DatabaseViewerSchemaParser::mapKey(const std::string_view &key) {
    if(m_state == State::InEntityMap) {
        std::string name;

        if(!m_buildingEntities.empty()) {
            name = m_buildingEntities.back()->entityName();
        }

        if(!name.empty())
            name.push_back('_');

        name.append(key);

        m_buildingEntities.emplace_back(&getCurrentEntitySet().entities().emplace_back(std::move(name)));

        m_state = State::ReceivedEntityKey;

        return true;
    } else {
        throw std::runtime_error("unexpected map key");
    }
}

bool DatabaseViewerSchemaParser::endMap() {
    if(m_state == State::InEntityMap) {
        if(m_buildingEntities.empty()) {
            m_state = State::Initial;
        } else {
            m_state = State::InEntity;
        }
        return true;
    } else if(m_state == State::InEntity) {
        m_buildingEntities.pop_back();
        m_state = State::InEntityMap;
        return true;
    } else {
        throw std::runtime_error("unexpected end of map");
    }
}

bool DatabaseViewerSchemaParser::startArray() {
    throw std::runtime_error("unexpected start of array");
}

bool DatabaseViewerSchemaParser::endArray() {
    throw std::runtime_error("unexpected end of array");
}


DatabaseViewerEntitySet &DatabaseViewerSchemaParser::getCurrentEntitySet() {
    if(m_buildingEntities.empty()) {
        return m_rootEntitySet;
    } else {
        return m_buildingEntities.back()->childEntities();
    }
}
