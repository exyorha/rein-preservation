#ifndef DATABASE_VIEWER_TEXT_XML_WRITER_H
#define DATABASE_VIEWER_TEXT_XML_WRITER_H

#include <vector>
#include <string_view>
#include <string>

class XMLFile;
class XMLNode;

class TextXMLWriter {
public:
    TextXMLWriter(std::string &output);
    ~TextXMLWriter();

    TextXMLWriter(const TextXMLWriter &other) = delete;
    TextXMLWriter &operator =(const TextXMLWriter &other) = delete;

    void write(const XMLFile &file);

private:
    void write(const XMLNode &node);

    void writeCharacterContent(const std::string_view &string, bool attribute = false);
    void writeText(const std::string_view &string);
    void writeIndent();

    std::string &m_output;
    int m_indentLevel;

};

#endif
