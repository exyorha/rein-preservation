#ifndef SYSTEM_API_THUNKING_H
#define SYSTEM_API_THUNKING_H

#include <string_view>

void *resolveUndefinedARMSymbol(const std::string_view &name);

#endif
