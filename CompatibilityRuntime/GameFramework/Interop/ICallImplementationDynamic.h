#ifndef ICALL_IMPLEMENTATION_DYNAMIC_H
#define ICALL_IMPLEMENTATION_DYNAMIC_H

#include "ICallImplementation.h"

class ICallImplementationDynamic final : public ICallImplementation {
public:
    ICallImplementationDynamic();
    ~ICallImplementationDynamic() override;

    void *installICallHandler(const char *name, Il2CppMethodPointer implementation) override;
    Il2CppMethodPointer resolveNativeICall(const char *name, void *handler) override;
};

#endif
