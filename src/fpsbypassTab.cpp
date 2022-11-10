#include "fpsbypassTab.h"
#include "fpsbypass.h"

vector<string> fpsbypassTab::fpsbypassData;
bool fpsbypassTab::fpsbypassInit = false;

namespace fpsbypassTab {
    void fpsbypassTab::init() {
        string line;
        fstream fileConfig("GDH\\fpsbypass.data", std::ios::in);
        if (fileConfig.is_open()) {
            getline(fileConfig, line);
            FPSMultiplier::g_target_fps = stof(line);
            getline(fileConfig, line);
            FPSMultiplier::g_enabled = uselessShit::stringToBool(line);
            getline(fileConfig, line);
            FPSMultiplier::g_fps_enabled = uselessShit::stringToBool(line);
        }
    }

    void fpsbypassTab::draw() {
        ImGui::Begin("FPS Bypass", 0, ImGuiWindowFlags_NoCollapse);
        if (!fpsbypassInit) {
            ImGui::SetWindowPos(ImVec2(410 * uselessShit::overlaysize, 410 * uselessShit::overlaysize));
            ImGui::SetWindowSize(ImVec2(190 * uselessShit::overlaysize, 100 * uselessShit::overlaysize));
            ImGui::SetWindowFontScale(uselessShit::overlaysize);
            fpsbypassInit = true;
        }    
        ImGui::PushItemWidth(150.f * uselessShit::overlaysize);
        ImGui::DragFloat("##fpsnum", &FPSMultiplier::g_target_fps, 0.1f, 1.f, FLT_MAX, "FPS: %f");
        if (FPSMultiplier::g_target_fps < 15) {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("FPS is very low. Can be overlay lagging!");   
            ImGui::PopStyleColor();     
        }
        ImGui::SameLine();

        ImGui::Checkbox("##fpsbypass", &FPSMultiplier::g_fps_enabled);

        ImGui::Checkbox("FPS Multipiler", &FPSMultiplier::g_enabled);
        ImGui::End(); 
    }

    void fpsbypassTab::close() {
        fstream file("GDH\\fpsbypass.data", std::ios::out);
        fpsbypassData.push_back(to_string(FPSMultiplier::g_target_fps));
        fpsbypassData.push_back(uselessShit::boolToString(FPSMultiplier::g_target_fps));
        fpsbypassData.push_back(uselessShit::boolToString(FPSMultiplier::g_fps_enabled));
        if (file.is_open()) {
            for (int i = 0; i < (int)fpsbypassData.size(); i++) {
                file << fpsbypassData[i] << "\n";
            }
        }
        file.close();
    }
}