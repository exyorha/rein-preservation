#include "GameServerConfiguration.h"
#include "ConfigFileParser.h"

#include <cmath>

const ConfigFileParser::Option GameServerConfiguration::m_options[] {
    {
        "incomingUserExperienceExp",
        &GameServerConfiguration::encodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingUserExperienceExp>,
        &GameServerConfiguration::decodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingUserExperienceExp>,
    },
    {
        "incomingCostumeExperienceExp",
        &GameServerConfiguration::encodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingCostumeExperienceExp>,
        &GameServerConfiguration::decodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingCostumeExperienceExp>,
    },
    {
        "incomingCharacterExperienceExp",
        &GameServerConfiguration::encodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingCharacterExperienceExp>,
        &GameServerConfiguration::decodeNumber<GameServerConfiguration, float, &GameServerConfiguration::incomingCharacterExperienceExp>,
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
    return ceilf(powf(val, incomingUserExperienceExp));
}

int32_t GameServerConfiguration::scaleCostumeExperience(int32_t val) const {
    return ceilf(powf(val, incomingCostumeExperienceExp));
}

int32_t GameServerConfiguration::scaleCharacterExperience(int32_t val) const {
    return ceilf(powf(val, incomingCharacterExperienceExp));
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
