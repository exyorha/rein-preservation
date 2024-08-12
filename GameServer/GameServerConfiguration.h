#ifndef GAME_SERVER_CONFIGURATION_H
#define GAME_SERVER_CONFIGURATION_H

#include <ConfigFileParser.h>

struct GameServerConfiguration : public ConfigFileParser {
public:
    GameServerConfiguration();
    ~GameServerConfiguration();

    GameServerConfiguration(const GameServerConfiguration &other);
    GameServerConfiguration &operator =(const GameServerConfiguration &other);

    GameServerConfiguration(GameServerConfiguration &&other) noexcept;
    GameServerConfiguration &operator =(GameServerConfiguration &&other) noexcept;

    float incomingUserExperienceMul = 1.0f;
    float incomingCostumeExperienceMul = 1.0f;
    float incomingCharacterExperienceMul = 1.0f;
    float incomingConsumableItemCountMul = 1.0f;
    float incomingGoldCountMul = 1.0f;
    float incomingMaterialCountMul = 1.0f;
    float incomingGemMul = 1.0f;

    int32_t scaleUserExperience(int32_t val) const;
    int32_t scaleCostumeExperience(int32_t val) const;
    int32_t scaleCharacterExperience(int32_t val) const;
    int32_t scaleConsumableItemCount(int32_t val) const;
    int32_t scaleGoldCount(int32_t val) const;
    int32_t scaleMaterialCount(int32_t val) const;
    int32_t scaleGemCount(int32_t val) const;

private:
    static const Option m_options[];
};

#endif
