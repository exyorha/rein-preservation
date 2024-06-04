#ifndef LAUNCHER_PLATFORM_H
#define LAUNCHER_PLATFORM_H

#include <memory>
#include <vector>
#include <filesystem>

#include "nuklear_config.h"

class LauncherApplicationInterface;

class LauncherPlatform {
protected:
    explicit LauncherPlatform(LauncherApplicationInterface *application);

public:
    using Resolution = std::pair<unsigned int, unsigned int>;

    virtual ~LauncherPlatform();

    virtual nk_context *nuklear() const = 0;
    virtual int run() = 0;
    virtual struct nk_rect screenRect() const = 0;
    virtual std::vector<Resolution> queryResolutionList() const = 0;
    virtual void startGameWithCommandLine(std::vector<std::string> &commandLine) = 0;
    virtual std::filesystem::path dataPath() const = 0;
    virtual void openDirectory(const std::filesystem::path &path) const = 0;

protected:
    void update();

private:
    LauncherApplicationInterface *m_application;
};


std::unique_ptr<LauncherPlatform> createLauncherPlatform(LauncherApplicationInterface *application);

#endif
