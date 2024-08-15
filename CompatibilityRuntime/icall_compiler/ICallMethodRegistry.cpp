#include "ICallMethodRegistry.h"

#include <il2cpp-tabledefs.h>

#include <optional>
#include <algorithm>
#include <stdexcept>

ICallMethodRegistry::ICallMethodRegistry() {

    auto domain = il2cpp_domain_get();
    size_t assemblyCount;
    auto assemblies = il2cpp_domain_get_assemblies(domain, &assemblyCount);
    for(size_t assemblyIndex = 0; assemblyIndex < assemblyCount; assemblyIndex++) {
        auto image = il2cpp_assembly_get_image(assemblies[assemblyIndex]);

        auto classCount = il2cpp_image_get_class_count(image);
        for(size_t classIndex = 0; classIndex < classCount; classIndex++) {
            auto assemblyClass = const_cast<Il2CppClass *>(il2cpp_image_get_class(image, classIndex));
            addClass(assemblyClass);
        }
    }

    std::sort(m_methods.begin(), m_methods.end(), [](const ICallMethod &a, const ICallMethod &b) -> bool {
        return a.cName() < b.cName();
    });

    if(!m_methods.empty()) {
        auto last = m_methods.end();
        --last;
        for(auto it = m_methods.begin(); it != last; ++it) {
            auto next = it;
            ++next;

            if(it->cName() == next->cName()) {
                throw std::runtime_error("duplicate icall method C name: " + it->cName());
            }
        }
    }
}

ICallMethodRegistry::~ICallMethodRegistry() = default;

void ICallMethodRegistry::addClass(Il2CppClass* klass) {
    std::optional<std::string> className;

    void *iterator = nullptr;
    const MethodInfo *methodInfo;

    while((methodInfo = il2cpp_class_get_methods(klass, &iterator)) != nullptr) {
        uint32_t flags, iflags;
        flags = il2cpp_method_get_flags(methodInfo, &iflags);
        if(iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL) {
            if(!className.has_value()) {
                className.emplace(makeClassName(klass));
            }

            if(!isInterestingClass(*className))
                continue;

            addMethod(*className, methodInfo);
        }
    }
}

bool ICallMethodRegistry::isInterestingClass(const std::string &className) {
    return className.starts_with("Unity.") || className.starts_with("UnityEngine.") || className.starts_with("UnityEngineInternal.");
}

void ICallMethodRegistry::addMethod(const std::string &className, const MethodInfo *method) {
    std::string composedName = className;
    composedName.append("::");
    composedName.append(il2cpp_method_get_name(method));

    m_methods.emplace_back(std::move(composedName), method);
}

std::string ICallMethodRegistry::makeClassName(Il2CppClass* klass) {
    std::string name;

    auto outer = il2cpp_class_get_declaring_type(klass);
    if(outer) {
        name = makeClassName(outer);
        name.push_back('/');
    }

    auto namespaceName = il2cpp_class_get_namespace(klass);
    if(namespaceName && *namespaceName) {
        name.append(namespaceName);
        name.push_back('.');
    }

    name.append(il2cpp_class_get_name(klass));

    return name;
}
