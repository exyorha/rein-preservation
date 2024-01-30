#ifndef ICALL_INTERPOSER_MANAGER_H
#define ICALL_INTERPOSER_MANAGER_H

#include <il2cpp-api.h>

#include <string_view>
#include <unordered_map>
#include <shared_mutex>
#include <optional>

class ICallInterposerManager {
public:
    static Il2CppMethodPointer getInterposerForMethod(const std::string_view &name);

    static void addInterposerForMethod(const std::string_view &name, Il2CppMethodPointer interposer);
    static void removeInterposerForMethod(const std::string_view &name);

private:
    using InterposerTable = std::unordered_map<std::string_view, Il2CppMethodPointer>;

    static Il2CppMethodPointer getInterposerForMethodLocked(const std::string_view &name);

    static std::shared_mutex m_tableMutex;
    static std::optional<InterposerTable> m_table;
};

#endif
