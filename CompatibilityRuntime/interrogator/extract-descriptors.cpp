#include "il2cpp-api-types.h"
#include <il2cpp-api.h>
#include <il2cpp-tabledefs.h>
#include <translator_api.h>

#include <sys/mman.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <filesystem>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

const Il2CppImage *getImageByName(const char *assemblyToFind) {
    auto domain = il2cpp_domain_get();

    size_t numberOfAssemblies;
    auto assemblies = il2cpp_domain_get_assemblies(domain, &numberOfAssemblies);
    for(size_t index = 0; index <numberOfAssemblies; index++) {
        auto assembly = assemblies[index];
        auto image = il2cpp_assembly_get_image(assembly);
        auto assemblyName = il2cpp_image_get_name(image);

        if(strcmp(assemblyName, assemblyToFind) == 0) {
            return image;
        }
    }

    return nullptr;
}

Il2CppClass *getClassByFullName(std::string_view fullName) {
    auto imageDelim = fullName.find("::");
    if(imageDelim == std::string::npos) {
        throw std::runtime_error("no image delimiter in " + std::string(fullName));
    }

    auto image = getImageByName(std::string(fullName.substr(0, imageDelim)).c_str());
    if(!image) {
        return nullptr;
    }

    fullName = fullName.substr(imageDelim + 2);

    auto delim = fullName.find_last_of('.');
    std::string className, namespaceName;

    if(delim == std::string::npos) {
        className = fullName;
    } else {
        className = fullName.substr(delim + 1);
        namespaceName = fullName.substr(0, delim);
    }

    return il2cpp_class_from_name(image, namespaceName.c_str(), className.c_str());
}

void printException(Il2CppException *exception) {

    static char messageBuf[4096];
    il2cpp_format_exception(exception, messageBuf, sizeof(messageBuf));

    fprintf(stderr, "%s\n", messageBuf);

    il2cpp_format_stack_trace(exception, messageBuf, sizeof(messageBuf));

    fprintf(stderr, "%s\n", messageBuf);

    auto exceptionClass = il2cpp_object_get_class(reinterpret_cast<Il2CppObject *>(exception));
    auto prop = il2cpp_class_get_property_from_name(exceptionClass, "InnerException");
    auto propGet = il2cpp_property_get_get_method(const_cast<PropertyInfo *>(prop));
    Il2CppException *exception2 = nullptr;
    auto inner = il2cpp_runtime_invoke(propGet, exception, nullptr, &exception2);

    if(inner && !exception2) {
        printf("Caused by:\n");

        printException(reinterpret_cast<Il2CppException *>(inner));
    }
}

