#include <LLServices/JSON/JSONTree.h>

#include <stdexcept>

namespace LLServices {

    JSONTree::~JSONTree() {
        yajl_tree_free(m_tree);
    }


    JSONTree::JSONTree(JSONTree &&other) noexcept : JSONTree() {
        *this = std::move(other);
    }

    JSONTree &JSONTree::operator =(JSONTree &&other) noexcept {
        if(this != &other) {
            yajl_tree_free(m_tree);
            m_tree = other.m_tree;
            other.m_tree = nullptr;
        }

        return *this;
    }

    JSONTree JSONTree::parse(const char *string) {
        char message[128];

        auto result = yajl_tree_parse(string, message, sizeof(message));
        if(!result) {
            throw std::runtime_error(message);
        }

        return JSONTree(result);
    }
}
