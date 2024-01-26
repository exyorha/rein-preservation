#ifndef ICALL_SAVED_ICALL_CONTEXT_H
#define ICALL_SAVED_ICALL_CONTEXT_H

#include <dynarmic/interface/A64/a64.h>

class JITThreadContext;

class SavedICallContext {
public:
    explicit SavedICallContext(const JITThreadContext &thread);
    ~SavedICallContext();

    SavedICallContext(const SavedICallContext &other) = delete;
    SavedICallContext &operator =(const SavedICallContext &other) = delete;

    std::array<std::uint64_t, 8> intArgs;
    std::array<Dynarmic::A64::Vector, 8> vectorArgs;
    uint64_t sp;
};

#endif
