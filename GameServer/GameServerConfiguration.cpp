#include "GameServerConfiguration.h"
#include "ConfigFileParser.h"

#include <cmath>

const ConfigFileParser::Option GameServerConfiguration::m_options[] {
    {
        "incomingUserExperienceMul",
        &GameServerConfiguration::encodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingUserExperienceMul>,
        &GameServerConfiguration::decodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingUserExperienceMul>,
    },
    {
        "incomingCostumeExperienceMul",
        &GameServerConfiguration::encodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingCostumeExperienceMul>,
        &GameServerConfiguration::decodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingCostumeExperienceMul>,
    },
    {
        "incomingCharacterExperienceMul",
        &GameServerConfiguration::encodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingCharacterExperienceMul>,
        &GameServerConfiguration::decodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingCharacterExperienceMul>,
    },
    {
        "incomingConsumableItemCountMul",
        &GameServerConfiguration::encodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingConsumableItemCountMul>,
        &GameServerConfiguration::decodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingConsumableItemCountMul>,
    },
    {
        "incomingGoldCountMul",
        &GameServerConfiguration::encodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingGoldCountMul>,
        &GameServerConfiguration::decodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingGoldCountMul>,
    },
    {
        "incomingMaterialCountMul",
        &GameServerConfiguration::encodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingMaterialCountMul>,
        &GameServerConfiguration::decodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingMaterialCountMul>,
    },
    {
        "incomingGemMul",
        &GameServerConfiguration::encodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingGemMul>,
        &GameServerConfiguration::decodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingGemMul>,
    },
};

GameServerConfiguration::GameServerConfiguration() : ConfigFileParser(m_options, sizeof(m_options) / sizeof(m_options[0])) {

}

GameServerConfiguration::~GameServerConfiguration() = default;

GameServerConfiguration::GameServerConfiguration(const GameServerConfiguration &other) = default;

GameServerConfiguration &GameServerConfiguration::operator =(const GameServerConfiguration &other) = default;

GameServerConfiguration::GameServerConfiguration(GameServerConfiguration &&other) noexcept = default;

GameServerConfiguration &GameServerConfiguration::operator =(GameServerConfiguration &&other) noexcept = default;

int32_t GameServerConfiguration::scaleUserExperience(int32_t val) const {
    return roundf(val * incomingUserExperienceMul);
}

int32_t GameServerConfiguration::scaleCostumeExperience(int32_t val) const {
    return roundf(val * incomingCostumeExperienceMul);
}

int32_t GameServerConfiguration::scaleCharacterExperience(int32_t val) const {
    return roundf(val * incomingCharacterExperienceMul);
}

int32_t GameServerConfiguration::scaleConsumableItemCount(int32_t val) const {
    return roundf(val * incomingConsumableItemCountMul);
}

int32_t GameServerConfiguration::scaleGoldCount(int32_t val) const {
    return roundf(val * incomingGoldCountMul);
}

int32_t GameServerConfiguration::scaleMaterialCount(int32_t val) const {
    return roundf(val * incomingMaterialCountMul);
}

int32_t GameServerConfiguration::scaleGemCount(int32_t val) const {
    return roundf(val * incomingGemMul);
}
