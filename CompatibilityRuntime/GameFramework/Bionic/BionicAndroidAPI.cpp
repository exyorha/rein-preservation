#include <Bionic/BionicAndroidAPI.h>

#include <cerrno>

#include <Bionic/BionicCallouts.h>

static int android_system_property_get(const char *name, char *value) {
    //printf("android_system_property_get(%s)\n", name);
    *value = 0;

    return 0;
}

int plat_system_property_get(const char *name, char *value) {
    auto result = android_system_property_get(name, value);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_write_log(int priority, const char* tag, const char* msg) {
    printf("Android log: prio %d, tag '%s': %s\n", priority, tag, msg);

    return 0;
}
