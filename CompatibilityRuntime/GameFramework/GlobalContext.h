#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include <ELF/Image.h>

#include <Translator/ThunkManager.h>
#include <Translator/DiversionManager.h>
#include <Translator/JIT.h>

class GlobalContext {
public:
    GlobalContext();
    ~GlobalContext();

    GlobalContext(const GlobalContext &other) = delete;
    GlobalContext &operator =(const GlobalContext &other) = delete;

    inline ThunkManager &thunkManager() {
        return m_thunkManager;
    }

    inline DiversionManager &diversionManager() {
        return m_diversionManager;
    }

    inline JIT &jit() {
        return m_jit;
    }

    inline bool hasArmlib() const {
        return m_armlib.has_value();
    }

    inline const Image &armlib() const {
        return m_armlib.value();
    }

    inline const Image &il2cpp() const {
        return m_il2cpp.value();
    }

    static const uint32_t PageSize;

    static inline GlobalContext &get() {
        return GlobalContextRegisterer::get();
    }

private:
    class GlobalContextRegisterer {
    public:
        explicit GlobalContextRegisterer(GlobalContext *context);
        ~GlobalContextRegisterer();

        GlobalContextRegisterer(const GlobalContextRegisterer &other) = delete;
        GlobalContextRegisterer &operator =(const GlobalContextRegisterer &other) = delete;

        static inline GlobalContext &get() {
            auto context = m_context;
            if(!context)
                missingGlobalContext();

            return *context;
        }

    private:
        [[noreturn]] static void missingGlobalContext();

        static GlobalContext *m_context;
    };

    static std::filesystem::path thisLibraryDirectory();

    static uint32_t queryPageSize();

    GlobalContextRegisterer m_registerer;
    ThunkManager m_thunkManager;
    DiversionManager m_diversionManager;
    JIT m_jit;
    std::optional<Image> m_armlib;
    std::optional<Image> m_il2cpp;
};

#endif
