#ifndef DATA_MODEL_SQLITE_FUNCTION_H
#define DATA_MODEL_SQLITE_FUNCTION_H

namespace sqlite {
    class Context;
    class Value;

    class Function {
    protected:
        Function() = default;

    public:
        virtual ~Function() = default;

        Function(const Function &other) = delete;
        Function &operator =(const Function &other) = delete;

        virtual const char *name() const = 0;
        virtual int arguments() const = 0;
        virtual int textRepresentation() const = 0;
    };
}

#endif
