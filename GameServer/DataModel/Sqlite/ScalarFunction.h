#ifndef DATA_MODEL_SQLITE_SCALAR_FUNCTION__H
#define DATA_MODEL_SQLITE_SCALAR_FUNCTION__H

#include <vector>

#include <DataModel/Sqlite/Function.h>

namespace sqlite {
    class ScalarFunction : public Function {
    protected:
        ScalarFunction() = default;

    public:
        ~ScalarFunction() = default;

        virtual void evaluate(Context &context, const std::vector<Value> &arguments) = 0;
    };
}

#endif
