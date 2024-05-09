#ifndef LAUNCHER_APPLICATION_INTERFACE_H
#define LAUNCHER_APPLICATION_INTERFACE_H

class LauncherPlatform;

class LauncherApplicationInterface {
protected:
    LauncherApplicationInterface();
    ~LauncherApplicationInterface();

public:
    LauncherApplicationInterface(const LauncherApplicationInterface &other) = delete;
    LauncherApplicationInterface &operator =(const LauncherApplicationInterface &other) = delete;

    virtual void update(LauncherPlatform *platform) = 0;
};

#endif
