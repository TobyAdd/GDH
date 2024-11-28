#include "gui.hpp"
#include <imgui/imgui_stdlib.h>
#include <font.hpp>
#include "config.hpp"
#include <matjson.hpp>
#include "hacks.hpp"
#include "memory.hpp"
#include "labels.hpp"

std::chrono::steady_clock::time_point animationStartTime;
bool isAnimating = false;
bool isFadingIn = false;

std::string search_text;

int selected_label_corner = 0;
int selected_label_type = 0;
std::string selected_label_text;

void Gui::updateCursorState() {
    bool canShowInLevel = true;
    if (auto* playLayer = PlayLayer::get()) {
        canShowInLevel = playLayer->m_hasCompletedLevel ||
                         playLayer->m_isPaused ||
                         GameManager::sharedState()->getGameVariable("0024");
    }
    if (m_show || canShowInLevel)
        PlatformToolbox::showCursor();
    else
        PlatformToolbox::hideCursor();
}

void Gui::animateAlpha()
{
    ImGuiStyle& style = ImGui::GetStyle();

    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> diff = currentTime - animationStartTime;
    float elapsed = diff.count();

    float time = m_anim_durr / 1000.0f;
    if (elapsed >= time)
    {
        style.Alpha = isFadingIn ? 1.0f : 0.0f;
        isAnimating = false;

        if (!isFadingIn)
        {
            m_show = !m_show;
            Config::get().save(fileDataPath);
            updateCursorState();
        }

        return;
    }

    float t = elapsed / time;
    float alpha = isFadingIn ? 0.0f + t: 1.0f - t;
    style.Alpha = alpha;
}

