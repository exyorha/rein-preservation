#ifndef DATA_MODEL_SQLITE_CONTEXT__H
#define DATA_MODEL_SQLITE_CONTEXT__H

struct sqlite3_context;

namespace sqlite {
    class Context {
    public:
        inline Context(sqlite3_context *context) : m_handle(context) { }

        Context(const Context &other) = delete;
        Context &operator =(const Context &other) = delete;

        inline sqlite3_context *handle() const { return m_handle; }
        operator sqlite3_context *() const { return m_handle; }

    private:
        sqlite3_context *m_handle;
    };
}

#endif
