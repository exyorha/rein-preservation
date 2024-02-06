#ifndef DATA_MODEL_SQLITE_AGGREGATE_FUNCTION_H
#define DATA_MODEL_SQLITE_AGGREGATE_FUNCTION_H

#include <vector>

#include <DataModel/Sqlite/Function.h>

namespace sqlite {
    class AggregateFunction : public Function {
    public:
        virtual void step(Context &context, const std::vector<Value> &columns) = 0;
        virtual void final(Context &context) = 0;
    };
}

#endif
