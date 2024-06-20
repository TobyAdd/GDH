#include "replayEngine.hpp"
#include "hacks.hpp"
#include "hooks.hpp"
#include <fstream>
#include "gui.hpp"
#include "labels.hpp"
#include <random>
#include "recorder.hpp"

ReplayEngine engine;
SpamBot spamBot;
StraightFly straightFly;
Sequence sequence;

unsigned ReplayEngine::get_frame(gd::PlayLayer* self) {
    if (self == nullptr)
        return 0;
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

bool inited = false;
const char *items[] = {"General", "Assist", "Editor", "Recorder (WIP)", "Sequence", "About", "Exit"};
int items_index = 0;

void ReplayEngine::renderFull() {
    if (!inited) {
        inited = true;
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }

    ImGui::Begin("Replay Engine Settings", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::BeginChild("##leftside", ImVec2(150 * ImGuiHook::scale, 350 * ImGuiHook::scale));
    ImGui::Text("Replay Engine");
    ImGui::Separator();
    ImGui::Spacing();
    for (int i = 0; i < IM_ARRAYSIZE(items); i++)
    {
        bool is_selected = (items_index == i);
        if (ImGui::Selectable(items[i], is_selected))
            items_index = i;
    }
    ImGui::EndChild();

    ImGui::SameLine(0, 5);
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine(0, 5);

    ImGui::BeginChild("##rightside", ImVec2(450 * ImGuiHook::scale, 350 * ImGuiHook::scale));
    if (items_index == 0) {
        if (ImGui::BeginPopup("Select Replay2")) {
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
                log = "Enable the TPS Bypass before recording";
                mode = state::disable;
            }
        }
        
        ImGui::SameLine();

        if (ImGui::RadioButton("Play", &mode_, 2, ImGuiHook::scale)) {
            if (hacks::fps_enabled) {
                mode = state::play;
            }
            else {
                log = "Enable the TPS Bypass before playing";
                mode = state::disable;
            }
        }  

        ImGui::SameLine();

        if (ImGui::RadioButton("Continue", &mode_, 3, ImGuiHook::scale))
            mode = state::continue_record;
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("This mode will allow you to continue recording level. First, you need to load unfinished replay and switch to the \"Continue\" mode\nAfter the next attempt, the mode will switch to \"Play\", and when the level reaches the frame that is the last frame of the replay, the mode will switch to \"Record\"\nPractice mode is activated automatically after switching to the \"Continue\" recording mode. Additionally, you can place checkpoints");


        ImGui::Separator();   

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 35 * ImGuiHook::scale);
        ImGui::InputText("##replay_name", &replay_name);

        ImGui::SameLine();

        if (ImGui::ArrowButton("##replay_select", ImGuiDir_Down)) {
            openSelectReplay("Select Replay2");
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

        ImGui::Separator();  

        ImGui::Checkbox("Accuracy Fix", &accuracy_fix, ImGuiHook::scale);

        ImGui::SameLine();

        ImGui::Checkbox("Rotation Fix (Avoid)", &rotation_fix, ImGuiHook::scale);

        ImGui::Separator();  

        ImGui::Checkbox("Real Time", &real_time, ImGuiHook::scale);

        ImGui::SameLine();

        ImGui::Checkbox("Ignore Inputs on Playing", &ignore_inputs, ImGuiHook::scale);

        ImGui::Separator();  

        ImGui::Text("Replay Size: %i", replay2.size());
        ImGui::Text("Frame: %i", get_frame(hooks::pl));
    }
    else if (items_index == 1) {
        ImGui::Checkbox("Frame Advance", &frame_advance, ImGuiHook::scale);
        
        ImGui::Separator();

        if (ImGui::Checkbox("Spambot", &spamBot.enabled, ImGuiHook::scale))
        {
            spamBot.reset_temp();
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2);
        ImGui::DragInt("##spamhold", &spamBot.hold, 1, 1, INT_MAX, "Hold: %i");

        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragInt("##spamrelease", &spamBot.release, 1, 1, INT_MAX, "Release: %i");

        ImGui::Checkbox("Player 1", &spamBot.player_p1, ImGuiHook::scale);
        ImGui::SameLine();
        ImGui::Checkbox("Player 2", &spamBot.player_p2, ImGuiHook::scale);

        ImGui::Separator();

        ImGui::Checkbox("Dual Clicks", &engine.dualClicks, ImGuiHook::scale);

        ImGui::Separator();

        // if (ImGui::Checkbox("Straight Fly Bot", &straightFly.enabled, ImGuiHook::scale))
        // {
        //     auto pl = gd::GameManager::sharedState()->getPlayLayer();
        //     straightFly.start(pl);
        // }

        // ImGui::SameLine();
        // ImGui::DragInt("##StraightFlyAcc", &straightFly.accuracy, 1, 0, 100, "Y Accuracy: %i");
        // ImGui::Text("Note: Straight Fly Bot works only on first player");
    }
    else if (items_index == 2) {
        static std::string physics;
        static std::string inputs;

        if (ImGui::BeginTabBar("Editor")) {
            if (ImGui::BeginTabItem("Physics")) {
                if (ImGui::Button("Refresh Physics Frames", {ImGui::GetContentRegionAvail().x / 2, NULL})) {
                    if (!engine.replay2.empty()) {
                        physics = "";
                        for (size_t i = 0; i < engine.replay.size(); i++) {
                            physics += std::to_string(engine.replay[i].frame) + " " + std::to_string(engine.replay[i].x) + " " +
                            std::to_string(engine.replay[i].y) + " " + std::to_string(engine.replay[i].y_accel) + " "
                                + std::to_string(engine.replay[i].player);
                            if (i != engine.replay.size() - 1)
                                physics += "\n"; 
                        }
                        log = "Actions loaded (<frame> <x position> <y position> <rotation> <y_accel> <player>)";
                    }
                    else {
                        log = "Replay doesn't have actions";
                    }
                }

                ImGui::SameLine();

                if (ImGui::Button("Apply Phisics Frames", {ImGui::GetContentRegionAvail().x, NULL})) {
                    replay.clear();
                    std::istringstream iss(physics);
                    std::string line;

                    while (std::getline(iss, line))
                    {
                        std::istringstream lineStream(line);
                        replay_data data;
                        lineStream >> data.frame >> data.x >> data.y >> data.rotation >> data.y_accel >> data.player;
                        replay.push_back(data);
                    }
                    log = "Actions saved";
                }

                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
                ImGui::InputTextMultiline("##PhysicsData", &physics, {-1, -1});
                ImGui::PopStyleVar();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Inputs")) {                    
                if (ImGui::Button("Refresh Inputs Frames", {ImGui::GetContentRegionAvail().x / 2, NULL})) {
                    if (!engine.replay2.empty()) {
                        inputs = "";
                        for (size_t i = 0; i < engine.replay2.size(); i++) {
                            inputs += std::to_string(engine.replay2[i].frame) + " " + std::to_string(engine.replay2[i].hold) + " " +
                            std::to_string(engine.replay2[i].button) + " " + std::to_string(engine.replay[i].player);
                            if (i != engine.replay2.size() - 1)
                                inputs += "\n"; 
                        }
                        log = "Actions loaded (<frame> <hold> <button> <player>)";
                    }
                    else {
                        log = "Replay doesn't have actions";
                    }
                }

                ImGui::SameLine();

                if (ImGui::Button("Apply Inputs Frames", {ImGui::GetContentRegionAvail().x, NULL})) {
                    replay2.clear();
                    std::istringstream iss(inputs);
                    std::string line;

                    while (std::getline(iss, line))
                    {
                        std::istringstream lineStream(line);
                        replay_data2 data;
                        lineStream >> data.frame >> data.hold >> data.button >> data.player;
                        replay2.push_back(data);
                    }
                    log = "Actions saved";
                }

                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6);
                ImGui::InputTextMultiline("##InputsData", &inputs, {-1, -1});
                ImGui::PopStyleVar();

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    else if (items_index == 3) {
        if (ImGui::BeginTabBar("Recorder")) {
            if (ImGui::BeginTabItem("Video")) {

                if (ImGui::Checkbox("Record", &recorder.enabled, ImGuiHook::scale)) {
                    if (recorder.enabled)
                        recorder.start();
                    else 
                        recorder.stop();
                }

                ImGui::SameLine();

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::InputText("##videoname", &recorder.video_name);

                ImGui::SameLine();

                ImGui::Spacing();

                ImGui::Text("Resolution:");
                ImGui::Separator();

                ImGui::PushItemWidth(45.f * ImGuiHook::scale);
                ImGui::InputInt("##width", &recorder.width, 0);
                ImGui::SameLine(0, 5);

                ImGui::Text("x");
                ImGui::SameLine(0, 5);

                ImGui::PushItemWidth(45.f * ImGuiHook::scale);
                ImGui::InputInt("##height", &recorder.height, 0);
                ImGui::SameLine(0, 5);

                ImGui::Text("@");
                ImGui::SameLine(0, 5);

                ImGui::PushItemWidth(35.f * ImGuiHook::scale);
                ImGui::InputInt("##fps", &recorder.fps, 0);

                ImGui::Spacing();

                ImGui::Text("Encoding Settings");
                ImGui::Separator(); 
                
                ImGui::PushItemWidth(50.f * ImGuiHook::scale);
                ImGui::InputText("Bitrate", &recorder.bitrate);

                ImGui::SameLine();

                ImGui::PushItemWidth(80.f * ImGuiHook::scale);
                ImGui::InputText("Codec", &recorder.codec);

                ImGui::PushItemWidth(250 * ImGuiHook::scale);
                ImGui::InputText("Extra Arguments", &recorder.extra_args);
                
                ImGui::Spacing();

                ImGui::Text("Level Settings");
                ImGui::Separator();

                ImGui::PushItemWidth(200 * ImGuiHook::scale);
                ImGui::InputFloat("Second to Render After", &recorder.after_end_duration, 1);

                ImGui::Spacing();

                ImGui::Text("Presets");
                ImGui::Separator();

                if (ImGui::Button("HD"))
                {
                    recorder.width = 1280;
                    recorder.height = 720;
                    recorder.fps = 60;
                    recorder.bitrate = "30M";
                }

                ImGui::SameLine();

                if (ImGui::Button("FULL HD"))
                {
                    recorder.width = 1920;
                    recorder.height = 1080;
                    recorder.fps = 60;
                    recorder.bitrate = "50M";
                }

                ImGui::SameLine();

                if (ImGui::Button("4K"))
                {
                    recorder.width = 3840;
                    recorder.height = 2160;
                    recorder.fps = 60;
                    recorder.bitrate = "70M";
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Audio")) {
                if (ImGui::Checkbox("Record Buffer", &recorderAudio.enabled, ImGuiHook::scale)) {
                    if (!recorderAudio.showcase_mode) {
                        if (recorderAudio.enabled)
                            recorderAudio.start();
                        else 
                            recorderAudio.stop();
                    }
                }

                ImGui::Checkbox("Showcase Mode", &recorderAudio.showcase_mode, ImGuiHook::scale);

                ImGui::SameLine();

                //ImGui::Checkbox("Disable Objects Render", &hacks::disable_render, ImGuiHook::scale);

                if (recorderAudio.showcase_mode) {
                    ImGui::Spacing();

                    ImGui::Text("Level Settings");
                    ImGui::Separator();

                
                    ImGui::PushItemWidth(200 * ImGuiHook::scale);
                    ImGui::InputFloat("Second to Render After##2", &recorderAudio.after_end_duration, 1);
                }

                ImGui::Spacing();

                ImGui::Text("Save Buffer");
                ImGui::Separator();

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::InputText("##videoname", &recorderAudio.audio_name);

                ImGui::PushItemWidth(250 * ImGuiHook::scale);
                ImGui::InputText("Extra Arguments", &recorderAudio.extra_args);

                if (ImGui::Button("Save", {ImGui::GetContentRegionAvail().x, NULL})) {
                    std::string audio_name_ = recorderAudio.audio_name;
                    std::string command = "ffmpeg.exe -y -i fmodoutput.wav ";
                    if (!recorderAudio.extra_args.empty()) {
                        command += recorderAudio.extra_args + " ";
                    }

                    command += "\"" + audio_name_ + "\"";
                    auto process = subprocess::Popen(command);
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Merge")) {
                static bool shortest = true;
                static std::vector<std::string> videos;
                static std::vector<std::string> audios;
                static int index_videos = 0;
                static int index_audios = 0;

                ImGui::BeginChild("##VideoSelect", {NULL, 130 * ImGuiHook::scale}, true);
                for (size_t i = 0; i < videos.size(); i++) {
                    bool is_selected = (index_videos == i);
                    if (ImGui::Selectable(videos[i].c_str(), is_selected)) {
                        index_videos = i;
                    }
                }
                ImGui::EndChild();

                ImGui::BeginChild("##AudioSelect", {NULL, 130 * ImGuiHook::scale}, true);
                for (size_t i = 0; i < audios.size(); i++) {
                    bool is_selected = (index_audios == i);
                    if (ImGui::Selectable(audios[i].c_str(), is_selected)) {
                        index_audios = i;
                    }
                }
                ImGui::EndChild();

                if (ImGui::Button("Refresh", {ImGui::GetContentRegionAvail().x, NULL})) {
                    videos.clear();
                    audios.clear();
                    for (const auto &entry : std::filesystem::directory_iterator(std::filesystem::current_path())) {
                        if (entry.path().extension() == ".mp4") {
                            videos.push_back(entry.path().string());
                        }

                        if (entry.path().extension() == ".mp3") {
                            audios.push_back(entry.path().string());
                        }
                    }
                }

                ImGui::Checkbox("Shortest", &shortest, ImGuiHook::scale);

                
                ImGui::SameLine();

                if (ImGui::Button("Merge", {ImGui::GetContentRegionAvail().x, NULL})) {
                    if (videos.empty() || audios.empty()) {
                        log = "Please select both a video and an audio file";
                    } else if (index_videos >= 0 && index_videos < (int)videos.size() && index_audios >= 0 && index_audios < (int)audios.size()) {
                        std::string command2 = "ffmpeg.exe -y -i \"" + videos[index_videos] + "\" -i \"" + audios[index_audios] + "\" -map 0:v -map 1:a -c:v copy ";
                        if (shortest) {
                            command2 += "-shortest ";
                        }
                        std::filesystem::path video_rel(videos[index_videos]);
                        command2 += "\"audio_" + video_rel.filename().string() + "\"";
                        Console::WriteLn(command2);
                        auto process = subprocess::Popen(command2);
                    }
                }


                ImGui::EndTabItem();
            }


            ImGui::EndTabBar();
        }
    }
    else if (items_index == 4) {
        ImGui::Checkbox("Toggle", &sequence.enable_sqp, ImGuiHook::scale);
        ImGui::SameLine();
        ImGui::Checkbox("Random", &sequence.random_sqp, ImGuiHook::scale);
        ImGui::SameLine();
        int mode = (int)engine.mode;
        if (ImGui::RadioButton("Play", &mode, 2, ImGuiHook::scale))
            engine.mode = state::play;
        
        ImGui::SetNextItemWidth(200.f * ImGuiHook::scale);
        ImGui::InputText("##sqp_replay", &sequence.replay_sq_name);
        ImGui::SameLine();
        if (ImGui::Button("Add"))
        {
            auto path = hacks::folderMacroPath / std::string(sequence.replay_sq_name + ".re");
            if (std::filesystem::exists(path))
            {
                sequence.replays.push_back(sequence.replay_sq_name);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Remove"))
        {
            if (sequence.replays.size() > (size_t)sequence.current_idx)
            {
                sequence.replays.erase(sequence.replays.begin() + sequence.current_idx);
                sequence.first_sqp = true;
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Remove All"))
        {
            sequence.replays.clear();
            sequence.first_sqp = true;
        }

        ImGui::BeginChild("##sequence_window", {NULL, NULL}, true);
        for (size_t n = 0; n < sequence.replays.size(); n++)
        {
            bool is_selected = (sequence.current_idx == n);
            std::string anticonflict = sequence.replays[n] + "##" + std::to_string(n);
            if (ImGui::Selectable(anticonflict.c_str(), is_selected))
                sequence.current_idx = n;
        }
        ImGui::EndChild();
    }
    else if (items_index == 5)
    {
        ImGui::Text("About");
        ImGui::Separator();
        ImGui::Text("Replay Engine by TobyAdd");
        if (ImGui::MenuItem("Graphical interface is made using ImGui"))
            ShellExecuteA(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);

        ImGui::Spacing();
        ImGui::Text("Keybinds");
        ImGui::Separator();
        ImGui::Text("C - Enable Frame Advance + Next Frame");
        ImGui::Text("V - Disable Frame Advance");
        ImGui::Text("P - Toggle Playback/Disable");
    }
    else if (items_index == 6)
    {
        items_index = 0;
        gui::onlyRE = false;
        inited = false;
    }

    ImGui::EndChild();

    ImGui::Text(log.c_str());
    ImGui::End();
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
            log = "Enable the TPS Bypass before recording";
            mode = state::disable;
        }
    }
    
    ImGui::SameLine();

    if (ImGui::RadioButton("Play", &mode_, 2, ImGuiHook::scale)) {
        if (hacks::fps_enabled) {
            mode = state::play;
        }
        else {
            log = "Enable the TPS Bypass before playing";
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
    ImGui::Text("Frame: %i", get_frame(hooks::pl));

    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImColor(64, 64, 64).Value);
    if (ImGui::MenuItem(log.c_str())) {
        gui::onlyRE = true;
    }
    ImGui::PopStyleColor();
}

bool SpamBot::next_frame()
{
    if ((downed && ++release_current >= release) || (!downed && ++hold_current >= hold))
    {
        downed = !downed;
        (downed) ? release_current = 0 : hold_current = 0;
        return true;
    }
    else
    {
        return false;
    }
}

void SpamBot::handle_spambot(gd::PlayLayer *self)
{
    if (enabled && next_frame())
    {
        if (player_p1) {
            hooks::GJBaseGameLayer_HandleButton_H(self, downed, 1, true);
        }

        if (player_p2) {
            hooks::GJBaseGameLayer_HandleButton_H(self, downed, 1, false);
        }
        
    }
}

void SpamBot::reset_temp()
{
    hold_current = hold - 1;
    release_current = 0;
}

void StraightFly::handle_straightfly(gd::PlayLayer *self)
{
    if (!enabled)
        return;

    // float y = self->m_pPlayer1()->m_position().y;
    // double accel = self->m_pPlayer1()->m_yAccel();
    // bool holding = self->m_pPlayer1()->m_isHolding();

    // if (start_y == 0)
    // {
    //     start(self);
    // }

    // if (self->m_pPlayer1()->m_isUpsideDown())
    // {
    //     float delta_y = y - start_y;
    //     y = start_y - delta_y;
    //     accel = -accel;
    // }

    // if (accel < 0 && y < start_y - accel - accuracy / 100 && !holding)
    // {
    //     hooks::GJBaseGameLayer_HandleButton_H(self, true, 1, true);
    // }
    // else if (accel > 0 && y > start_y - accel + accuracy / 100 && holding)
    // {
    //     hooks::GJBaseGameLayer_HandleButton_H(self, false, 1, true);
    // }
}

void StraightFly::start(gd::PlayLayer *self)
{
    start_y = self ? self->m_pPlayer1()->m_position().y : 0.0f;
}

int getRandomNumber(int min, int max)
{
    std::random_device rd;
    std::mt19937_64 generator(rd());

    std::uniform_int_distribution<int> distribution(min, max);

    int randomNum = distribution(generator);
    return randomNum;
}

void Sequence::do_some_magic()
{
    if (sequence.enable_sqp && engine.mode == state::play && !sequence.replays.empty())
    {
        engine.replay.clear();
        engine.replay2.clear();

        if (!sequence.random_sqp)
        {          
            if (sequence.first_sqp)
            {
                sequence.current_idx = 0;
                sequence.first_sqp = false;
            }
            else
                sequence.current_idx++;
            if ((int)sequence.replays.size() <= sequence.current_idx)
                sequence.current_idx = 0;
            engine.load(sequence.replays[sequence.current_idx]);
        }
        else
        {
            sequence.current_idx = getRandomNumber(0, (int)sequence.replays.size() - 1);
            engine.load(sequence.replays[sequence.current_idx]);
        }
    }
}