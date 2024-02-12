#ifndef BIONIC_BIONIC_ANDROID_API_H
#define BIONIC_BIONIC_ANDROID_API_H

int plat_system_property_get(const char *name, char *value);
int plat_write_log(int priority, const char* tag, const char* msg);

#endif
