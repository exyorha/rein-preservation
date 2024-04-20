#ifndef DATABASE_VIEWER_TEXT_ASSET_PARSER_H
#define DATABASE_VIEWER_TEXT_ASSET_PARSER_H

#include <string_view>

namespace ClientDataAccess {
    class OctoContentStorage;
}

class TextAssetParser {
protected:
    TextAssetParser();
    ~TextAssetParser();

    TextAssetParser(const TextAssetParser &other) = delete;
    TextAssetParser &operator =(const TextAssetParser &other) = delete;

public:
    void parseAllTextAssets(const ClientDataAccess::OctoContentStorage &contentStorage);

protected:
    virtual void addTextEntry(const std::string_view &language, const std::string_view &key, std::string &&value) = 0;

private:
    void parseTextAsset(const std::string_view &language, std::string_view asset);
};

#endif
