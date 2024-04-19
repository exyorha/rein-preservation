#ifndef DATABASE_VIEWER_HIERARCHICAL_PATH_PARSER_H
#define DATABASE_VIEWER_HIERARCHICAL_PATH_PARSER_H

#include <string_view>

class HierarchicalPathParser {
public:
    explicit HierarchicalPathParser(const std::string_view &path);
    ~HierarchicalPathParser();

    HierarchicalPathParser(const HierarchicalPathParser &other);
    HierarchicalPathParser &operator =(const HierarchicalPathParser &other);

    inline bool atEnd() const {
        return m_path.empty();
    }

    std::string_view getElement();

private:
    std::string_view m_path;
};

#endif
