#include "ServerCommandLine.h"
#include "DataModel/DatabaseContext.h"
#include "DataModel/DatabaseEnums.h"
#include "DataModel/Sqlite/Transaction.h"
#include "DataModel/Sqlite/Statement.h"

#include "DataModel/UserContext.h"
#include "WordListParser.h"

#include "DataModel/Database.h"
#include "service/GiftService.pb.h"

#include <LLServices/Logging/LogFacility.h>

#include <ctime>
#include <filesystem>
#include <string_view>

LLServices::LogFacility LogCLI("CLI");

const ServerCommandLine::Command ServerCommandLine::m_commands[]{
    { .cmd = "help", .help = "output the list of supported commands", .handler = &ServerCommandLine::commandHelp },
    { .cmd = "backup", .help = "back up the database (save file). If a name is not specified, it'll be picked automatically",
        .handler = &ServerCommandLine::commandBackup
    },
    { .cmd = "backups", .help = "output the list of available backups", .handler = &ServerCommandLine::commandBackups },
    { .cmd = "restore", .help = "restore a previous version of the database (save file)", .handler = &ServerCommandLine::commandRestore },
    { .cmd = "timetravel", .help = "adjusts the server time to be different from the real-world time",
        .handler = &ServerCommandLine::commandTimeTravel },
    { .cmd = "present", .help = "adjusts the server time back to the real-world time", .handler = &ServerCommandLine::commandPresent },
    { .cmd = "gift", .help = "sends a gift (execute just 'gift' for the syntax details)", .handler = &ServerCommandLine::commandGift },
    { .cmd = "portalcage", .help = "moves the player to the Mama's Room (will log out)", .handler = &ServerCommandLine::commandPortalCage },
    { .cmd = "addpremium", .help = "adds the premium item with the specified ID (will log out)", .handler = &ServerCommandLine::commandAddPremiumItem },
    { .cmd = "allweapons", .help = "gives the player all purchasable (via the store or gacha) weapons and costumes that they don't already have (will log out)", .handler = &ServerCommandLine::commandAllWeapons },
    { .cmd = "reunlock",   .help = "recalculate all user unlockables (will log out)", .handler = &ServerCommandLine::commandUpdateUnlocks },
};

void ServerCommandLine::commandHelp(WordListParser &parser) {
    size_t maxLength = 0;
    for(const auto &command: m_commands) {
        maxLength = std::max<size_t>(maxLength, command.cmd.size());
    }

    maxLength += 3;

    LogCLI.info("Commands supported by this server:");
    for(const auto &command: m_commands) {
        LogCLI.info("%*.*s%.*s",
                    -static_cast<int>(maxLength),
                    static_cast<int>(command.cmd.size()),
                    command.cmd.data(),
                    static_cast<int>(command.help.size()),
                    command.help.data());
    }
}

std::filesystem::path ServerCommandLine::backupLocation() const {

    std::filesystem::path backupLocation = m_db.databaseDirectory() / "backups";

    std::filesystem::create_directories(backupLocation);

    return backupLocation;
}

void ServerCommandLine::commandBackup(WordListParser &parser) {
    std::string backupName;

    if(parser.isAtEnd()) {
        time_t currentTime;
        time(&currentTime);

        struct tm currentTimeParts;
#ifdef _WIN32
        gmtime_s(&currentTimeParts, &currentTime);
#else
        gmtime_r(&currentTime, &currentTimeParts);
#endif

        char time[64];
        strftime(time, sizeof(time), "%Y%m%d_%H%M%S", &currentTimeParts);

        backupName.assign(time);
    } else {
        backupName = parser.getRestOfTheString();
    }

    auto backupLocation = this->backupLocation();
    backupLocation /= std::u8string_view(reinterpret_cast<const char8_t *>(backupName.data()), backupName.size());
    backupLocation += ".dbjson";

    DatabaseContext ctx(m_db);
    ctx.writeJSONBackup(backupLocation);

    LogCLI.info("The backup file '%s' has been created and saved at:", backupName.c_str());

    auto finalPath = backupLocation.generic_u8string();
    LogCLI.info("%s", reinterpret_cast<const char *>(finalPath.c_str()));
}

void ServerCommandLine::commandBackups(WordListParser &parser) {

    bool hasAnyFiles = false;

    auto backupLocation = this->backupLocation();

    auto pathU8 = backupLocation.u8string();

    for(const auto &entry: std::filesystem::directory_iterator(backupLocation)) {
        const auto &path = entry.path();

        if(!entry.is_regular_file() || !path.has_extension() || path.extension() != ".dbjson")
            continue;

        if(!hasAnyFiles) {
            hasAnyFiles = true;
            LogCLI.info("Available backup files that can be restored with the 'restore' command:");
        }

        auto name = path.stem().u8string();
        LogCLI.info("%s", reinterpret_cast<const char *>(name.c_str()));
    }

    if(hasAnyFiles) {
        LogCLI.info("\nThe backup files are stored in the following directory: %s",
                    reinterpret_cast<const char *>(pathU8.data()));
    } else {
        LogCLI.info("No backup files have been made yet. Backups can be created with the 'backup' command.");

        LogCLI.info("Once any are made, the backup files will be stored in the following directory: %s",
                    reinterpret_cast<const char *>(pathU8.data()));
    }
}

