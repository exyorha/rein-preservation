#include <Interop/InteropMethodLocator.h>
#include <il2cpp-tabledefs.h>

#include "support.h"

#include <stdexcept>

const MethodInfo *InteropMethodLocator::resolveMethod(const std::string_view &name, const InteropMethodLocatorParameters &parameters) {

    auto domain = il2cpp_domain_get();

    size_t numberOfAssemblies;
    auto assemblies = il2cpp_domain_get_assemblies(domain, &numberOfAssemblies);

    const Il2CppImage *image = nullptr;

    std::string_view fullName;

    if(parameters.hasExplicitAssemblyName) {
        auto delim = name.find("::");
        if(delim == std::string_view::npos) {
            throw std::runtime_error("no assembly name delimiter in " + std::string(name));
        }

        std::string_view assemblyName(name.substr(0, delim));
        fullName = name.substr(delim + 2);

        for(size_t index = 0; index <numberOfAssemblies; index++) {
            auto assembly = assemblies[index];
            auto thisImage = il2cpp_assembly_get_image(assembly);
            auto thisAssemblyName = il2cpp_image_get_name(thisImage);

            if(thisAssemblyName == assemblyName) {
                image = thisImage;
                break;
            }
        }

        if(!image) {
            throw std::runtime_error("the assembly was not found: " + std::string(name));
        }

    } else {
        fullName = name;
    }


    auto methodNameDelim = fullName.find("::");
    if(methodNameDelim == std::string::npos)
        panic("icall method name has no class-method delimiter: %s\n", std::string(fullName).c_str());

    std::string_view namespaceClass = fullName.substr(0, methodNameDelim);

    auto namespaceDelim = namespaceClass.rfind('.');
    if(namespaceDelim >= methodNameDelim)
        panic("icall method name has no namespace-class delimiter: %s\n", std::string(namespaceClass).c_str());

    std::string namespaceName(namespaceClass.substr(0, namespaceDelim));
    std::string className(namespaceClass.substr(namespaceDelim + 1, methodNameDelim - namespaceDelim - 1));
    std::string methodName(fullName.substr(methodNameDelim + 2));

    Il2CppClass *classDef = nullptr;

    if(image) {
        classDef = il2cpp_class_from_name(image, namespaceName.c_str(), className.c_str());
    } else {
        for(size_t assemblyIndex = 0; assemblyIndex < numberOfAssemblies; assemblyIndex++) {
            auto image = il2cpp_assembly_get_image(assemblies[assemblyIndex]);

            classDef = il2cpp_class_from_name(image, namespaceName.c_str(), className.c_str());
            if(classDef)
                break;
        }
    }

    if(!classDef) {
        panic("icall: unable to find the class in any assembly when calling '%s'\n", std::string(fullName).c_str());
    }

    void *iter = nullptr;
    const MethodInfo *methodDef = nullptr;
    const MethodInfo *itMethod;

    while((itMethod = il2cpp_class_get_methods(classDef, &iter)) != nullptr) {
        uint32_t iflags;
        auto flags = il2cpp_method_get_flags(itMethod, &iflags);

        if(parameters.mustBeInternalCall && !(iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL))
            continue;

        auto name = il2cpp_method_get_name(itMethod);
        if(name == methodName) {
            if(methodDef) {
                panic("InteropMethodLocator::resolveMethod: multiple possibilites for %s", std::string(fullName).c_str());
            }

            methodDef = itMethod;
        }
    }

    if(!methodDef) {
        panic("InteropMethodLocator::resolveMethod: unable to find the method in the class: %s\n", std::string(fullName).c_str());
    }

    return methodDef;
}
