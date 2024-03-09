#ifndef DATABASE_VIEWER_XML_FILE_H
#define DATABASE_VIEWER_XML_FILE_H

#include <string>
#include <optional>
#include <vector>

class XMLFile;
class XMLNode;

class XMLAttribute {
public:
    XMLAttribute();
    ~XMLAttribute();

    XMLAttribute(const std::string &name, const std::string &value);
    XMLAttribute(std::string &&name, const std::string &value);
    XMLAttribute(const std::string &name, std::string &&value);
    XMLAttribute(std::string &&name, std::string &&value);

    XMLAttribute(const XMLAttribute &other);
    XMLAttribute &operator =(const XMLAttribute &other);

    XMLAttribute(XMLAttribute &&other) noexcept;
    XMLAttribute &operator =(XMLAttribute &&other) noexcept;

    inline const std::string &name() const {
        return m_name.value();
    }

    inline std::string &name()  {
        return m_name.value();
    }

    inline void removeName() {
        m_name.reset();
    }

    inline void setName(const std::string &name) {
        m_name.emplace(name);
    }

    inline void setName(std::string &&name) {
        m_name.emplace(std::move(name));
    }

    inline bool hasName() const {
        return m_name.has_value();
    }

    inline const std::string &value() const {
        return m_value.value();
    }

    inline std::string &value() {
        return m_value.value();
    }

    inline void removeValue() {
        m_value.reset();
    }

    inline void setValue(const std::string &name) {
        m_value.emplace(name);
    }

    inline void setValue(std::string &&name) {
        m_value.emplace(std::move(name));
    }

    inline bool hasValue() const {
        return m_value.has_value();
    }

private:
    friend class XMLFile;
    std::optional<std::string> m_name;
    std::optional<std::string> m_value;
};

class XMLNode final {
public:
    XMLNode();
    ~XMLNode();

    explicit XMLNode(const std::string &name);
    explicit XMLNode(std::string &&name);

    XMLNode(const std::string &name, const std::string &content);
    XMLNode(std::string &&name, const std::string &content);
    XMLNode(const std::string &name, std::string &&content);
    XMLNode(std::string &&name, std::string &&content);

    XMLNode(const XMLNode &other);
    XMLNode &operator =(const XMLNode &other);

    XMLNode(XMLNode &&other) noexcept;
    XMLNode &operator =(XMLNode &&other) noexcept;

    inline const std::string &name() const {
        return m_name.value();
    }

    inline std::string &name()  {
        return m_name.value();
    }

    inline void removeName() {
        m_name.reset();
    }

    inline void setName(const std::string &name) {
        m_name = name;
    }

    inline void setName(std::string &&name) {
        m_name = std::move(name);
    }

    inline bool hasName() const {
        return m_name.has_value();
    }

    inline const std::string &content() const {
        return m_content.value();
    }

    inline std::string &content() {
        return m_content.value();
    }

    inline void removeContent() {
        m_content.reset();
    }

    inline void setContent(const std::string &content) {
        m_content = content;
    }

    inline void setContent(std::string &&content) {
        m_content = std::move(content);
    }

    inline bool hasContent() const {
        return m_content.has_value();
    }

    inline const std::vector<XMLAttribute> &attributes() const {
        return m_attributes;
    }

    inline std::vector<XMLAttribute> &attributes() {
        return m_attributes;
    }

    inline const std::vector<XMLNode> &children() const {
        return m_children;
    }

    inline std::vector<XMLNode> &children() {
        return m_children;
    }

    void trimWhitespace();

private:
    static void trimString(std::string &string);
    static bool isXMLWhitespace(char ch);

    friend class XMLFile;
    std::optional<std::string> m_name;
    std::optional<std::string> m_content;
    std::vector<XMLAttribute> m_attributes;
    std::vector<XMLNode> m_children;
};

class XMLFile {
public:
    XMLFile();
    ~XMLFile();

    XMLFile(const XMLFile &other);
    XMLFile &operator =(const XMLFile &other);

    XMLFile(XMLFile &&other) noexcept;
    XMLFile &operator =(XMLFile &&other) noexcept;

    void writeXML(std::string &stream) const;

    inline bool hasRoot() const {
        return m_root.has_value();
    }

    inline void setRoot(XMLNode &&root) {
        m_root = std::move(root);
    }

    inline const XMLNode &root() const {
        return m_root.value();
    }

    inline XMLNode &root() {
        return m_root.value();
    }

    void trimWhitespace();

private:
    std::optional<XMLNode> m_root;
};


#endif
