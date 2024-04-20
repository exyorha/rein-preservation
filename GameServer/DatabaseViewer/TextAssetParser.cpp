#include "DatabaseViewer/TextAssetParser.h"
#include "DatabaseViewer/DatabaseViewerService.h"

#include "LLServices/Logging/LogFacility.h"

#include <ClientDataAccess/OctoContentStorage.h>
#include <ClientDataAccess/ZIPBasedStorage.h>

#include <UnityAsset/Streams/FileInputOutput.h>
#include <UnityAsset/Streams/StreamBackingBuffer.h>
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h>
#include <UnityAsset/SerializedAsset/SerializedAssetFile.h>
#include <UnityAsset/UnityTypes.h>

TextAssetParser::TextAssetParser() = default;

TextAssetParser::~TextAssetParser() = default;

void TextAssetParser::parseAllTextAssets(const ClientDataAccess::OctoContentStorage &contentStorage) {

    for(const auto &bundle: contentStorage.database().assetbundlelist()) {
        if(bundle.name().starts_with("text)")) {
            auto delimiter = bundle.name().find(')', 5);
            std::string_view language;
            if(delimiter == std::string_view::npos) {
                language = std::string_view(bundle.name()).substr(5);
            } else {
                language = std::string_view(bundle.name()).substr(5, delimiter - 5);
            }

            auto path = contentStorage.locateFile("A" + bundle.objectname());
            if(!path.has_value()) {
                LogDatabaseViewerService.error("unable to find the asset file for text file %s", bundle.name().c_str());
                continue;
            }

            std::shared_ptr<UnityAsset::StreamBackingBuffer> buffer;

            auto zipLocation = ClientDataAccess::ZIPBasedStorage::unwrapZIPPath(std::move(*path));
            if(zipLocation.has_value()) {
                buffer = UnityAsset::readFile(zipLocation->zipFilePath, zipLocation->fileOffset, zipLocation->fileLength);
            } else {
                buffer = UnityAsset::readFile(*path);
            }

            UnityAsset::AssetBundleFile bundle((UnityAsset::Stream(buffer)));
            for(const auto &file: bundle.entries) {
                if(file.filename().ends_with(".resource") || file.filename().ends_with(".resS"))
                    continue;

                UnityAsset::SerializedAssetFile asset(UnityAsset::Stream(file.data()));
                for(const auto &object: asset.m_Objects) {
                    const auto &type = asset.m_Types.at(object.typeIndex);
                    if(type.m_ScriptTypeIndex < 0 && !type.m_ScriptID.has_value() && type.classID == UnityAsset::UnityTypes::TextAssetClassID) {

                        auto textAsset = UnityAsset::UnityTypes::deserializeTextAsset(object.objectData);

                        if(textAsset.m_Name == "credit") {
                            /*
                             * Uses a different format.
                             */
                            continue;
                        }

                        parseTextAsset(language, textAsset.m_Script);
                    }
                }
            }
        }
    }
}


void TextAssetParser::parseTextAsset(const std::string_view &language, std::string_view asset) {
    while(!asset.empty()) {
        std::string_view line;

        auto eol = asset.find('\n');
        if(eol == std::string_view::npos) {
            line = asset;
            asset = std::string_view();
        } else {
            line = asset.substr(0, eol);
            asset = asset.substr(eol + 1);
        }

        while(line.starts_with('\r'))
            line = line.substr(1);

        while(line.ends_with('\r'))
            line = line.substr(0, line.size() - 1);

        if(line.empty() || line.starts_with("//"))
            continue;

        auto delimiter = line.find(':');
        if(delimiter == std::string_view::npos) {
            LogDatabaseViewerService.warning("no delimiter in a string asset line: %.*s", static_cast<int>(line.size()), line.data());
            continue;
        }

        auto key = line.substr(0, delimiter);
        auto value = line.substr(delimiter + 1);

        std::string expandedValue;
        expandedValue.reserve(value.size());

        for(size_t index = 0, size = value.size(); index < size; index++) {
            auto ch = value[index];
            if(ch == '\\') {
                ++index;
                if(index == value.size())
                    break;

                auto escaped = value[index];

                if(escaped == 'n') {
                    ch = '\n';
                } else {
                    ch = escaped;
                }
            }

            expandedValue.push_back(ch);
        }

        addTextEntry(language, key, std::move(expandedValue));
    }
}
