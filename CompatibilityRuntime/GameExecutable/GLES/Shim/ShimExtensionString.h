#ifndef GLES_SHIM_SHIM_EXTENSION_STRING_H
#define GLES_SHIM_SHIM_EXTENSION_STRING_H

#include <vector>
#include <string_view>

class ShimNextContextSymbols;

class ShimExtensionString {
public:
    ShimExtensionString(const ShimNextContextSymbols &next);
    ~ShimExtensionString();

    ShimExtensionString(const ShimExtensionString &other) = delete;
    ShimExtensionString &operator =(const ShimExtensionString &other) = delete;

    inline int numberOfExtensions() const {
        return m_extensions.size();
    }

    const unsigned char *extension(unsigned int index) const;

    bool hasExtension(const char *extension) const;

    void addExtension(const char *extension);
    void removeExtension(const char *extension);

private:
    std::vector<const char *> m_extensions;
};

#endif

