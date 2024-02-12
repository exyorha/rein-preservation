#include "GlobalContext.h"

namespace MasterDatabase {

    GlobalContext::GlobalContext() : m_references(0) {

    }

    GlobalContext::~GlobalContext() = default;

    void GlobalContext::addReference() noexcept {
        m_references.fetch_add(1);
    }

    void GlobalContext::release() noexcept {
        if(m_references.fetch_sub(1) == 1)
            delete this;
    }

    void GlobalContext::freeFunc(void *ptr) {
        static_cast<GlobalContext *>(ptr)->release();
    }

    void GlobalContext::setDatabaseFile(std::filesystem::path &&databaseFile) {
        m_database.reset();
        m_databaseFile.clear();

        if(!databaseFile.empty()) {
            m_database.emplace(databaseFile);
            m_databaseFile = std::move(databaseFile);
        }
    }

    std::shared_ptr<DatabaseTable> GlobalContext::open(const std::string_view &name) {
        if(m_database.has_value()) {
            return m_database->open(name);
        }

        return {};
    }
}
