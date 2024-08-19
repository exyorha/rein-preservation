#include "LauncherConfiguration.h"
#include "ConfigFileParser.h"

#include <fstream>
#include <cstring>
#include <charconv>

const ConfigFileParser::Option LauncherConfiguration::m_options[]{
    {
        .key = "customizeResolution",
        .encode = &LauncherConfiguration::encodeBool<LauncherConfiguration, &LauncherConfiguration::customizeResolution>,
        .decode = &LauncherConfiguration::decodeBool<LauncherConfiguration, &LauncherConfiguration::customizeResolution>
    },
    {
        .key = "displayMode",
        .encode = static_cast<LauncherConfiguration::OptionEncoder>(&LauncherConfiguration::encodeDisplayMode),
        .decode = static_cast<LauncherConfiguration::OptionDecoder>(&LauncherConfiguration::decodeDisplayMode),
    },
    {
        .key = "disableTouchscreenEmulation",
        .encode = &LauncherConfiguration::encodeBool<LauncherConfiguration, &LauncherConfiguration::disableTouchscreenEmulation>,
        .decode = &LauncherConfiguration::decodeBool<LauncherConfiguration, &LauncherConfiguration::disableTouchscreenEmulation>
    },
    {
        .key = "useCustomGameServer",
        .encode = &LauncherConfiguration::encodeBool<LauncherConfiguration, &LauncherConfiguration::useCustomGameServer>,
        .decode = &LauncherConfiguration::decodeBool<LauncherConfiguration, &LauncherConfiguration::useCustomGameServer>
    },
    {
        .key = "customGameServer",
        .encode = &LauncherConfiguration::encodeFixedString<LauncherConfiguration, 256, &LauncherConfiguration::customGameServer>,
        .decode = &LauncherConfiguration::decodeFixedString<LauncherConfiguration, 256, &LauncherConfiguration::customGameServer>
    },
    {
        .key = "resolution",
        .encode = &LauncherConfiguration::encodeString<LauncherConfiguration, &LauncherConfiguration::resolution>,
        .decode = &LauncherConfiguration::decodeString<LauncherConfiguration, &LauncherConfiguration::resolution>
    },
    {
        .key = "unityFramePacing",
        .encode = &LauncherConfiguration::encodeBool<LauncherConfiguration, &LauncherConfiguration::unityFramePacing>,
        .decode = &LauncherConfiguration::decodeBool<LauncherConfiguration, &LauncherConfiguration::unityFramePacing>
    },
};

LauncherConfiguration::LauncherConfiguration() : ConfigFileParser(m_options, sizeof(m_options) / sizeof(m_options[0])) {

}

LauncherConfiguration::~LauncherConfiguration() = default;

LauncherConfiguration::LauncherConfiguration(const LauncherConfiguration &other) = default;

LauncherConfiguration &LauncherConfiguration::operator =(const LauncherConfiguration &other) = default;

LauncherConfiguration::LauncherConfiguration(LauncherConfiguration &&other) noexcept = default;

LauncherConfiguration &LauncherConfiguration::operator =(LauncherConfiguration &&other) noexcept = default;

std::vector<std::string> LauncherConfiguration::buildCommandLine() const {
    std::vector<std::string> commandLine;

    if(customizeResolution) {
        commandLine.emplace_back("-disable-downscale");

        if(displayMode == DisplayMode::Window) {
            commandLine.emplace_back("-screen-fullscreen");
            commandLine.emplace_back("0");
        } else {
            commandLine.emplace_back("-screen-fullscreen");
            commandLine.emplace_back("1");

            if(displayMode == DisplayMode::Borderless) {
                commandLine.emplace_back("-window-mode");
                commandLine.emplace_back("borderless");
            } else {
                commandLine.emplace_back("-window-mode");
                commandLine.emplace_back("exclusive");
            }
        }

        auto delim = resolution.find('x');
        if(delim != std::string::npos) {
            commandLine.emplace_back("-screen-width");
            commandLine.emplace_back(resolution.substr(0, delim));
            commandLine.emplace_back("-screen-height");
            commandLine.emplace_back(resolution.substr(delim + 1));
        }
    }

    if(disableTouchscreenEmulation)
        commandLine.emplace_back("-disable-touch-emulation");

    if(useCustomGameServer) {
        commandLine.emplace_back("-gameserver");
        commandLine.emplace_back(customGameServer.data());
    }

    if(unityFramePacing) {
        commandLine.emplace_back("-unity-frame-pacing");
    }

    return commandLine;
}

std::string LauncherConfiguration::encodeDisplayMode() const {
    return std::to_string(static_cast<int>(displayMode));
}

void LauncherConfiguration::decodeDisplayMode(const std::string_view &value) {
    displayMode = static_cast<DisplayMode>(parseNumber<std::underlying_type<DisplayMode>::type>(value));
}