std::vector<std::string> stretchedWindows;
void Gui::Render() {
    #ifdef GEODE_IS_ANDROID
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1);
    ImGui::Begin("toggle gui");
    if (ImGui::Button("toggle")) {
        Toggle();
    }
    ImGui::End();
    ImGui::PopStyleVar();
    #endif


    if (isAnimating) {
        animateAlpha();
    }
    
    if (!m_show) return;

    auto& hacks = Hacks::get();
    for (auto& win : hacks.m_windows) {
        std::string windowName = win.name;
        if (std::find(stretchedWindows.begin(), stretchedWindows.end(), windowName) == stretchedWindows.end())
        {
            ImVec2 windowSize = ImVec2(win.w, win.h);
            ImVec2 windowPos = ImVec2(win.x, win.y);

            if (m_needRescale) {
                windowSize = ImVec2(win.orig_w * m_scale, win.orig_h * m_scale);
                windowPos = ImVec2(win.orig_x * m_scale, win.orig_y * m_scale);
            }

            ImGui::SetNextWindowSize(windowSize);
            ImGui::SetNextWindowPos(windowPos);

            stretchedWindows.push_back(windowName);
        }

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Begin(windowName.c_str(), 0, ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs);
        ImGui::PopFont();

        if (!ImGui::IsWindowCollapsed()) {
            auto size = ImGui::GetWindowSize();
            auto pos = ImGui::GetWindowPos();

            win.w = size.x;
            win.h = size.y;
            win.x = pos.x;
            win.y = pos.y;
        }


        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);    

       
        auto &config = Config::get();
        if (windowName == "GDH Settings") {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputTextWithHint("##Search", "Search:", &search_text);

            if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsItemActive())
                ImGui::SetWindowFocus(NULL);

            const char* items[] = {"25%", "50%", "75%", "80%", "85%", "90%", "95%", "100%", "125%", "150%", "175%", "200%", "250%", "300%", "400%"};
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::Combo("##Menu scale", &m_index_scale, items, IM_ARRAYSIZE(items))) {
                m_scale = float(atof(items[m_index_scale])) / 100.0f;    
                config.set<float>("gui_scale", m_scale);
                config.set<int>("gui_index_scale", m_index_scale);
                m_needRescale = true;
                ImGuiCocos::get().reload();
            }

            int alpha = int(ImGui::GetStyle().Alpha * 255);

            for (int i = 0; i < themes.size(); ++i) {
                const auto& theme = themes[i];
                ImColor button_color(theme.color_bg.r, theme.color_bg.g, theme.color_bg.b, alpha);
                
                if (i == 0)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.f * m_scale);
                
                if (ImGuiH::CircularButton(fmt::format("COLOR_{}", i).c_str(), 10.f * m_scale, button_color)) {
                    config.set<int>("gui_color_index", i);
                    ApplyColor(theme);
                }

                ImGui::SameLine();
            }

            bool inverted = config.get("gui_inverted", false);
            ImColor inverted_button_color = inverted ? ImColor(255, 255, 255) : ImColor(27, 27, 29, alpha);
            ImColor inverted_button_hover_color = ImColor(64, 64, 64, alpha);

            if (ImGuiH::CircularButton("TOGGLE_INVERT", 10.f * m_scale, inverted_button_color, true, inverted_button_hover_color)) {
                config.set("gui_inverted", !inverted);
                ApplyGuiColors(!inverted);
            }
        }
        else if (windowName == "Framerate") {
            bool tps_enabled = config.get<bool>("tps_enabled", false);
            float tps_value = config.get<float>("tps_value", 240.f);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (35 + 5) * m_scale);
            if (ImGui::DragFloat("##tps_value", &tps_value, 1, 1, FLT_MAX, "%0.f TPS"))
                config.set<float>("tps_value", tps_value);

            ImGui::SameLine();
            if (ImGuiH::Checkbox("##tps_enabled", &tps_enabled, m_scale))
                config.set<bool>("tps_enabled", tps_enabled);
            
            bool speedhack_enabled = config.get<bool>("speedhack_enabled", false);
            float speedhack_value = config.get<float>("speedhack_value", 1.f);

            bool speedhackAudio_enabled = config.get<bool>("speedhackAudio_enabled", false);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (35 + 5) * m_scale);
            if (ImGui::DragFloat("##speedhack_value", &speedhack_value, 0.01f, 0, FLT_MAX, "Speed: %.2fx")) 
                config.set<float>("speedhack_value", speedhack_value);

            ImGui::SameLine();
            if (ImGuiH::Checkbox("##speedhack_enabled", &speedhack_enabled, m_scale))
                config.set<bool>("speedhack_enabled", speedhack_enabled);

            if (ImGuiH::Checkbox("Speedhack Audio", &speedhackAudio_enabled, m_scale))
                config.set<bool>("speedhackAudio_enabled", speedhackAudio_enabled);
        }
        else if (windowName == "Variables") { 
            static uintptr_t address = geode::base::get();
            ImGui::Text("%s", fmt::format("{:x} ({:x})", address, address - geode::base::get()).c_str());

            if (ImGui::Button("scan"))
                address = memory::PatternScan(address+1, 0, "00 60 6A 48");

            if (ImGui::Button("scan2"))
                address = memory::PatternScan(address+1, 0, "80 67 6A 48");

            if (ImGui::Button("reset"))
                address = geode::base::get();
        }
        else if (windowName == "Labels") {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x/2);
            if (ImGui::DragFloat("##Label Opacity", &Labels::get().opacity, 0.01f, 0.f, 1.f, "Opacity: %.2f")) config.set<float>("label-opacity", Labels::get().opacity);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::DragFloat("##Label Size", &Labels::get().size, 0.01f, 0.f, 1.f, "Size: %.2f")) config.set<float>("label-size", Labels::get().size);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::DragFloat("##Label Padding", &Labels::get().padding, 1.f, 0.f, 50.f, "Label Padding: %.1fpx")) config.set<float>("label-padding", Labels::get().padding);
            
            const char *labels_positions[] = {"Top Left", "Top Right", "Top", "Bottom Left", "Bottom Right", "Bottom"};
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::Combo("##labelspos", &selected_label_corner, labels_positions, 6, 6);

            const char *label_types[] = {
                "Hui",
                "Custom Text",
            };
            int label_types_count = sizeof(label_types)/sizeof(label_types[0]);
            
            ImGui::Combo("##labeladdtype", &selected_label_type, label_types, label_types_count);
            if (selected_label_type == label_types_count - 1)
                ImGui::InputText("##labelinput", &selected_label_text);

            ImGui::SameLine();
            if (ImGui::Button("+")) {
                std::string text;
                if (selected_label_type == label_types_count - 1) text = selected_label_text;
                else if (selected_label_type == 0) text = "eto %hui";
                
                Label l((LabelCorner) (selected_label_corner+1), text);
                Labels::get().add(l);
            }
            
            ImGui::Separator();
            ImGui::BeginChild("Labels");
            ImGui::Spacing();

            if (Labels::get().labels.size() == 0) {
                ImGui::TextDisabled("No labels in this corner");
            }
            
            for (size_t index = 0; index < Labels::get().labels.size(); index++) {
                Label& item = Labels::get().labels[index];
                
                ImGui::PushID(index);

                ImGui::Selectable("  =", false, 0, {20.f, 20.f});
                ImGui::SameLine();
                
                if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) >= 2 && ImGui::IsItemHovered()) {
                    Labels::get().remove(index);
                }
                
                if (ImGui::BeginDragDropSource()) {
                    ImGui::TextUnformatted(item.format_text.c_str());
                    ImGui::SetDragDropPayload("LBLMOVE", &index, sizeof(size_t));
                    ImGui::EndDragDropSource();
                }
                
                if (ImGui::BeginDragDropTarget()) {
                    ImGuiDragDropFlags target_flags = 0;
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("LBLMOVE", target_flags)) {
                        size_t move_from = *(size_t*)payload->Data;
                        Labels::get().swap(move_from, index);
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::InputText("##inptext", &item.format_text);
                
                ImGui::PopID();
            }
            ImGui::EndChild();
        }
        else {
            for (auto& hck : win.hacks) {
                bool enabled = config.get(hck.config, false);

                std::string search_name = hck.name;
                std::string search_item = search_text;
                
                std::transform(search_item.begin(), search_item.end(), search_item.begin(), ::tolower);
                std::transform(search_name.begin(), search_name.end(), search_name.begin(), ::tolower);

                bool founded = search_item.empty() ? true : (search_name.find(search_item) != std::string::npos);
                ImGui::PushStyleColor(ImGuiCol_Text, founded ? ImGui::GetStyle().Colors[ImGuiCol_Text] : ImColor(64, 64, 64).Value);

                if (ImGuiH::Checkbox(hck.name.c_str(), &enabled, m_scale)) {
                    config.set(hck.config, enabled);
                    if (!hck.game_var.empty())
                        GameManager::get()->setGameVariable(hck.game_var.c_str(), enabled);
                    if (hck.handlerFunc) hck.handlerFunc(enabled);
                }

                if (ImGui::IsItemHovered() && !hck.desc.empty()) {
                    ImGui::SetTooltip("%s", hck.desc.c_str());
                }

                if (hck.handlerCustomWindow) {
                    ImGui::SameLine();
                    if (ImGui::ArrowButton(fmt::format("{} Settings", hck.name).c_str(), ImGuiDir_Right)) {
                        ImGui::OpenPopup(fmt::format("{} Settings", hck.name).c_str());
                    }

                    if (ImGui::BeginPopupModal(fmt::format("{} Settings", hck.name).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        hck.handlerCustomWindow();

                        if (ImGui::Button("Close", {400 * m_scale, NULL})) {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                
                ImGui::PopStyleColor();
            }
        }

        ImGui::PopFont();
        ImGui::End();
    }
}

void Gui::Init() {
    auto &config = Config::get();

    stretchedWindows.clear();

    m_scale = config.get<float>("gui_scale", 1.f);
    m_index_scale = config.get<int>("gui_index_scale", 7);

    ApplyGuiColors(config.get("gui_inverted", false));
    ApplyColor(themes[config.get<int>("gui_color_index", 0)]);
    ApplyStyle(m_scale);
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = NULL;
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 18.f * m_scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 32.f * m_scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 20.f * m_scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->Build();
}

void Gui::Toggle() {
    if (!isAnimating)
    {
        isAnimating = true;
        isFadingIn = !isFadingIn;
        animationStartTime = std::chrono::steady_clock::now();

        if (isFadingIn)
        {
            m_show = !m_show;
            updateCursorState();        
        }
    }
}
