#ifndef LAUNCHER_CONFIGURATION_H
#define LAUNCHER_CONFIGURATION_H

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <filesystem>

struct LauncherConfiguration {

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

    void load(const std::filesystem::path &configPath);
    void store(const std::filesystem::path &configPath) const;

private:
    using OptionEncoder = std::string (LauncherConfiguration::*)() const;
    using OptionDecoder = void (LauncherConfiguration::*)(const std::string_view &value);

    struct Option {
        const char *key;
        OptionEncoder encode;
        OptionDecoder decode;
    };

    template<bool LauncherConfiguration::*field>
    std::string encodeBool() const;

    template<bool LauncherConfiguration::*field>
    void decodeBool(const std::string_view &value);

    template<std::string LauncherConfiguration::*field>
    std::string encodeString() const;

    template<std::string LauncherConfiguration::*field>
    void decodeString(const std::string_view &value);

    template<size_t Length, std::array<char, Length> LauncherConfiguration::*field>
    std::string encodeFixedString() const;

    template<size_t Length, std::array<char, Length> LauncherConfiguration::*field>
    void decodeFixedString(const std::string_view &value);

    std::string encodeDisplayMode() const;
    void decodeDisplayMode(const std::string_view &value);

    static int parseInt(const std::string_view &value);

    static const Option m_options[];
};

#endif

