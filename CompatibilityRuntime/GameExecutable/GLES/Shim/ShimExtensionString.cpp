#include <GLES/Shim/ShimExtensionString.h>
#include <GLES/Shim/ShimNextContextSymbols.h>

#include <cstdio>
#include <algorithm>
#include <cstring>

ShimExtensionString::ShimExtensionString(const ShimNextContextSymbols &next) {

    GLint numberOfExtensions = 0;

    next.glGetIntegerv(GL_NUM_EXTENSIONS, &numberOfExtensions);

    m_extensions.reserve(numberOfExtensions);

    for(GLint extension = 0; extension < numberOfExtensions; extension++) {
        auto extensionName = reinterpret_cast<const char *>(next.glGetStringi(GL_EXTENSIONS, extension));

        m_extensions.emplace_back(extensionName);
    }
}

ShimExtensionString::~ShimExtensionString() = default;

bool ShimExtensionString::hasExtension(const char *extension) const {
    auto existing = std::find_if(m_extensions.begin(), m_extensions.end(), [=](const char *other) -> bool { return strcmp(other, extension) == 0; });

    return existing != m_extensions.end();
}

void ShimExtensionString::addExtension(const char *extension) {
    auto existing = std::find_if(m_extensions.begin(), m_extensions.end(), [=](const char *other) -> bool { return strcmp(other, extension) == 0; });
    if(existing == m_extensions.end())
        m_extensions.emplace_back(extension);
}

void ShimExtensionString::removeExtension(const char *extension) {
    auto existing = std::find_if(m_extensions.begin(), m_extensions.end(), [=](const char *other) -> bool { return strcmp(other, extension) == 0; });
    if(existing != m_extensions.end())
        m_extensions.erase(existing);
}

const unsigned char *ShimExtensionString::extension(unsigned int index) const {
    if(index >= m_extensions.size())
        return nullptr;

    return reinterpret_cast<const unsigned char *>(m_extensions[index]);
}

