#ifndef DATABASE_IMAGE_H
#define DATABASE_IMAGE_H

#include <filesystem>
#include <vector>

namespace MasterDatabase {

    class DatabaseImage {
    public:
        explicit DatabaseImage(const std::filesystem::path &filename);
        ~DatabaseImage();

        DatabaseImage(const DatabaseImage &other) = delete;
        DatabaseImage &operator =(const DatabaseImage &other) = delete;

        inline const char *data() const {
            return m_data.data();
        }

        inline size_t size() const {
            return m_data.size();
        }

    private:
        std::vector<char> m_data;
    };

}

#endif

