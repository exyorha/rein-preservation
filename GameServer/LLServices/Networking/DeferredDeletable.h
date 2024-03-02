#ifndef LLSERVICES_NETWORKING_DEFERRED_DELETABLE_H
#define LLSERVICES_NETWORKING_DEFERRED_DELETABLE_H

namespace LLServices {

    class DeferredDeletable {
    protected:
        DeferredDeletable();
        ~DeferredDeletable();

    public:
        DeferredDeletable(const DeferredDeletable &other) = delete;
        DeferredDeletable &operator =(const DeferredDeletable &other) = delete;

        virtual void deferredDelete() = 0;
    };
}

#endif

