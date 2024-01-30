#include <ICall/ICallInterposerManager.h>
#include <mutex>

#include <translator_api.h>

std::shared_mutex ICallInterposerManager::m_tableMutex;
std::optional<ICallInterposerManager::InterposerTable> ICallInterposerManager::m_table;

Il2CppMethodPointer ICallInterposerManager::getInterposerForMethod(const std::string_view &name) {
    {
        std::shared_lock<std::shared_mutex> sharedLocker(m_tableMutex);

        if(m_table.has_value())
            return getInterposerForMethodLocked(name);
    }

    {
        std::unique_lock<std::shared_mutex> uniqueLocker(m_tableMutex);

        if(!m_table.has_value())
            m_table.emplace();

        return getInterposerForMethodLocked(name);
    }
}

void ICallInterposerManager::addInterposerForMethod(const std::string_view &name, Il2CppMethodPointer interposer) {
    std::unique_lock<std::shared_mutex> uniqueLocker(m_tableMutex);

    if(!m_table.has_value())
        m_table.emplace();

    m_table->emplace(name, interposer);
}

void ICallInterposerManager::removeInterposerForMethod(const std::string_view &name) {
    std::unique_lock<std::shared_mutex> uniqueLocker(m_tableMutex);

    if(!m_table.has_value())
        m_table.emplace();

    m_table->erase(name);
}

Il2CppMethodPointer ICallInterposerManager::getInterposerForMethodLocked(const std::string_view &name) {
    auto it = m_table->find(name);
    if(it == m_table->end())
        return nullptr;

    return it->second;
}

void translator_add_icall_interposer(const char *name, Il2CppMethodPointer interposer) {
    ICallInterposerManager::addInterposerForMethod(name, interposer);
}

void translator_remove_icall_interposer(const char *name) {
    ICallInterposerManager::removeInterposerForMethod(name);
}