void ServerCommandLine::commandRestore(WordListParser &parser) {

    if(parser.isAtEnd()) {
        LogCLI.error("Please specify the name of the backup file to be restored. Try 'backups' for the list.");
        return;
    }

    auto name = parser.getRestOfTheString();
    std::u8string_view u8name(reinterpret_cast<const char8_t *>(name.data()), name.size());

    std::filesystem::path backupLocation = this->backupLocation();
    backupLocation /= u8name;
    backupLocation += ".dbjson";

    if(!std::filesystem::exists(backupLocation)) {
        LogCLI.error("The specified backup file doesn't exist. Try 'backups' for the list.");
        return;
    }

    {
        Database restoredDB(std::filesystem::path(), m_db.masterDatabase());
        {
            sqlite::Transaction restoreTransaction(&restoredDB.db());
            DatabaseContext restoredContext(restoredDB);

            restoredContext.restoreJSONBackup(backupLocation);
            restoreTransaction.commit();
        }

        restoredDB.db().checkpoint("main", SQLITE_CHECKPOINT_TRUNCATE);
        restoredDB.db().prepare("VACUUM")->exec();

        m_db.restoreFromDB(restoredDB);
    }

    LogCLI.debug("The backup has been successfully restored.");
}

void ServerCommandLine::commandTimeTravel(WordListParser &parser) {
    /*
     * Calculate the time offset using SQLite's time functions, since their
     * syntax is friendly enough.
     */

    std::stringstream query;

    query << "SELECT CAST(round((unixepoch(";

    std::vector<std::string> args;
    while(!parser.isAtEnd()) {
        if(!args.empty())
            query << ", ";

        args.emplace_back(parser.getStringWord());

        query << "?";
    }

    if(!args.empty()) {
        query << ", ";
    }

    query << "'subsec') - unixepoch('subsec')) * 1000) AS INT)";

    auto statement = m_db.db().prepare(query.str());
    int index = 1;
    for(const auto &arg: args) {
        statement->bind(index, arg);
        index++;
    }

    /*
     * On error, this will return an empty string.
     */
    std::optional<int64_t> offset;
    if(!statement->step() || statement->columnType(0) != SQLITE_INTEGER)
        throw std::runtime_error("unable to parse the time specification");

    offset.emplace(statement->columnInt64(0));

    statement->reset();

    if(offset.has_value() && *offset == 0)
        offset.reset();

    m_db.setTimeOffset(offset);
}

void ServerCommandLine::commandPresent(WordListParser &parser) {
    m_db.setTimeOffset(std::nullopt);
}

void ServerCommandLine::commandGift(WordListParser &parser) {
    runCommandInDatabaseContext(parser, &ServerCommandLine::commandGiftDatabase);
}

