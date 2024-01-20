#include "replayEngine.hpp"
#include <fstream>
#include <sstream>

ReplayEngine engine;

std::string ReplayEngine::save(char *name) {
    auto isEmpty = [](char *str) {return (str != nullptr && str[0] == '\0');};
    if (replay.empty())
    {
        return "Replay doens't have actions";
    }
    else
    {
        if (isEmpty(name))
        {
            return "Replay name is empty";
        }
        else
        {
            std::ofstream file("GDH/macros/" + (std::string)name + ".txt");

            if (file.is_open())
            {
                file << fps << "\n";
                for (auto &action : replay)
                {
                    file << action.frame << " " << action.hold << " " << action.player_button << " " << action.player << "\n";
                }
                file.close();
                return "Replay saved";
            }
        }
    }
    return "NOT???";
}

std::string ReplayEngine::load(char *name) {
    auto isEmpty = [](char *str) {return (str != nullptr && str[0] == '\0');};
    if (!replay.empty())
    {
        return "Please clear the replay before loading another";
    }
    else
    {
        if (isEmpty(name))
        {
            return "The replay name is empty";
        }
        else
        {
            std::ifstream file("GDH/macros/" + (std::string)name + ".txt");
            std::string line;

            if (file.is_open())
            {
                std::getline(file, line);
                fps = stof(line);
                while (std::getline(file, line))
                {
                    std::istringstream iss(line);
                    int value1, value2, value3, value4;

                    if (iss >> value1 >> value2 >> value3 >> value4)
                    {
                        replay.push_back({(unsigned)value1, (bool)value2, (int)value3, (bool)value4});
                    }
                }
                file.close();
                return "The replay has been loaded";
            }
            else
            {
                return "Replay doesn't exist";
            }
        }
    }
    return "NOT???";
}

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
        return static_cast<unsigned>(time * fps);
    }       

    return 0;
}