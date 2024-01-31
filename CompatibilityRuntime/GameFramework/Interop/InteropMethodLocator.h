#ifndef INTEROP_INTEROP_METHOD_LOCATOR_H
#define INTEROP_INTEROP_METHOD_LOCATOR_H

#include <il2cpp-api.h>

#include <string_view>

struct InteropMethodLocatorParameters {
    bool hasExplicitAssemblyName = false;
    bool mustBeInternalCall = false;
};

class InteropMethodLocator {
public:
    static const MethodInfo *resolveMethod(const std::string_view &name, const InteropMethodLocatorParameters &parameters = {});
};

#endif

