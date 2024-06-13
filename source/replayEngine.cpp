#include "replayEngine.hpp"
#include "hacks.hpp"
#include "hooks.hpp"
#include <fstream>
#include "gui.hpp"
#include "labels.hpp"
#include <random>

ReplayEngine engine;

unsigned ReplayEngine::get_frame(gd::PlayLayer* self) {
    return static_cast<unsigned>(self->m_time() * hacks::fps_value);
} 

void ReplayEngine::handle_recording(gd::PlayLayer* self, bool player) {
    if (!player && !self->m_bTwoPlayerMode())
        return;

    unsigned frame = get_frame(self);

    bool frameExists = std::find_if(replay.begin(), replay.end(), [&](const auto &data)
                                    { return data.frame == frame && data.player == player; }) != replay.end();

    if (frameExists)
    {
        return;
    }

    replay.push_back({frame,
                      player ? self->m_pPlayer1()->m_position().x : self->m_pPlayer2()->m_position().x,
                      player ? self->m_pPlayer1()->m_position().y : self->m_pPlayer2()->m_position().y,
                      player ? self->m_pPlayer1()->getRotation() : self->m_pPlayer2()->getRotation(),
                      player ? self->m_pPlayer1()->m_yAccel() : self->m_pPlayer2()->m_yAccel(),
                      player});
}

void ReplayEngine::handle_recording2(bool hold, int button, bool player, gd::PlayLayer* self) {
    replay2.push_back({get_frame(self), hold, button, player});
}

void ReplayEngine::handle_playing(gd::PlayLayer* self)
{
    unsigned frame = get_frame(self);

    if (accuracy_fix) {
        while (index < (int)replay.size() && frame >= replay[index].frame)
        {
            if (replay[index].player)
            {
                self->m_pPlayer1()->m_position().x = replay[index].x;
                self->m_pPlayer1()->m_position().y = replay[index].y;
                if (rotation_fix)
                     self->m_pPlayer1()->setRotation(replay[index].rotation);
                self->m_pPlayer1()->m_yAccel() = replay[index].y_accel;
            }
            else
            {
                self->m_pPlayer2()->m_position().x = replay[index].x;
                self->m_pPlayer2()->m_position().y = replay[index].y;
                if (rotation_fix)
                     self->m_pPlayer2()->setRotation(replay[index].rotation);
                self->m_pPlayer2()->m_yAccel() = replay[index].y_accel;
            }
            index++;
        }
    }


    while (index2 < (int)replay2.size() && frame >= replay2[index2].frame)
    {
        hooks::GJBaseGameLayer_HandleButton(self, replay2[index2].hold, replay2[index2].button, replay2[index2].player);
        cps_counter.recordClick(); // pointercreate задвайте вопросы
        index2++;
    }
}

void ReplayEngine::handle_reseting(gd::PlayLayer* self) {
    int lastCheckpointFrame = get_frame(self);

    if (mode == state::record) {
        remove_actions(lastCheckpointFrame);

        if (!replay2.empty() && replay2.back().hold) {
            handle_recording2(false, replay2.back().button, replay2.back().player, self);
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

    file.write(reinterpret_cast<char *>(&hacks::fps_value), sizeof(hacks::fps_value));

    unsigned replay_size = (unsigned int)replay.size();
    unsigned replay2_size = (unsigned int)replay2.size();

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

    file.read(reinterpret_cast<char *>(&hacks::fps_value), sizeof(hacks::fps_value));

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
    ImGui::BeginChild("Select Replay##3", {400 * ImGuiHook::scale, 300 * ImGuiHook::scale});
    for (int i = 0; i < (int)replay_list.size(); i++)
    {
        if (ImGui::Button(replay_list[i].c_str(), {ImGui::GetContentRegionAvail().x, NULL}))
        {
            replay_name = replay_list[i];
            ImGui::CloseCurrentPopup();
        }
    }
    ImGui::EndChild();

    ImGui::Separator();
    
    if (ImGui::Button("Close", {ImGui::GetContentRegionAvail().x, NULL})) {
        ImGui::CloseCurrentPopup();
    }
}

void ReplayEngine::render() {
    if (ImGui::BeginPopup("Select Replay", ImGuiWindowFlags_NoResize)) {
        openReplayMultishit();
        ImGui::EndPopup();
    }

    int mode_ = (int)mode;

    if (ImGui::RadioButton("Disable", &mode_, 0, ImGuiHook::scale))
        mode = state::disable;
    ImGui::SameLine();

    if (ImGui::RadioButton("Record", &mode_, 1, ImGuiHook::scale))
    {
        if (hacks::fps_enabled) {
            if (mode != state::record) {
                replay.clear();
                replay2.clear();
            }
            mode = state::record;
        }
        else {
            log = "Enable the FPS multiplier before recording";
            mode = state::disable;
        }
    }
    
    ImGui::SameLine();

    if (ImGui::RadioButton("Play", &mode_, 2, ImGuiHook::scale)) {
        if (hacks::fps_enabled) {
            mode = state::play;
        }
        else {
            log = "Enable the FPS multiplier before playing";
            mode = state::disable;
        }
    }  

    ImGui::Separator();   

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 35 * ImGuiHook::scale);
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
        log = "The replay has been cleared";
    }
    ImGui::SameLine();

    ImGui::Separator();  

    ImGui::Text("Replay Size: %i", replay2.size());
    ImGui::Text("Frame: 0");

    ImGui::Separator();

    ImGui::Text(log.c_str());
}