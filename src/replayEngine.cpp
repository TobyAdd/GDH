#include "replayEngine.hpp"
#include "memory.hpp"
#include "gui.hpp"
#include "hacks.hpp"

ReplayEngine engine;

int ReplayEngine::get_tick() {
    auto gjbgl = GJBaseGameLayer::get();
    if (!gjbgl) return 0;
    
    return gjbgl->m_gameState.m_currentProgress;
}

void ReplayEngine::handle_recording(bool hold, int button, bool player) {
    if (mode == state::record)
        replay.push_back({get_tick(), hold, button, player});
}

void ReplayEngine::handle_recording2(bool player) {
    if (mode == state::record) {
        auto gjbgl = GJBaseGameLayer::get();
        if (!player && !gjbgl->m_gameState.m_isDualMode)
            return;

        int tick = get_tick();

        bool frameExists = std::find_if(replay2.begin(), replay2.end(), [&](const auto &data)
                                    { return data.tick == tick && data.player == player; }) != replay2.end();

        if (frameExists)
        {
            return;
        }

        replay2.push_back({tick,
                        player ? gjbgl->m_player1->m_position.x : gjbgl->m_player2->m_position.x,
                        player ? gjbgl->m_player1->m_position.y : gjbgl->m_player2->m_position.y,
                        player ? gjbgl->m_player1->getRotation() : gjbgl->m_player2->getRotation(),
                        player ? gjbgl->m_player1->m_yVelocity : gjbgl->m_player2->m_yVelocity,
                        player});
    }
}

void ReplayEngine::handle_playing()
{
    if (mode != state::play)
        return;

    auto gjbgl = GJBaseGameLayer::get();
    unsigned tick = get_tick();

    while (index < (int)replay.size() && tick >= replay[index].tick)
    {
        gjbgl->handleButton(replay[index].hold, replay[index].button, replay[index].player);
        index++;
    }

    while (index2 < (int)replay2.size() && tick >= replay2[index2].tick)
    {
        if (replay[index2].player)
        {
            gjbgl->m_player1->m_position.x = replay2[index2].x;
            gjbgl->m_player1->m_position.y = replay2[index2].y;
            gjbgl->m_player1->setRotation(replay2[index2].rotation);
            gjbgl->m_player1->m_yVelocity = replay2[index2].y_accel;
        }
        else
        {
            gjbgl->m_player2->m_position.x = replay2[index2].x;
            gjbgl->m_player2->m_position.y = replay2[index2].y;
            gjbgl->m_player2->setRotation(replay2[index2].rotation);
            gjbgl->m_player2->m_yVelocity = replay2[index2].y_accel;
        }
        index2++;
    }
}

void ReplayEngine::handle_reseting() {
    int lastCheckpointFrame = get_tick();

    if (mode == state::record) {
        remove_actions(lastCheckpointFrame);

        if (!replay.empty() && replay.back().hold) {
            handle_recording(false, replay.back().button, replay.back().player);
        }
    }
    else if (mode == state::play) {
        index = 0;
        index2 = 0;
    }
}

void ReplayEngine::remove_actions(int tick)
{
    auto check = [&](ReplayData &action) -> bool
    {
        return action.tick >= tick;
    };
    replay.erase(remove_if(replay.begin(), replay.end(), check), replay.end());

    auto check2 = [&](ReplayData2 &action) -> bool
    {
        return action.tick > tick;
    };
    replay2.erase(remove_if(replay2.begin(), replay2.end(), check2), replay2.end());
}

std::string ReplayEngine::save(std::string name)
{
    if (replay.empty())
        return "Replay doesn't have actions";

    std::ofstream file(hacks::folderMacroPath / std::string(name + ".re"), std::ios::binary);

    std::string magicString = "ENGINE2";
    file.write(magicString.c_str(), magicString.size());

    unsigned replay_size = (unsigned int)replay.size();

    file.write(reinterpret_cast<char *>(&replay_size), sizeof(replay_size));

    file.write(reinterpret_cast<char *>(&replay[0]), sizeof(ReplayData) * replay_size);

    unsigned replay_size2 = (unsigned int)replay2.size();

    file.write(reinterpret_cast<char *>(&replay_size2), sizeof(replay_size2));

    file.write(reinterpret_cast<char *>(&replay2[0]), sizeof(ReplayData2) * replay_size2);

    file.close();
    return "Replay saved";
}

