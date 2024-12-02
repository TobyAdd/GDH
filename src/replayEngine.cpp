#include "replayEngine.hpp"
#include "config.hpp"

unsigned ReplayEngine::get_frame() {
    auto &config = Config::get();

    auto gjbgl = GJBaseGameLayer::get();
    if (gjbgl)
        return static_cast<unsigned>(gjbgl->m_gameState.m_levelTime * config.get<float>("tps_value", 240.f));
    return 0;
}

void ReplayEngine::remove_actions(unsigned frame) {
    auto check_physics = [&](replay_data &action) -> bool
    {
        return action.frame > frame;
    };
    m_physicFrames_p1.erase(remove_if(m_physicFrames_p1.begin(), m_physicFrames_p1.end(), check_physics), m_physicFrames_p1.end());
    m_physicFrames_p2.erase(remove_if(m_physicFrames_p1.begin(), m_physicFrames_p1.end(), check_physics), m_physicFrames_p1.end());

    auto check_inputs = [&](replay_data2 &action) -> bool { return action.frame >= frame; };
    m_inputFrames_p1.erase(remove_if(m_inputFrames_p1.begin(), m_inputFrames_p1.end(), check_inputs), m_inputFrames_p1.end());
    m_inputFrames_p2.erase(remove_if(m_inputFrames_p2.begin(), m_inputFrames_p2.end(), check_inputs), m_inputFrames_p2.end());
}

size_t ReplayEngine::get_actions_size() {
    return m_inputFrames_p1.size() + m_inputFrames_p2.size();
}

std::string ReplayEngine::save(std::string name) {
    auto& config = Config::get();

    std::ofstream file(folderMacroPath / std::string(name + ".re3"), std::ios::binary);    

    float tps_value = config.get("tps_value", 240.f);
    file.write(reinterpret_cast<char *>(&tps_value), sizeof(tps_value));

    unsigned p1_size = m_physicFrames_p1.size();
    unsigned p2_size = m_physicFrames_p2.size();
    unsigned p1_input_size = m_inputFrames_p1.size();
    unsigned p2_input_size = m_inputFrames_p2.size();

    file.write(reinterpret_cast<char *>(&p1_size), sizeof(p1_size));
    file.write(reinterpret_cast<char *>(&p2_size), sizeof(p2_size));
    file.write(reinterpret_cast<char *>(&p1_input_size), sizeof(p1_input_size));
    file.write(reinterpret_cast<char *>(&p2_input_size), sizeof(p2_input_size));

    file.write(reinterpret_cast<char *>(&m_physicFrames_p1[0]), sizeof(replay_data) * p1_size);    
    file.write(reinterpret_cast<char *>(&m_physicFrames_p2[0]), sizeof(replay_data) * p2_size);    
    file.write(reinterpret_cast<char *>(&m_inputFrames_p1[0]), sizeof(replay_data2) * p1_input_size);    
    file.write(reinterpret_cast<char *>(&m_inputFrames_p2[0]), sizeof(replay_data2) * p2_input_size);

    file.close();
    return "Replay saved";
}

std::string ReplayEngine::load(std::string name) {
    auto& config = Config::get();

    if (!m_inputFrames_p1.empty() || !m_inputFrames_p2.empty())
        return "Please clear replay before loading another";

    std::ifstream file(folderMacroPath / std::string(name + ".re3"), std::ios::binary);
    if (!file)
        return "Replay doesn't exist";


    float tps_value;
    file.read(reinterpret_cast<char *>(&tps_value), sizeof(tps_value));
    config.set<float>("tps_value", tps_value);

    unsigned p1_size;
    unsigned p2_size;
    unsigned p1_input_size;
    unsigned p2_input_size;

    file.read(reinterpret_cast<char *>(&p1_size), sizeof(p1_size));
    file.read(reinterpret_cast<char *>(&p2_size), sizeof(p2_size));
    file.read(reinterpret_cast<char *>(&p1_input_size), sizeof(p1_input_size));
    file.read(reinterpret_cast<char *>(&p2_input_size), sizeof(p2_input_size));

    m_physicFrames_p1.resize(p1_size);
    m_physicFrames_p2.resize(p2_size);
    m_inputFrames_p1.resize(p1_input_size);
    m_inputFrames_p2.resize(p2_input_size);

    file.read(reinterpret_cast<char *>(&m_physicFrames_p1[0]), sizeof(replay_data) * p1_size);
    file.read(reinterpret_cast<char *>(&m_physicFrames_p2[0]), sizeof(replay_data) * p2_size);
    file.read(reinterpret_cast<char *>(&m_inputFrames_p1[0]), sizeof(replay_data2) * p1_input_size);
    file.read(reinterpret_cast<char *>(&m_inputFrames_p2[0]), sizeof(replay_data2) * p2_input_size);

    file.close();

    return "Replay loaded";
}

std::string ReplayEngine::clear() {
    m_physicFrames_p1.clear();
    m_physicFrames_p2.clear();
    m_inputFrames_p1.clear();
    m_inputFrames_p2.clear();

    return "Replay has been cleared";
}

