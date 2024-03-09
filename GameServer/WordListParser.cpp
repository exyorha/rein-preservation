#include "WordListParser.h"

#include <stdexcept>

WordListParser::WordListParser(const std::string_view &string) : m_rest(string) {

}

WordListParser::~WordListParser() = default;

void WordListParser::getBase(std::string_view &word, int &base) {
    base = 10;

    if(word.starts_with("0x")) {
        word = word.substr(2);
        base = 16;
    }
}

void WordListParser::checkConversion(const std::string_view &word, const std::from_chars_result &result) {
    if(result.ec != std::errc() || word.empty() || result.ptr != word.data() + word.size())
        throw std::logic_error("failed to parse");
}

void WordListParser::parse(bool &value) {
    int val;
    parse(val);
    value = val != 0;
}

void WordListParser::skipSpaces() {
    while(!m_rest.empty() && m_rest.starts_with(' ')) {
        m_rest = m_rest.substr(1);
    }
}

std::string_view WordListParser::getWord() {
    skipSpaces();

    auto end = m_rest.find_first_of(' ');
    auto word = m_rest.substr(0, end);

    if(end == std::string_view::npos) {
        m_rest = std::string_view();
    } else {
        m_rest = m_rest.substr(end + 1);
        skipSpaces();
    }

    return word;
}

std::string_view WordListParser::getRestOfTheString() {
    std::string_view word = m_rest;

    m_rest = std::string_view();

    return word;
}

/*
 * This replicates the rules used in the original game for storing strings in HAP files.
 */
std::string WordListParser::getStringWord() {
    std::string output;

    /*
     * First, remove all the spaces at the start.
     */
    skipSpaces();

    bool quoted;
    if(m_rest.starts_with('"')) {
        /*
         * This is a quoted string.
         */
        quoted = true;
        m_rest = m_rest.substr(1);
    } else {
        quoted = false;
    }

    while(!m_rest.empty()) {
        auto ch = m_rest[0];
        m_rest = m_rest.substr(1);

        /*
         * Quoted strings terminate when we hit a '"', and unquoted
         * strings terminate when we hit a space.
         */
        if((quoted && ch == '"') || (!quoted && ch == ' '))
            break;

        /*
         * If we have a backslash, the escape character do the escape
         * processing.
         *
         * As an addition - it's not a feature that's included in the
         * original engine, but seems to be used because of something
         * that seems to be an encoding error in the original data -
         * we implement 'Yen sign escaping'.
         *
         * The escape character used is U+FFE5 FULLWIDTH YEN SIGN, or,
         * in the original SHIFT-JIS, '81 8F', not the U+00A5 YEN SIGN
         * that's encoded as '5C' in SHIFT-JIS, so it does *not*
         * coincede with the ASCII backslash at 0x5C.
         */

        // This is U+FFE5 FULLWIDTH YEN SIGN as encoded in UTF-8.
        static const char escapeSequence[] = {
            static_cast<char>(0xEF),
            static_cast<char>(0xBF),
            static_cast<char>(0xA5)
        };

        bool escaped;

        if(ch == '\\') {
            escaped = true;
        } else if(m_rest.size() >= 2 &&
            ch == escapeSequence[0] &&
            m_rest[1] == escapeSequence[1] &&
            m_rest[2] == escapeSequence[2]) {

            escaped = true;
            m_rest = m_rest.substr(2);
        } else {
            escaped = false;
        }

        if(escaped) {

            /*
             * It's not an error condition if the string ends in an escape
             * sequence.
             */

            if(m_rest.empty())
                break;

            ch = m_rest[0];
            m_rest = m_rest.substr(1);

            /*
             * A few characters are handled specially, the rest are copied
             * verbatim.
             */

            switch(ch) {
            case 'b':
                ch = '\b';
                break;

            case 'f':
                ch = '\f';
                break;

            case 'n':
                ch = '\n';
                break;

            case 'r':
                ch = '\r';
                break;

            case 't':
                ch = '\t';
                break;


            default:
                break;
            }

            output.push_back(ch);
        } else {
            /*
                * Non-escaped characters are copied verbatim.
                */

            output.push_back(ch);
        }
    }

    /*
     * Last, remove all the spaces at the end - this is necessary because
     * of the different parsing rules we have for the lists, though
     * there's no functional difference for the parsing itself.
     */
    skipSpaces();

    return output;
}

size_t WordListParser::numberOfParseableElements() {
    auto savedRest = m_rest;

    size_t numberOfWords = 0;
    while(!m_rest.empty()) {
        numberOfWords++;

        getWord();
    }

    m_rest = savedRest;

    return numberOfWords;
}
