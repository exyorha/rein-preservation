#ifndef LAUNCHER_APPLICATION_H
#define LAUNCHER_APPLICATION_H

#include "LauncherApplicationInterface.h"

#include <cstdint>
#include <vector>
#include <utility>
#include <string>
#include <array>
#include <filesystem>
#include "LauncherConfiguration.h"

struct nk_context;

class LauncherApplication final : public LauncherApplicationInterface {
public:
    LauncherApplication();
    ~LauncherApplication();

    void update(LauncherPlatform *platform) override;

private:
    using ComboItemGetter = const char *(LauncherApplication::*)(int index);

    static int autoSizedCombo(nk_context *ctx, const char **items, int itemCount, int selectedIndex);
    int autoSizedCombo(nk_context *ctx, ComboItemGetter itemGetter, int itemCount, int selectedIndex);

    const char *resolutionListItem(int index);

    struct ComboCallbackContext {
        LauncherApplication *this_;
        ComboItemGetter itemGetter;
    };

    static void comboItemGetter(void *userData, int index, const char** item);

    static std::string resolutionString(const std::pair<unsigned int, unsigned int> &resolution);

    LauncherConfiguration m_config;

    std::vector<std::pair<unsigned int, unsigned int>> m_resolutions;
    std::filesystem::path m_dataPathFS;
    std::string m_dataPath;
    std::string m_resolutionString;

};

#endif