void ServerCommandLine::commandGiftDatabase(WordListParser &parser, DatabaseContext &db) {
    if(parser.isAtEnd()) {
        LogCLI.info(
            "The gift command add items to the user's gift box, and can be used to easily add items to the game's inventory.\n"
            "\n"
            "Usage: gift <ITEM TYPE> <ITEM ID WITHIN THE TYPE> [EXTRA PARAMETER=EXTRA PARAMETER VALUE]...\n"
            "\n"
            "Example: gift costume 21000\n"
            "Example: gift gold count=10000\n"
            "\n"
            "Item types:\n"
            " - 'costume'\n"
#if 0
            "    Accepts parameter 'level' specifying the level of the costume (default: 1)\n"
            "    Accepts parameter 'exp' specifying the experience of the costume (default: determined based on the level)\n"
            "    Accepts parameter 'limit_break_count' specifying the number of ascensions performed on the costume (default: 0)\n"
            "    Accepts parameter 'active_skill_level' specifying the level of the skill of the costume (default: 1)\n"
#endif
            " - 'weapon'\n"
#if 0
            "   Accepts parameter 'level' specifying the level of the weapon (default: 1)\n"
            "   Accepts parameter 'exp' specifying the experience of the weapon (default: determined based on the level)\n"
            "   Accepts parameter 'limit_break_count' specifying the number of ascensions performed on the weapon (default: 0)\n"
            "   Accepts parameters 'skill<SKILL ID>' specifying the level of the corresponding skill (default: not set)\n"
#endif
            " - 'companion'\n"
#if 0
            "   Accepts parameter 'level' specifying the level of the companion (default: 1)\n"
#endif
            " - 'parts' (memoirs)\n"
            " - 'material'\n"
            " - 'consumable_item'\n"
            " - 'gold': shorthand for 'consumable_item %d'; the item ID doesn't need to be specified\n"
            " - 'paid_gem': no item ID should be specified\n"
            " - 'free_gem': no item ID should be specified\n"
            " - 'important_item'\n"
            " - 'thought' (debris)\n"
            //" - 'mission_pass_point'\n"
            // seems to not show up correctly
            //" - 'premium_item'\n"
            "\n"
#if 0
            "WARNING: Setting the leveling parameters on items to the values outside of the normal bounds may cause game client errors.\n"
            "         Create a backup if you're not sure about the parameters you're setting.\n"
            "         Not setting any leveling parameters is always safe.\n"
#endif
            "The following parameters apply to all item types:\n"
            "  'count': the number of items to be gifted, default: 1\n"
            "  'gift_text': the ID of the gift message text to be used, default: '13' for 'sent by administration'\n"
            "  'expires': the amount of time in which the gift will expire unless claimed, such as 'expires=\"7 days\"'; default: no expiration\n"
            , db.consumableItemIdForGold()
        );

        return;
    }

    PossessionType possessionType;
    std::optional<int32_t> possessionId;

    auto possessionTypeString = parser.getStringWord();

    if(possessionTypeString == "costume") {
        possessionType = PossessionType::COSTUME;

    } else if(possessionTypeString == "weapon") {
        possessionType = PossessionType::WEAPON;
    } else if(possessionTypeString == "companion") {
        possessionType = PossessionType::COMPANION;
    } else if(possessionTypeString == "parts") {
        possessionType = PossessionType::PARTS;
    } else if(possessionTypeString == "material") {
        possessionType = PossessionType::MATERIAL;
    } else if(possessionTypeString == "consumable_item") {
        possessionType = PossessionType::CONSUMABLE_ITEM;
    } else if(possessionTypeString == "gold") {
        possessionType = PossessionType::CONSUMABLE_ITEM;
        possessionId.emplace(db.consumableItemIdForGold());
    } else if(possessionTypeString == "paid_gem") {
        possessionType = PossessionType::PAID_GEM;
        possessionId.emplace(0);
    } else if(possessionTypeString == "free_gem") {
        possessionType = PossessionType::FREE_GEM;
        possessionId.emplace(0);
    } else if(possessionTypeString == "paid_gem") {
        possessionType = PossessionType::PAID_GEM;
        possessionId.emplace(0);
    } else if(possessionTypeString == "important_item") {
        possessionType = PossessionType::IMPORTANT_ITEM;
    } else if(possessionTypeString == "thought") {
        possessionType = PossessionType::THOUGHT;
    // seems to not show up correctly
    //} else if(possessionTypeString == "premium_item") {
    //    possessionType = PossessionType::PREMIUM_ITEM;
    } else {
        LogCLI.error("incorrect possession type '%s'\n", possessionTypeString.c_str());
        return;
    }

    if(!possessionId.has_value()) {
        if(parser.isAtEnd()) {
            LogCLI.error("this possession type requires an ID\n");
            return;
        }

        parser.parse(possessionId.emplace());
    }

    if(!db.isValidPossession(possessionType, *possessionId)) {
        LogCLI.error("no such possession exists\n");
        return;
    }

    int64_t expiresAt = 0;

    apb::api::gift::GiftCommon gift;
    gift.set_possession_type(static_cast<int32_t>(possessionType));
    gift.set_possession_id(*possessionId);
    gift.set_count(1);
    gift.set_description_gift_text_id(13);

#if 0
    if(possessionType == PossessionType::COSTUME) {
        auto costume = gift.mutable_costume();
        costume->set_level(1);
        costume->set_active_skill_level(1);
        costume->set_exp(0);
        costume->set_limit_break_count(0);
    }
#endif

    // TODO: is the extra data (costume data, etc) required?

    while(!parser.isAtEnd()) {
        auto pair = parser.getStringWord();

        auto delim = pair.find_first_of('=');
        if(delim == std::string_view::npos) {
            LogCLI.error("no key-value delimiter '=' in parameter '%s'\n", pair.c_str());
            return;
        }

        auto key = pair.substr(0, delim);
        auto value = pair.substr(delim + 1);

        if(key == "count") {
            gift.set_count(WordListParser::parseInteger<int32_t>(value));
        } else if(key == "gift_text") {
            gift.set_description_gift_text_id(WordListParser::parseInteger<int32_t>(value));
        } else if(key == "expires") {

            auto query = db.db().prepare("SELECT ROUND(unixepoch(current_net_timestamp() / 1000.0, 'unixepoch', ?, 'subsec') * 1000)");
            query->bind(1, value);
            if(query->step()) {
                expiresAt = query->columnInt64(0);
            }
        } else {
            LogCLI.error("unknown parameter: '%.*s'\n", static_cast<int>(key.size()), key.data());
            return;
        }
    }

    if(gift.count() <= 0) {
        LogCLI.error("the possession count cannot be zero or negative");
        return;
    }

    db.gift(FixedUserID, gift, expiresAt);

}

