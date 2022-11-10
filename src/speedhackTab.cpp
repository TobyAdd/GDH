#include "speedhackTab.h"
#include "SpeedhackAudio.h"

float speedhackTab::speedhack = 1.f;
vector<string> speedhackTab::speedhackData;
bool speedhackTab::audio = false;
bool speedhackTab::speedhackInit = false;

namespace speedhackTab {
    void speedhackTab::init() {
        string line;
        fstream fileConfig("GDH\\speedhack.data", std::ios::in);
        if (fileConfig.is_open()) {
            getline(fileConfig, line);
            speedhack = stof(line);
            getline(fileConfig, line);
            audio = uselessShit::stringToBool(line);
            CCDirector::sharedDirector()->getScheduler()->setTimeScale(speedhack);
            SpeedhackAudio::set(CCDirector::sharedDirector()->getScheduler()->getTimeScale());
        }

    }

    void speedhackTab::draw() {
        ImGui::Begin("Speedhack", 0, ImGuiWindowFlags_NoCollapse);
        if (!speedhackInit) {
            ImGui::SetWindowPos(ImVec2(10 * uselessShit::overlaysize, 410 * uselessShit::overlaysize));
            ImGui::SetWindowSize(ImVec2(190 * uselessShit::overlaysize, 100 * uselessShit::overlaysize));
            ImGui::SetWindowFontScale(uselessShit::overlaysize);
            speedhackInit = true;
        }    
        ImGui::PushItemWidth(174.f * uselessShit::overlaysize);
        if (ImGui::DragFloat("##speedhack", &speedhack, 0.01f, 0.000001f, FLT_MAX, "Speedhack: %f")) {
            if (speedhack != 0) {
                CCDirector::sharedDirector()->getScheduler()->setTimeScale(speedhack);
                if (audio) SpeedhackAudio::set(speedhack);
            }
        }
        if (ImGui::Checkbox("Speedhack Audio", &audio)) {
            if (audio) SpeedhackAudio::set(speedhack);
            else SpeedhackAudio::set(1.f);
        }
        ImGui::End(); 
    }

    void speedhackTab::close() {
        speedhackData.push_back(to_string(speedhack));
        speedhackData.push_back(uselessShit::boolToString(audio));
        fstream file("GDH\\speedhack.data", std::ios::out);
        if (file.is_open()) {
            for (int i = 0; i < (int)speedhackData.size(); i++) {
                file << speedhackData[i] << "\n";
            }
        }
        file.close();
    }
}