std::string ReplayEngine::load(std::string name)
{
    if (!replay.empty())
        return "Please clear replay before loading another";

    std::ifstream file(hacks::folderMacroPath / std::string(name + ".re"), std::ios::binary);
    if (!file)
        return "Replay doesn't exist";

    std::string expectedMagicString = "ENGINE2";
    std::string magicString(expectedMagicString.size(), '\0');

    file.read(&magicString[0], expectedMagicString.size());

    if (magicString != expectedMagicString) {
        file.close();
        return "Invalid replay file";
    }

    unsigned replay_size = 0;
    file.read(reinterpret_cast<char *>(&replay_size), sizeof(replay_size));
    replay.resize(replay_size);
    file.read(reinterpret_cast<char *>(&replay[0]), sizeof(ReplayData) * replay_size);

    unsigned replay_size2 = 0;
    file.read(reinterpret_cast<char *>(&replay_size2), sizeof(replay_size2));
    replay2.resize(replay_size2);
    file.read(reinterpret_cast<char *>(&replay2[0]), sizeof(ReplayData2) * replay_size2);

    file.close();
    return "Replay loaded";
}

std::vector<std::string> replay_list;
void openSelectReplay(const char *str_id) {
    replay_list.clear();
    for (const auto &entry : std::filesystem::directory_iterator(hacks::folderMacroPath)) {
        std::string replay = entry.path().filename().string();
        if (replay.size() >= 3 && replay.substr(replay.size() - 3) == ".re") {
            replay_list.push_back(replay.substr(0, replay.size() - 3));
        }
    }
    ImGui::OpenPopup(str_id);
}

void ReplayEngine::openReplayMultishit() {
    ImGui::BeginChild("Select Replay", {400 * gui::scale, 300 * gui::scale});
    for (int i = 0; i < (int)replay_list.size(); i++)
    {
        if (ImGui::Button(replay_list[i].c_str(), {ImGui::GetContentRegionAvail().x, NULL}))
        {
            replay_name = replay_list[i];
            ImGui::CloseCurrentPopup();
        }
    }
    ImGui::EndChild();
    
    if (ImGui::Button("Close", {ImGui::GetContentRegionAvail().x, NULL})) {
        ImGui::CloseCurrentPopup();
    }
}

void ReplayEngine::render() {
    if (ImGui::BeginPopupModal("Select Replay", 0, ImGuiWindowFlags_NoResize)) {
        openReplayMultishit();
        ImGui::EndPopup();
    }

    int mode_ = (int)mode;

    if (ImGui::RadioButton("Disable", &mode_, 0, gui::scale))
        mode = state::disable;

    ImGui::SameLine();

    if (ImGui::RadioButton("Record", &mode_, 1, gui::scale))
    {
        if (mode != state::record) {
            replay.clear();
        }
        mode = state::record;
    }
    
    ImGui::SameLine();

    if (ImGui::RadioButton("Play", &mode_, 2, gui::scale)) {
        mode = state::play;
    }  

    ImGui::Separator();   

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 35 * gui::scale);
    ImGui::InputText("##replay_name", &replay_name);

    ImGui::SameLine();

    if (ImGui::ArrowButton("##replay_select", ImGuiDir_Down)) {
        openSelectReplay("Select Replay");
    }

    if (ImGui::Button("Save", {ImGui::GetContentRegionAvail().x / 3, NULL})) {
        log = save(replay_name);
    }
    ImGui::SameLine();

    if (ImGui::Button("Load", {ImGui::GetContentRegionAvail().x / 2, NULL})) {
        log = load(replay_name);
    }
    ImGui::SameLine();

    if (ImGui::Button("Clear", {ImGui::GetContentRegionAvail().x, NULL})) {
        replay.clear();
        log = "The replay has been cleared";
    }

    ImGui::Text("Replay Size: %i", replay.size());
    ImGui::Text("Tick: %i", get_tick());

    ImGui::Separator();

    ImGui::Text("%s", log.c_str());
}