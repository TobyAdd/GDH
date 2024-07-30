#include "replayEngine.hpp"
#include "hacks.hpp"
#include "gui.hpp"

ReplayEngine engine;

unsigned ReplayEngine::get_frame() {
    auto pl = GameManager::sharedState()->getPlayLayer();
    if (pl)
        return static_cast<unsigned>(from<double>(pl, 0x3C8) * hacks::tps_value);
    return 0;
} 


void ReplayEngine::handle_recording(GJBaseGameLayer* self, bool player) {
    if (!player && !self->m_gameState.m_isDualMode)
        return;

    unsigned frame = get_frame();

    bool frameExists = std::find_if(replay.begin(), replay.end(), [&](const auto &data)
                                    { return data.frame == frame && data.player == player; }) != replay.end();

    if (frameExists)
    {
        return;
    }

    replay.push_back({frame,
                      player ? self->m_player1->m_position.x : self->m_player2->m_position.x,
                      player ? self->m_player1->m_position.y : self->m_player2->m_position.y,
                      player ? self->m_player1->getRotation() : self->m_player2->getRotation(),
                      player ? self->m_player1->m_yVelocity : self->m_player2->m_yVelocity,
                      player});
}

void ReplayEngine::handle_recording2(bool hold, int button, bool player) {
    replay2.push_back({get_frame(), hold, button, player});
}

void ReplayEngine::handle_playing(GJBaseGameLayer* self)
{
    unsigned frame = get_frame();

    if (accuracy_fix) {
        while (index < (int)replay.size() && frame >= replay[index].frame)
        {
            if (replay[index].player)
            {
                self->m_player1->m_position.x = replay[index].x;
                self->m_player1->m_position.y = replay[index].y;
                if (rotation_fix)
                    self->m_player1->setRotation(replay[index].rotation);
                self->m_player1->m_yVelocity = replay[index].y_accel;
            }
            else
            {
                self->m_player2->m_position.x = replay[index].x;
                self->m_player2->m_position.y = replay[index].y;
                if (rotation_fix)
                    self->m_player2->setRotation(replay[index].rotation);
                self->m_player2->m_yVelocity = replay[index].y_accel;
            }
            index++;
        }
    }


    while (index2 < (int)replay2.size() && frame >= replay2[index2].frame)
    {
        self->handleButton(replay2[index2].hold, replay2[index2].button, replay2[index2].player);
        index2++; 
    }
}

void ReplayEngine::handle_reseting(PlayLayer* self) {
    int lastCheckpointFrame = get_frame();

    if (mode == state::record) {
        remove_actions(lastCheckpointFrame);

        if (!replay2.empty() && replay2.back().hold) {
            handle_recording2(false, replay2.back().button, replay2.back().player);
        }
    }
    else if (mode == state::play) {
        index = 0;
        index2 = 0;
    }
}

std::string ReplayEngine::save(std::string name)
{
    if (replay2.empty())
        return "Replay doesn't have actions";

    std::ofstream file(hacks::folderMacroPath / std::string(name + ".re"), std::ios::binary);

    file.write(reinterpret_cast<char *>(&hacks::tps_value), sizeof(hacks::tps_value));

    unsigned replay_size = replay.size();
    unsigned replay2_size = replay2.size();

    file.write(reinterpret_cast<char *>(&replay_size), sizeof(replay_size));
    file.write(reinterpret_cast<char *>(&replay2_size), sizeof(replay2_size));

    file.write(reinterpret_cast<char *>(&replay[0]), sizeof(replay_data) * replay_size);
    file.write(reinterpret_cast<char *>(&replay2[0]), sizeof(replay_data2) * replay2_size);

    file.close();
    return "Replay saved";
}

std::string ReplayEngine::load(std::string name)
{
    if (!replay2.empty())
        return "Please clear replay before loading another";

    std::ifstream file(hacks::folderMacroPath / std::string(name + ".re"), std::ios::binary);
    if (!file)
        return "Replay doesn't exist";

    file.read(reinterpret_cast<char *>(&hacks::tps_value), sizeof(hacks::tps_value));

    hacks::update_framerate();

    unsigned replay_size = 0;
    unsigned replay2_size = 0;

    file.read(reinterpret_cast<char *>(&replay_size), sizeof(replay_size));
    file.read(reinterpret_cast<char *>(&replay2_size), sizeof(replay2_size));

    replay.resize(replay_size);
    replay2.resize(replay2_size);

    file.read(reinterpret_cast<char *>(&replay[0]), sizeof(replay_data) * replay_size);
    file.read(reinterpret_cast<char *>(&replay2[0]), sizeof(replay_data2) * replay2_size);

    file.close();
    return "Replay loaded";
}

void ReplayEngine::remove_actions(unsigned frame)
{
    auto check = [&](replay_data &action) -> bool
    {
        return action.frame > frame;
    };
    replay.erase(remove_if(replay.begin(), replay.end(), check), replay.end());

    auto check2 = [&](replay_data2 &action) -> bool
    {
        return action.frame >= frame;
    };
    replay2.erase(remove_if(replay2.begin(), replay2.end(), check2), replay2.end());
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
    ImGui::BeginChild("Select Replay##3", {400 * gui::scale, 300 * gui::scale});
    for (int i = 0; i < (int)replay_list.size(); i++)
    {
        if (ImGui::Button(replay_list[i].c_str(), {ImGui::GetContentRegionAvail().x, NULL}))
        {
            replay_name = replay_list[i];
            ImGui::CloseCurrentPopup();
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("Open Folder", {ImGui::GetContentRegionAvail().x, NULL})) {
        ShellExecuteW(nullptr, L"open", L"explorer", hacks::folderMacroPath.wstring().c_str(), nullptr, SW_SHOWDEFAULT);
    }
    
    if (ImGui::Button("Close", {ImGui::GetContentRegionAvail().x, NULL})) {
        ImGui::CloseCurrentPopup();
    }
}

void ReplayEngine::render() {
    if (ImGui::BeginPopupModal("Select Replay", 0, ImGuiWindowFlags_NoResize)) {
        openReplayMultishit();
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Replay Engine Settings", 0, ImGuiWindowFlags_NoResize)) {
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::DragFloat("##tps_value", &hacks::tps_value, 1, 1, FLT_MAX, "%0.f TPS")) {
            hacks::update_framerate();
        }

        if (ImGui::IsItemHovered()) 
            ImGui::SetTooltip("Recommend setting the recording to 240 TPS to ensure stability in both recording and playback of the macro");

        ImGui::Checkbox("Real Time", &engine.real_time, gui::scale);

        //ImGui::SameLine();

        //ImGui::Checkbox("Ignore Inputs", &engine.ignore_inputs, gui::scale);

        ImGui::Checkbox("Accuracy Fix", &engine.accuracy_fix, gui::scale);

        ImGui::SameLine();

        ImGui::Checkbox("Rotation Fix", &engine.rotation_fix, gui::scale);

        if (ImGui::Button("Close", {ImGui::GetContentRegionAvail().x, NULL})) {
            ImGui::CloseCurrentPopup();
        }

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
            replay2.clear();
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
        replay2.clear();
        log = "Replay has been cleared";
    }

    ImGui::Text("Replay Size: %zu", replay2.size());
    ImGui::Text("Frame: %i", get_frame());

    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImColor(64, 64, 64).Value);
    if (ImGui::MenuItem(log.c_str())) {
        ImGui::OpenPopup("Replay Engine Settings");
    }
    ImGui::PopStyleColor();
}