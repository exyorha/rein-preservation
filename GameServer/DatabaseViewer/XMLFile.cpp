#include "DatabaseViewer/XMLFile.h"
#include "DatabaseViewer/TextXMLWriter.h"

#include <algorithm>

XMLAttribute::XMLAttribute() = default;

XMLAttribute::~XMLAttribute() = default;

XMLAttribute::XMLAttribute(const std::string &name, const std::string &value) : m_name(name), m_value(value) {

}

XMLAttribute::XMLAttribute(std::string &&name, const std::string &value) : m_name(std::move(name)), m_value(value) {

}

XMLAttribute::XMLAttribute(const std::string &name, std::string &&value) : m_name(name), m_value(std::move(value)) {

}

XMLAttribute::XMLAttribute(std::string &&name, std::string &&value) : m_name(std::move(name)), m_value(std::move(value)) {

}

XMLAttribute::XMLAttribute(const XMLAttribute &other) = default;

XMLAttribute &XMLAttribute::operator =(const XMLAttribute &other) = default;

XMLAttribute::XMLAttribute(XMLAttribute &&other) noexcept = default;

XMLAttribute &XMLAttribute::operator =(XMLAttribute &&other) noexcept = default;

XMLNode::XMLNode() = default;

XMLNode::~XMLNode() = default;

XMLNode::XMLNode(const std::string &name) : m_name(name) {

}

XMLNode::XMLNode(std::string &&name) : m_name(std::move(name)) {

}

XMLNode::XMLNode(const std::string &name, const std::string &content) : m_name(name) {

}

XMLNode::XMLNode(std::string &&name, const std::string &content) : m_name(std::move(name)), m_content(content) {

}

XMLNode::XMLNode(const std::string &name, std::string &&content) : m_name(name), m_content(std::move(content)) {

}

XMLNode::XMLNode(std::string &&name, std::string &&content) : m_name(std::move(name)), m_content(std::move(content)) {

}

XMLNode::XMLNode(const XMLNode &other) = default;

XMLNode &XMLNode::operator =(const XMLNode &other) = default;

XMLNode::XMLNode(XMLNode &&other) noexcept = default;

XMLNode &XMLNode::operator =(XMLNode &&other) noexcept = default;

XMLFile::XMLFile() = default;

XMLFile::~XMLFile() = default;

XMLFile::XMLFile(const XMLFile &other) = default;

XMLFile &XMLFile::operator =(const XMLFile &other) = default;

XMLFile::XMLFile(XMLFile &&other) noexcept = default;

XMLFile &XMLFile::operator =(XMLFile &&other) noexcept = default;

void XMLFile::writeXML(std::string &stream) const {
    TextXMLWriter writer(stream);

    writer.write(*this);
}

void XMLFile::trimWhitespace() {
    if(m_root) {
        m_root->trimWhitespace();
    }
}

void XMLNode::trimWhitespace() {
    if(m_content) {
        trimString(*m_content);
        if(m_content->empty())
            removeContent();
    }

    for(auto it = m_children.begin(); it != m_children.end(); ) {
        auto &child = *it;

        child.trimWhitespace();

        if(!child.hasName() && !child.hasContent() && child.children().empty())
            it = m_children.erase(it);
        else
            ++it;
    }
}

bool XMLNode::isXMLWhitespace(char ch) {
    return ch == 0x09 || ch == 0x0A || ch == 0x0D || ch == 0x20;
}

void XMLNode::trimString(std::string &string) {
    /*
        * XML considers the following characters whitespace:
        *  U+0009, U+000A, U+000D, U+0020.
        * These are all ASCII, so we can trim them from both ends without
        * concerning ourselves with UTF-8 encoding.
        */

    auto trimmedBegin = string.begin();

    while(trimmedBegin != string.end()) {
        if(!isXMLWhitespace(*trimmedBegin))
            break;
        else
            trimmedBegin++;
    }

    auto trimmedEnd = string.end();

    while(trimmedEnd != trimmedBegin) {
        auto newEnd = trimmedEnd;
        --newEnd;

        if(!isXMLWhitespace(*newEnd))
            break;
        else
            trimmedEnd = newEnd;
    }

    if(string.begin() != trimmedBegin || string.end() != trimmedEnd)
        string = std::string(trimmedBegin, trimmedEnd);
}
