#include "ConfigFileParser.h"

#include <fstream>

ConfigFileParser::ConfigFileParser(const Option *options, size_t optionCount) : m_options(options), m_optionCount(optionCount) {

}

ConfigFileParser::~ConfigFileParser() = default;

ConfigFileParser::ConfigFileParser(const ConfigFileParser &other) = default;

ConfigFileParser &ConfigFileParser::operator =(const ConfigFileParser &other) = default;

ConfigFileParser::ConfigFileParser(ConfigFileParser &&other) noexcept = default;

ConfigFileParser &ConfigFileParser::operator =(ConfigFileParser &&other) noexcept = default;

std::string_view ConfigFileParser::trimSpaces(const std::string_view &view) {
    auto firstNonSpace = view.find_first_not_of(" \t");

    if(firstNonSpace == std::string_view::npos)
        return std::string_view();

    auto lastNonSpace = view.find_last_not_of(" \t");

    return view.substr(firstNonSpace, lastNonSpace + 1 - firstNonSpace);
}

void ConfigFileParser::load(const std::filesystem::path &configPath) {
    std::ifstream stream;
    stream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
    stream.open(configPath, std::ios::in | std::ios::binary);

    stream.exceptions(std::ios::badbit);

    std::string line;

    while(std::getline(stream, line)) {
        std::string_view lineView(line);

        auto comment = lineView.find('#');
        if(comment != std::string_view::npos) {
            lineView = lineView.substr(0, comment);
        }

        auto delim = lineView.find('=');
        if(delim == std::string::npos)
            continue;

        auto lineKey = trimSpaces(lineView.substr(0, delim));
        auto lineValue = trimSpaces(lineView.substr(delim + 1));

        for(size_t optionIndex = 0; optionIndex < m_optionCount; optionIndex++) {
            const auto &option = m_options[optionIndex];

            if(lineKey == option.key) {

                (this->*option.decode)(lineValue);

                break;
            }
        }

    }
}

void ConfigFileParser::store(const std::filesystem::path &configPath) const {
    std::ofstream stream;
    stream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
    stream.open(configPath, std::ios::out | std::ios::trunc | std::ios::binary);

    for(size_t optionIndex = 0; optionIndex < m_optionCount; optionIndex++) {
        const auto &option = m_options[optionIndex];

        stream << option.key;
        stream << "=";

        stream << (this->*option.encode)();

        stream << "\n";
    }
}
