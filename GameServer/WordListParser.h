#ifndef WORD_LIST_PARSER_H
#define WORD_LIST_PARSER_H

#include <string>
#include <string_view>
#include <optional>
#include <array>
#include <vector>
#include <charconv>

class ObjectId;

class WordListParser {
public:
    explicit WordListParser(const std::string_view &string);
    ~WordListParser();

    WordListParser(const WordListParser &other) = delete;
    WordListParser &operator =(const WordListParser &other) = delete;

    /*
     * Optionals.
     */
    template<typename T>
    inline void parse(std::optional<T> &value) {
        parse(value.emplace());
    }

    /*
     * Fixed-size arrays
     */
    template<typename T, size_t Size>
    void parse(std::array<T, Size> &value) {
        for(auto &entry: value) {
            parse(entry);
        }
    }

    /*
     * Dynamically-sized arrays.
     */
    template<typename T>
    void parse(std::vector<T> &value) {
        for(auto &entry: value) {
            parse(entry);
        }
    }

    /*
     * Integral types.
     */
    template<typename T>
    auto parse(T &value) ->
        typename std::enable_if<std::is_integral<T>::value>::type {

        auto word = getWord();

        int base;
        getBase(word, base);

        auto result = std::from_chars(word.data(), word.data() + word.size(), value, base);
        checkConversion(word, result);
    }

    /*
     * Floats.
     */
    template<typename T>
    auto parse(T &value) ->
        typename std::enable_if<std::is_floating_point<T>::value>::type {

        auto word = getWord();

        auto result = std::from_chars(word.data(), word.data() + word.size(), value);
        checkConversion(word, result);
    }

    /*
     * Various other types.
     */
    void parse(bool &value);
    void parse(ObjectId &value);

    size_t numberOfParseableElements();

    inline bool isAtEnd() const {
        return m_rest.empty();
    }

    void skipSpaces();

    std::string getStringWord();
    std::string_view getRestOfTheString();

private:
    std::string_view getWord();

    static void getBase(std::string_view &word, int &base);
    static void checkConversion(const std::string_view &word, const std::from_chars_result &result);

    std::string_view m_rest;
};

template<typename T, typename Dummy = void>
struct WordListParserInvoker {
    inline bool operator()(WordListParser &parser, T &value) const {
        (void)parser;
        (void)value;

        return false;
    }
};

template<typename T>
struct WorldListParserInvokerUtil : std::true_type {};

template<typename T>
struct WordListParserInvoker<T, typename std::enable_if<
    WorldListParserInvokerUtil<decltype(std::declval<WordListParser *>()->parse(std::declval<T &>()))>::value>::type> {

    inline bool operator()(WordListParser &parser, T &value) const {
        parser.parse(value);
        return true;
    }
};

#endif
