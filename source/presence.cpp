#include "presence.hpp"

std::string presence::player_name = "";
long long presence::rpcStartTime = 0;

const char* getLevelDifficultyAssetName(gd::GJGameLevel* m_lvl) {
    int stars = m_lvl->m_stars();
    int difficulty = m_lvl->getAverageDifficulty();
    int levelID = m_lvl->m_lvlID();

    if (stars == 0) {
        switch (difficulty) {
            case -1: return "auto";
            case 0: return "na";
            case 1: return "easy";
            case 2: return "normal";
            case 3: return "hard";
            case 4: return "harder";
            case 5: return "insane";
            case 6: return "hard_demon";
            default: break;
        }
    } else if (stars == 10) {
        switch (difficulty) {
            case 1: return "easy_demon";
            case 2: return "medium_demon";
            case 3: return "hard_demon";
            case 4: return "insane_demon";
            case 5: return "extreme_demon";
            default: break;
        }
    } else if (levelID >= 1 && levelID <= 22) {
        switch (levelID) {
            case 1: case 2: return "easy";
            case 3: case 4: return "normal";
            case 5: case 6: return "hard";
            case 7: case 8: case 9: return "harder";
            case 10: case 11: case 12: case 13: case 15: case 16: case 21: case 22: return "insane";
            case 17: case 19: return "harder";
            case 14: case 18: case 20: return "hard_demon";
            case 5001: return "hard";
            case 5002: case 5003: case 5004: return "harder";
            case 3001: return "hard";
            default: break;
        }
    } else {
        switch (stars) {
            case 1: return "auto";
            case 2: return "easy";
            case 3: return "normal";
            case 4: return "hard";
            case 5: case 6: return "harder";
            case 7: case 8: return "insane";
            case 9: return "insane";
            default: break;
        }
    }

    return "na";
}

void presence::InitPresence() {
    DiscordEventHandlers handlers;
    Discord_Initialize(APPLICATION_ID, &handlers, 1, "322170");

    rpcStartTime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    player_name = gd::GameManager::sharedState()->playerName();

    UpdatePresence(Status::IN_MENU);
}

void presence::UpdatePresence(Status status, gd::GJGameLevel *m_lvl) {
    if (!hacks::discord_rpc) return;

    long long levelStartTime = 0;
    DiscordRichPresence rpc{};

    if (!m_lvl) status = Status::IN_MENU;

    switch (status) {
        case Status::IN_LEVEL:
            rpc.state = "Playing a level";
            levelStartTime = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count();

            if (m_lvl->m_levelCreator().empty())
                rpc.details = std::format("{}", m_lvl->m_levelName().c_str()).c_str();
            else
                rpc.details = std::format("{} by {}", m_lvl->m_levelName().c_str(), m_lvl->m_levelCreator().c_str()).c_str();

            rpc.smallImageKey = getLevelDifficultyAssetName(m_lvl);
            break;
        case Status::IN_EDITOR:
            rpc.state = "Editing a level";
            rpc.details = m_lvl->m_levelName().c_str();
            rpc.smallImageKey = "editor";
            break;
        case Status::IN_MENU:
            rpc.state = "";
            rpc.details = "Browsing Menus";
            break;
    }

    rpc.startTimestamp = status == Status::IN_LEVEL ? levelStartTime : rpcStartTime;
    rpc.largeImageText = player_name.c_str();
    rpc.largeImageKey = "cool";
    rpc.instance = 0;
    Discord_UpdatePresence(&rpc);
}