void ReplayEngine::handle_update(GJBaseGameLayer* self) {
    unsigned frame = get_frame();

    if (mode == state::record) {
            bool frameExist_p1 = std::find_if(m_physicFrames_p1.begin(), m_physicFrames_p1.end(), [&](const auto &data)
                                    { return data.frame == frame; }) != m_physicFrames_p1.end();

            if (!frameExist_p1)
                m_physicFrames_p1.push_back({
                    frame, 
                    self->m_player1->m_position.x,
                    self->m_player1->m_position.y,
                    self->m_player1->getRotation(),
                    self->m_player1->m_yVelocity,
                    true //first player
                });

            if (!self->m_gameState.m_isDualMode)
                return;

            bool frameExist_p2 = std::find_if(m_physicFrames_p2.begin(), m_physicFrames_p2.end(), [&](const auto &data)
                { return data.frame == frame; }) != m_physicFrames_p2.end();

            if (frameExist_p2)
                m_physicFrames_p2.push_back({
                    frame, 
                    self->m_player2->m_position.x,
                    self->m_player2->m_position.y,
                    self->m_player2->getRotation(),
                    self->m_player2->m_yVelocity,
                    false //second player
                });
    }
    else if (mode == state::play) {
        if (accuracy_fix) {
            while (m_physicIndex_p1 < m_physicFrames_p1.size() && frame >= m_physicFrames_p1[m_physicIndex_p1].frame)
            {
                self->m_player1->m_position.x = m_physicFrames_p1[m_physicIndex_p1].x;
                self->m_player1->m_position.y = m_physicFrames_p1[m_physicIndex_p1].y;
                if (rotation_fix)
                    self->m_player1->setRotation(m_physicFrames_p1[m_physicIndex_p1].rotation);
                self->m_player1->m_yVelocity = m_physicFrames_p1[m_physicIndex_p1].y_accel;
                m_physicIndex_p1++;
            }

            while (m_physicIndex_p2 < m_physicFrames_p2.size() && frame >= m_physicFrames_p2[m_physicIndex_p2].frame)
            {
                self->m_player1->m_position.x = m_physicFrames_p2[m_physicIndex_p2].x;
                self->m_player1->m_position.y = m_physicFrames_p2[m_physicIndex_p2].y;
                if (rotation_fix)
                    self->m_player1->setRotation(m_physicFrames_p2[m_physicIndex_p2].rotation);
                self->m_player1->m_yVelocity = m_physicFrames_p2[m_physicIndex_p2].y_accel;
                m_physicIndex_p2++;
            }
        }


        while (m_inputIndex_p1 < m_inputFrames_p1.size() && frame >= m_inputFrames_p1[m_inputIndex_p1].frame)
        {
            self->handleButton(m_inputFrames_p1[m_inputIndex_p1].down, m_inputFrames_p1[m_inputIndex_p1].button, m_inputFrames_p1[m_inputIndex_p1].isPlayer1);
            m_inputIndex_p1++; 
        }

        while (m_inputIndex_p2 < m_inputFrames_p2.size() && frame >= m_inputFrames_p2[m_inputIndex_p2].frame)
        {
            self->handleButton(m_inputFrames_p2[m_inputIndex_p2].down, m_inputFrames_p2[m_inputIndex_p2].button, m_inputFrames_p2[m_inputIndex_p2].isPlayer1);
            m_inputIndex_p2++; 
        }
    }
}

void ReplayEngine::handle_reset() {
    if (mode == state::record) {
        int lastCheckpointFrame = get_frame();
        remove_actions(lastCheckpointFrame);
        auto_button_release();
    }
    else if (mode == state::play) {
        m_physicIndex_p1 = 0;
        m_physicIndex_p2 = 0;
        m_inputIndex_p1 = 0;
        m_inputIndex_p2 = 0;
    }
}

void ReplayEngine::handle_button(bool down, int button, bool isPlayer1) {
    if (mode != state::record)
        return;

    unsigned frame = get_frame();
    if (isPlayer1) {
        if (!m_inputFrames_p1.empty() && down && !m_inputFrames_p1.back().down && m_inputFrames_p1.back().frame == frame)
            m_inputFrames_p1.pop_back();

        m_inputFrames_p1.push_back({get_frame(), down, button, isPlayer1});
    }        
    else {
        if (!m_inputFrames_p2.empty() && down && !m_inputFrames_p2.back().down && m_inputFrames_p2.back().frame == frame)
            m_inputFrames_p2.pop_back();

        m_inputFrames_p2.push_back({get_frame(), down, button, isPlayer1});
    }
}

void ReplayEngine::auto_button_release() {
    if (mode == state::record) {
        if (!m_inputFrames_p1.empty() && m_inputFrames_p1.back().down)
            handle_button(false, m_inputFrames_p1.back().button, m_inputFrames_p1.back().isPlayer1);

        if (!m_inputFrames_p2.empty() && m_inputFrames_p2.back().down)
            handle_button(false, m_inputFrames_p2.back().button, m_inputFrames_p2.back().isPlayer1);
    }
}