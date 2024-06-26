#include "LauncherConfiguration.h"

#include <fstream>
#include <cstring>
#include <charconv>

const LauncherConfiguration::Option LauncherConfiguration::m_options[]{
    {
        .key = "customizeResolution",
        .encode = &LauncherConfiguration::encodeBool<&LauncherConfiguration::customizeResolution>,
        .decode = &LauncherConfiguration::decodeBool<&LauncherConfiguration::customizeResolution>
    },
    {
        .key = "displayMode",
        .encode = &LauncherConfiguration::encodeDisplayMode,
        .decode = &LauncherConfiguration::decodeDisplayMode,
    },
    {
        .key = "disableTouchscreenEmulation",
        .encode = &LauncherConfiguration::encodeBool<&LauncherConfiguration::disableTouchscreenEmulation>,
        .decode = &LauncherConfiguration::decodeBool<&LauncherConfiguration::disableTouchscreenEmulation>
    },
    {
        .key = "useCustomGameServer",
        .encode = &LauncherConfiguration::encodeBool<&LauncherConfiguration::useCustomGameServer>,
        .decode = &LauncherConfiguration::decodeBool<&LauncherConfiguration::useCustomGameServer>
    },
    {
        .key = "customGameServer",
        .encode = &LauncherConfiguration::encodeFixedString<256, &LauncherConfiguration::customGameServer>,
        .decode = &LauncherConfiguration::decodeFixedString<256, &LauncherConfiguration::customGameServer>
    },
    {
        .key = "useOpenGLES",
        .encode = &LauncherConfiguration::encodeBool<&LauncherConfiguration::useOpenGLES>,
        .decode = &LauncherConfiguration::decodeBool<&LauncherConfiguration::useOpenGLES>
    },
    {
        .key = "resolution",
        .encode = &LauncherConfiguration::encodeString<&LauncherConfiguration::resolution>,
        .decode = &LauncherConfiguration::decodeString<&LauncherConfiguration::resolution>
    },
    {
        .key = "unityFramePacing",
        .encode = &LauncherConfiguration::encodeBool<&LauncherConfiguration::unityFramePacing>,
        .decode = &LauncherConfiguration::decodeBool<&LauncherConfiguration::unityFramePacing>
    },
};


void LauncherConfiguration::load(const std::filesystem::path &configPath) {
    std::ifstream stream;
    stream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
    stream.open(configPath, std::ios::in | std::ios::binary);

    stream.exceptions(std::ios::badbit);

    std::string line;

    while(std::getline(stream, line)) {
        auto delim = line.find('=');
        if(delim == std::string::npos)
            continue;

        auto lineKey = std::string_view(line).substr(0, delim);
        auto lineValue = std::string_view(line).substr(delim + 1);

        for(const auto &option: m_options) {
            if(lineKey == option.key) {

                (this->*option.decode)(lineValue);

                break;
            }
        }

    }
}

void LauncherConfiguration::store(const std::filesystem::path &configPath) const {
    std::ofstream stream;
    stream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
    stream.open(configPath, std::ios::out | std::ios::trunc | std::ios::binary);

    for(const auto &option: m_options) {
        stream << option.key;
        stream << "=";

        stream << (this->*option.encode)();

        stream << "\n";
    }
}

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

    if(useOpenGLES) {
        commandLine.emplace_back("-force-gles");
    }

    if(unityFramePacing) {
        commandLine.emplace_back("-unity-frame-pacing");
    }

    return commandLine;
}

template<bool LauncherConfiguration::*field>
std::string LauncherConfiguration::encodeBool() const {
    return std::to_string(static_cast<int>(this->*field));
}

template<bool LauncherConfiguration::*field>
void LauncherConfiguration::decodeBool(const std::string_view &value) {
    this->*field = parseInt(value) != 0;
}

template<std::string LauncherConfiguration::*field>
std::string LauncherConfiguration::encodeString() const {
    return this->*field;
}

template<std::string LauncherConfiguration::*field>
void LauncherConfiguration::decodeString(const std::string_view &value) {
    this->*field = value;
}

template<size_t Length, std::array<char, Length> LauncherConfiguration::*field>
std::string LauncherConfiguration::encodeFixedString() const {
    const auto &val = this->*field;

    return std::string(val.data(), strnlen(val.data(), val.size()));
}

template<size_t Length, std::array<char, Length> LauncherConfiguration::*field>
void LauncherConfiguration::decodeFixedString(const std::string_view &value) {
    auto &val = this->*field;

    auto size = std::min<size_t>(val.size() - 1, value.size());
    memcpy(val.data(), value.data(), size);
    val[size] = 0;
}

std::string LauncherConfiguration::encodeDisplayMode() const {
    return std::to_string(static_cast<int>(displayMode));
}

void LauncherConfiguration::decodeDisplayMode(const std::string_view &value) {
    displayMode = static_cast<DisplayMode>(parseInt(value));
}

int LauncherConfiguration::parseInt(const std::string_view &string) {
    int value;

    auto result = std::from_chars(string.data(), string.data() + string.size(), value);
    if(result.ec != std::errc() || result.ptr != string.data() + string.size())
        throw std::runtime_error("failed to parse an integer value");

    return value;
}
