#include <fstream>
#include <random>
#include <sstream>

#include "replayEngine.hpp"
#include "gui.hpp"

ReplayEngine engine;

void ReplayEngine::handle_action(void* self, bool hold, int player_button, bool player) {
    unsigned frame = getFrame(self);
    
    replay.push_back({
        frame,
        hold,
        player_button,
        player
    });
}

unsigned ReplayEngine::getFrame(void* self) {
    if (self) {
        auto time = *(double*)(((char*)self) + 0x328);
        return static_cast<unsigned>(time * engine.fps);
    }       

    return 0;
}

inline int ReplayEngine::random(int min, int max)
{
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(generator);
}

void ReplayEngine::save(std::string replay_name) {
    if (replay.empty())
    {
        ImGui::InsertNotification({ ImGuiToastType_Error, 5000, "Replay is empty!" });
    }
    else
    {
        if (replay_name.empty())
        {
            ImGui::InsertNotification({ ImGuiToastType_Error, 5000, "Replay name is empty!" });
        }
        else
        {
            std::ofstream file("GDH/replays/" + replay_name + ".txt");

            if (file.is_open())
            {
                file << fps << "\n";

                for (auto &action : replay) {
                    file << action.frame << " " << action.hold << " " << action.player_button << " " << action.player << "\n";
                }

                file.close();
                gui::ReplaysList();
                ImGui::InsertNotification({ ImGuiToastType_Success, 5000, "Replay: \"%s\" successfuly saved!", replay_name.c_str() });
            }
        }
    }
}

void ReplayEngine::load(std::string replay_name) {
    if (!replay.empty())
    {
        ImGui::InsertNotification({ ImGuiToastType_Error, 5000, "Please clear replay before loading another!" });
    }
    else
    {
        if (replay_name.empty())
        {
            ImGui::InsertNotification({ ImGuiToastType_Error, 5000, "Replay name is empty!" });
        }
        else
        {
            std::ifstream replay_file("GDH/replays/" + replay_name + ".txt");
            std::string line;

            if (replay_file.is_open())
            {
                std::getline(replay_file, line);
                engine.fps = stof(line);

                while (std::getline(replay_file, line))
                {
                    std::istringstream iss(line);
                    int frame, hold, player_button, player;

                    if (iss >> frame >> hold >> player_button >> player)
                    {
                        replay.push_back({(unsigned)frame, (bool)hold, (int)player_button, (bool)player});
                    }
                }

                replay_file.close();
                gui::ReplaysList();
                ImGui::InsertNotification({ ImGuiToastType_Success, 5000, "Replay: \"%s\" - Successfuly loaded!", replay_name.c_str() });
            }
            else
            {
                ImGui::InsertNotification({ ImGuiToastType_Error, 5000, "Replay doens't exists!" });
            }
        }
    }
}

void ReplayEngine::sequence_work()
{
    if (sequence_state && mode == state::play && !sequence.empty())
    {
        replay.clear();

        if (!sequence_random)
        {
            if (sequence_first)
            {
                sequence_index = 0;
                sequence_first = false;
            }
            else
                sequence_index++;

            if ((int)sequence.size() <= sequence_index)
                sequence_index = 0;
            
            engine.load(sequence[sequence_index]);
        }
        else
        {
            sequence_index = engine.random(0, (int)sequence.size() - 1);
            engine.load(sequence[sequence_index]);
        }
    }
}