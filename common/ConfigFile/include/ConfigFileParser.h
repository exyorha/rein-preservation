#ifndef CONFIG_FILE_CONFIG_FILE_PARSER_H
#define CONFIG_FILE_CONFIG_FILE_PARSER_H

#include <string>
#include <filesystem>
#include <charconv>
#include <cstring>

class ConfigFileParser {
protected:
    using OptionEncoder = std::string (ConfigFileParser::*)() const;
    using OptionDecoder = void (ConfigFileParser::*)(const std::string_view &value);

    struct Option {
        const char *key;
        OptionEncoder encode;
        OptionDecoder decode;
    };

    ConfigFileParser(const Option *options, size_t optionCount);
    ~ConfigFileParser();

    ConfigFileParser(const ConfigFileParser &other);
    ConfigFileParser &operator =(const ConfigFileParser &other);

    ConfigFileParser(ConfigFileParser &&other) noexcept;
    ConfigFileParser &operator =(ConfigFileParser &&other) noexcept;

public:
    void load(const std::filesystem::path &configPath);
    void store(const std::filesystem::path &configPath) const;

protected:
    template<typename Subclass, bool Subclass::*field>
    std::string encodeBool() const {
        return std::to_string(static_cast<int>(static_cast<const Subclass *>(this)->*field));
    }

    template<typename Subclass, bool Subclass::*field>
    void decodeBool(const std::string_view &value) {
        static_cast<Subclass *>(this)->*field = parseNumber<uint8_t>(value) != 0;
    }

    template<typename Subclass, std::string Subclass::*field>
    std::string encodeString() const {
        return static_cast<const Subclass *>(this)->*field;
    }

    template<typename Subclass, std::string Subclass::*field>
    void decodeString(const std::string_view &value) {
        static_cast<Subclass *>(this)->*field = value;
    }

    template<typename Subclass, size_t Length, std::array<char, Length> Subclass::*field>
    std::string encodeFixedString() const {
        const auto &val = static_cast<const Subclass *>(this)->*field;

        return std::string(val.data(), strnlen(val.data(), val.size()));
    }

    template<typename Subclass, size_t Length, std::array<char, Length> Subclass::*field>
    void decodeFixedString(const std::string_view &value) {
        auto &val = static_cast<Subclass *>(this)->*field;

        auto size = std::min<size_t>(val.size() - 1, value.size());
        memcpy(val.data(), value.data(), size);
        val[size] = 0;
    }

    template<typename T>
    static T parseNumber(const std::string_view &string) {
        T value;

        auto result = std::from_chars(string.data(), string.data() + string.size(), value);
        if(result.ec != std::errc() || result.ptr != string.data() + string.size())
            throw std::runtime_error("failed to parse an integer value");

        return value;
    }

    template<typename Subclass, typename NumberType, NumberType Subclass::*field>
    std::string encodeNumber() const {
        return std::to_string(static_cast<const Subclass *>(this)->*field);
    }

    template<typename Subclass, typename NumberType, NumberType Subclass::*field>
    void decodeNumber(const std::string_view &value) {
        static_cast<Subclass *>(this)->*field = parseNumber<NumberType>(value);
    }

private:
    static std::string_view trimSpaces(const std::string_view &view);

    const Option *m_options;
    size_t m_optionCount;
};

#endif

