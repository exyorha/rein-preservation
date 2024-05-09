#define NK_IMPLEMENTATION
#define NK_XLIB_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION

#include "nuklear_config.h"

float nuklear_inverse_sqrt(float value) {
    return 1.0f / sqrtf(value);
}

double nuklear_strtod(const char *nptr, const char **endptr) {
    return strtod(nptr, const_cast<char **>(endptr));
}

char *nuklear_dtoa(char *s, double n) {
    sprintf(s, "%f", n);

    return s;
}
