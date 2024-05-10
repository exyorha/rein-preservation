#ifndef RELOADABLE_RESOURCE_H
#define RELOADABLE_RESOURCE_H

#include <filesystem>
#include <optional>

namespace LLServices {
    class LogFacility;
}

class ReloadableResource {
protected:
    explicit ReloadableResource(std::filesystem::path &&filename);
    ~ReloadableResource();

public:
    ReloadableResource(const ReloadableResource &other) = delete;
    ReloadableResource &operator =(const ReloadableResource &other) = delete;

    void reloadIfNeeded();

protected:
    virtual void doReload(std::string &&content) = 0;

private:
    std::filesystem::path m_filename;
    std::optional<std::filesystem::file_time_type> m_modtimeOfLoadedVersion;
};

extern LLServices::LogFacility LogReloadableResource;

#endif
