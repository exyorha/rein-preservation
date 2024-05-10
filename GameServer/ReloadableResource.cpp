#include "ReloadableResource.h"

#include "LLServices/Logging/LogFacility.h"

#include <filesystem>
#include <fstream>

LLServices::LogFacility LogReloadableResource("ReloadableResource");

ReloadableResource::ReloadableResource(std::filesystem::path &&filename) : m_filename(std::move(filename)) {

}

ReloadableResource::~ReloadableResource() = default;

void ReloadableResource::reloadIfNeeded() {
    auto modified = std::filesystem::last_write_time(m_filename);
    if(!m_modtimeOfLoadedVersion.has_value() || *m_modtimeOfLoadedVersion != modified) {

        LogReloadableResource.info("reloading %s", m_filename.string().c_str());

        m_modtimeOfLoadedVersion.reset();

        std::string data;
        {
            std::ifstream stream;
            stream.exceptions(std::ios::badbit | std::ios::eofbit | std::ios::failbit);
            stream.open(m_filename, std::ios::in | std::ios::binary);

            stream.seekg(0, std::ios::end);
            auto inputSize = static_cast<size_t>(stream.tellg());

            data.resize(inputSize);

            stream.seekg(0);

            stream.read(data.data(), data.size());
        }

        doReload(std::move(data));

        m_modtimeOfLoadedVersion.emplace(modified);
    }
}