void ServerCommandLine::runCommandInDatabaseContext(WordListParser &parser, void (ServerCommandLine::*command)(WordListParser &parser, DatabaseContext &db)) {
    sqlite::Transaction transaction(&m_db.db());

    DatabaseContext db(m_db);

    (this->*command)(parser, db);

    transaction.commit();

}

void ServerCommandLine::runCommandInUserContext(WordListParser &parser, void (ServerCommandLine::*command)(WordListParser &parser, UserContext &user)) {
    sqlite::Transaction transaction(&m_db.db());

    DatabaseContext db(m_db);
    db.registerUser();

    int64_t userId;
    std::string outputSession;
    time_t outputExpiration;
    db.authenticate(userId, outputSession, outputExpiration);

    UserContext user(db, userId);

    (this->*command)(parser, user);

    transaction.commit();
}

void ServerCommandLine::commandPortalCage(WordListParser &parser) {
    runCommandInUserContext(parser, &ServerCommandLine::commandPortalCageUser);
}


void ServerCommandLine::commandPortalCageUser(WordListParser &parser, UserContext &user) {
    user.updatePortalCageSceneProgress(user.getIntConfig("PORTAL_CAGE_SCENE_ID"));
}

void ServerCommandLine::commandAddPremiumItem(WordListParser &parser) {
    runCommandInUserContext(parser, &ServerCommandLine::commandAddPremiumItemUser);
}


void ServerCommandLine::commandAddPremiumItemUser(WordListParser &parser, UserContext &user) {
    if(parser.isAtEnd()) {
        LogCLI.error("item ID must be specified");
        return;
    }

    int32_t itemID;
    parser.parse(itemID);

    if(!user.isValidPossession(PossessionType::PREMIUM_ITEM, itemID)) {
        LogCLI.error("no premium item with such ID exists");
        return;
    }

    user.givePossession(static_cast<int32_t>(PossessionType::PREMIUM_ITEM), itemID, 1);
}

void ServerCommandLine::commandAllWeapons(WordListParser &parser) {
    runCommandInUserContext(parser, &ServerCommandLine::commandAllWeaponsUser);
}

void ServerCommandLine::commandAllWeaponsUser(WordListParser &parser, UserContext &user) {
    auto selectAllPurchasableWeapons = user.db().prepare(R"SQL(
        SELECT DISTINCT possession_type, possession_id
        FROM
            m_shop,
            m_shop_item_cell_group USING (shop_item_cell_group_id),
            m_shop_item_cell USING (shop_item_cell_id),
            m_shop_item_content_possession USING (shop_item_id)
        WHERE
            shop_id = 6105
    )SQL");

    while(selectAllPurchasableWeapons->step()) {
        auto possessionType = static_cast<PossessionType>(selectAllPurchasableWeapons->columnInt(0));
        auto possessionId = selectAllPurchasableWeapons->columnInt(1);

        if((possessionType == PossessionType::WEAPON && !user.getWeaponLevel(possessionId).has_value()) ||
           (possessionType == PossessionType::COSTUME && !user.hasCostume(possessionId))) {

            user.givePossession(static_cast<int32_t>(possessionType), possessionId, 1);
        }
    }
}

void ServerCommandLine::commandUpdateUnlocks(WordListParser &parser) {
    runCommandInUserContext(parser, &ServerCommandLine::commandUpdateUnlocksUser);
}

void ServerCommandLine::commandUpdateUnlocksUser(WordListParser &parser, UserContext &user) {
    user.updateUserUnlocks();
}

ServerCommandLine::ServerCommandLine(Database &db) : m_db(db) {

}

ServerCommandLine::~ServerCommandLine() = default;

void ServerCommandLine::execute(const std::string_view &command) {
    try {

        LogCLI.info("%.*s", static_cast<int>(command.size()), command.data());

        WordListParser parser(command);
        parser.skipSpaces();

        if(parser.isAtEnd())
            return;

        auto cmdToken = parser.getStringWord();

        for(const auto &command: m_commands) {
            if(command.cmd == cmdToken) {
                (this->*command.handler)(parser);

                return;
            }
        }

        LogCLI.error("unsupported command: '%s'. Try 'help'.", cmdToken.c_str());

    } catch(const std::exception &e) {
        LogCLI.error("an exception has occurred while executing the console command: %s", e.what());
    } catch(...) {
        LogCLI.error("an exception has occurred while executing the console command");
    }
}
