#ifndef LAUNCHER_APPLICATION_H
#define LAUNCHER_APPLICATION_H

#include "LauncherApplicationInterface.h"

#include <cstdint>
#include <vector>
#include <utility>
#include <string>

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

    std::vector<std::string> buildCommandLine() const;

    enum class DisplayMode : uint32_t {
        Fullscreen,
        Borderless,
        Window
    };

    bool m_customizeResolution = false;
    DisplayMode m_displayMode = DisplayMode::Fullscreen;
    int m_selectedResolution = 0;

    std::vector<std::pair<unsigned int, unsigned int>> m_resolutions;
    std::string m_resolutionString;
};

#endif
