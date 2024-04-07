#ifndef ASSET_REPROCESSING_H
#define ASSET_REPROCESSING_H

#include <optional>
#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset::UnityTypes {
    class Shader;
}

class AssetReprocessing {
public:
    static std::optional<UnityAsset::Stream> reprocessShader(const UnityAsset::Stream &original);
};

#endif
