#ifndef NUKLEAR_CONFIG_H
#define NUKLEAR_CONFIG_H

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_STANDARD_BOOL
#define NK_XLIB_USE_XFT
//#define NK_GDI_DRAW_ON_SCREEN

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

float nuklear_inverse_sqrt(float value);

double nuklear_strtod(const char *nptr, const char **endptr);
char *nuklear_dtoa(char *s, double n);

#define NK_MEMSET memset
#define NK_MEMCPY memcpy
#define NK_INV_SQRT nuklear_inverse_sqrt
#define NK_SIN sinf
#define NK_COS cosf
#define NK_STRTOD nuklear_strtod
#define NK_DTOA nuklear_dtoa
#define NK_VSNPRINTF vsnprintf

#include "nuklear.h"

#ifdef _WIN32
#include <windows.h>
#include "nuklear_gdi.h"
#else
#include "nuklear_xlib.h"
#endif

#endif

