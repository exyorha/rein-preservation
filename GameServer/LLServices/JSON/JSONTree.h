#ifndef LLSERVICES_JSON_JSON_TREE_H
#define LLSERVICES_JSON_JSON_TREE_H

#include <yajl_tree.h>
#include <string>

namespace LLServices {

    class JSONTree {

        inline JSONTree(yajl_val val) noexcept : m_tree(val) {

        }

    public:
        inline JSONTree() noexcept : JSONTree(nullptr) {

        }

        ~JSONTree();

        JSONTree(const JSONTree &other) = delete;
        JSONTree &operator =(const JSONTree &other) = delete;

        JSONTree(JSONTree &&other) noexcept;
        JSONTree &operator =(JSONTree &&other) noexcept;

        inline operator bool() const {
            return m_tree;
        }

        inline operator yajl_val() const {
            return m_tree;
        }

        static JSONTree parse(const char *string);

        static inline JSONTree parse(const std::string &string) {
            return parse(string.c_str());
        }

    private:
        yajl_val m_tree;
    };

}

#endif
