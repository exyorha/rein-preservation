#include "HierarchicalPathParser.h"

#include <stdexcept>

HierarchicalPathParser::HierarchicalPathParser(const std::string_view &path) {
    if(!path.starts_with("/")) {
        throw std::runtime_error("the path doesn't start with a slash");
    }

    m_path = path.substr(1);
}

HierarchicalPathParser::~HierarchicalPathParser() = default;

std::string_view HierarchicalPathParser::getElement() {
    auto delimiter = m_path.find('/');
    if(delimiter == std::string_view::npos) {
        auto result = m_path;
        m_path = std::string_view();

        return result;
    } else {
        auto result = m_path.substr(0, delimiter);
        m_path = m_path.substr(delimiter + 1);

        return result;
    }
}