int tool_main(void) {
    /*
     * Patch Google.Protobuf.dll::Google.Protobuf.Reflection.FileDescriptor::FromGeneratedCode into just returning
     * the descriptor binary instead of actually building the descriptor.
     */

    struct Patch {
        uintptr_t address;
        const uint32_t *insns;
        size_t insnCount;
    };

    for(uintptr_t address: {
        0x2567f04, // array element type checking

        0x3aa2e34, /* Google.Protobuf.dll::Google.Protobuf.Reflection.GeneratedClrTypeInfo::Art.ctor */
        0x3a9e678 /* Google.Protobuf.dll::Google.Protobuf.Reflection.FileDescriptor::FromGeneratedCode */
    }) {
        auto patchSite = reinterpret_cast<uint32_t *>(
            static_cast<char *>(translator_get_il2cpp_base_address()) + address
        );

        mprotect(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(patchSite) & ~4095), 8192, PROT_READ | PROT_WRITE | PROT_EXEC);


        *patchSite = 0xD65F03C0;

        translator_flush_instruction_cache(patchSite, sizeof(uint32_t));
    }

    auto objectHeaderSize = il2cpp_array_object_header_size();

    google::protobuf::DescriptorPool pool;

    for(const char *className: {
#if 0
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Data.DataServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Config.ConfigServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Weapon.WeaponServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Gift.GiftServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Mission.MissionServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Quest.QuestServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Movie.MovieServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.User.UserServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.NaviCutIn.NaviCutInServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Notification.NotificationServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Omikuji.ServiceReflection",
#endif
        "Google.Protobuf.dll::Google.Protobuf.WellKnownTypes.EmptyReflection",
        "Google.Protobuf.dll::Google.Protobuf.WellKnownTypes.TimestampReflection",

        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Data.DataServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Weapon.WeaponServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.User.UserServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Deck.DeckServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Tutorial.TutorialServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.SideStoryQuest.SideStoryQuestServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Shop.ShopServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Battle.BattleServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.BigHunt.BigHuntServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Labyrinth.LabyrinthServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Pvp.PvpServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Reward.RewardServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Gift.GiftServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Mission.MissionServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Quest.QuestServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.PortalCage.PortalCageServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Parts.PartsServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Omikuji.OmikujiServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Notification.NotificationServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.NaviCutIn.NaviCutInServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Movie.MovieServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Material.MaterialServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.LoginBonus.LoginBonusServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.IndividualPop.IndividualPopServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Gimmick.GimmickServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Gacha.GachaServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.GamePlay.GamePlayServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Friend.FriendServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Explore.ExploreServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Dokan.DokanServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Costume.CostumeServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.ContentsStory.ContentsStoryServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.ConsumableItem.ConsumableItemServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Config.ConfigServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Companion.CompanionServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.CharacterViewer.CharacterViewerServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Character.CharacterServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.CharacterBoard.CharacterBoardServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.CageOrnament.CageOrnamentServiceReflection",
        "Assembly-CSharp.dll::Art.Framework.ApiNetwork.Grpc.Api.Banner.BannerServiceReflection",
    }) {
        auto classDesc = getClassByFullName(className);
        if(!classDesc) {
            fprintf(stderr, "%s was not found\n", className);
            return 1;
        }

        auto descriptorProperty = il2cpp_class_get_property_from_name(classDesc, "Descriptor");
        if(!descriptorProperty) {
            fprintf(stderr, "Descriptor was not found in %s\n", className);
            return 1;
        }

        auto getMethod = il2cpp_property_get_get_method(const_cast<PropertyInfo *>(descriptorProperty));

        Il2CppException *exception = nullptr;
        auto result = reinterpret_cast<Il2CppArray *>(il2cpp_runtime_invoke(getMethod, classDesc, nullptr, &exception));

        if(exception) {
            fprintf(stderr, "An exception has occurred invoking %s::Descriptor.get:\n", className);

            printException(exception);

            return 1;
        }

        auto length = il2cpp_array_length(result);
        auto data = reinterpret_cast<unsigned char *>(result) + objectHeaderSize;

        auto resultClass = il2cpp_object_get_class(reinterpret_cast<Il2CppObject *>(result));

        google::protobuf::FileDescriptorProto descriptor;
        if(!descriptor.ParseFromArray(data, length)) {
            fprintf(stderr, "failed to parse the descriptor of %s\n", className);
            return 1;
        }

        auto fullDescriptor = pool.BuildFile(descriptor);

        google::protobuf::DebugStringOptions options;
        options.include_comments = true;

        std::filesystem::create_directories(std::filesystem::path(fullDescriptor->name()).parent_path());

        std::ofstream stream;
        stream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
        stream.open(fullDescriptor->name().c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
        stream << fullDescriptor->DebugStringWithOptions(options);
    }

    return 0;
}

static int tool_noengine_main(int argc, char **argv) {

    il2cpp_set_data_dir("../graft/NieR_Data/il2cpp_data");
    auto result = il2cpp_init("IL2CPP Root Domain");
    if(!result) {
        fprintf(stderr, "il2cpp has failed to initialize.\n");
        return 1;
    }

    return tool_main();
}

static void tool() {
    exit(tool_main());
}

int main(int argc, char *argv[]) {
    translator_set_post_initialize_callback(tool);

    //return translator_main(argc, argv, PlayerMain);
    return translator_main(argc, argv, tool_noengine_main);
}
