#ifndef LAUNCHER_CONFIGURATION_H
#define LAUNCHER_CONFIGURATION_H

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <filesystem>

#include <ConfigFileParser.h>

struct LauncherConfiguration final : public ConfigFileParser {

    LauncherConfiguration();
    ~LauncherConfiguration();

    LauncherConfiguration(const LauncherConfiguration &other);
    LauncherConfiguration &operator =(const LauncherConfiguration &other);

    LauncherConfiguration(LauncherConfiguration &&other) noexcept;
    LauncherConfiguration &operator =(LauncherConfiguration &&other) noexcept;

    enum class DisplayMode : uint32_t {
        Fullscreen,
        Borderless,
        Window
    };

    bool customizeResolution = false;
    DisplayMode displayMode = DisplayMode::Fullscreen;
    bool disableTouchscreenEmulation = false;
    bool useCustomGameServer = false;
    std::array<char, 256> customGameServer = { 0 };
    bool useOpenGLES = false;
    std::string resolution;
    bool unityFramePacing = false;

    std::vector<std::string> buildCommandLine() const;

private:
    std::string encodeDisplayMode() const;
    void decodeDisplayMode(const std::string_view &value);

    static const ConfigFileParser::Option m_options[];
};

#endif

