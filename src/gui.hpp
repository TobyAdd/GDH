#pragma once
#include <imgui-cocos.hpp>
#include <imgui_internal.h>
#include <imgui-theme.hpp>
#include "config.hpp"
#include <vector>

class Gui {
public:
    static Gui& get() {
        static Gui instance;
        return instance;
    }

    Gui& operator=(const Gui&) = delete;
    Gui(const Gui&) = delete;

    bool m_license_inited = false;
    bool first_time_re_settings = true;

    bool m_show = false;

    float m_scale = 1.f;
    int m_index_scale = 7;

    bool m_needRescale = true;
    
    bool m_keybindMode = false;

    #ifdef GEODE_IS_WINDOWS
    int m_toggleKey = GLFW_KEY_TAB;
    
    int m_speedhackKey = 0;
	int m_speedhackDecreaseKey = 0;
	int m_speedhackIncreaseKey = 0;
	float m_speedhackStepInterval = 0.1;
    int m_tpsKey = 0;

    int m_playbackKey = 0;
    int m_saveMacroByCurrentNameKey = 0;
    int m_loadMacroByCurrentNameKey = 0;
    int m_frameAdvanceEnableKey = GLFW_KEY_C;
    int m_frameAdvanceDisableKey = GLFW_KEY_V;

    int m_optionsKey = 0;
    int m_resetLevelKey = 0;
    int m_practiceModeKey = 0;
    int m_resetVolumeKey = 0;
    int m_uncompleteLevelKey = 0;

    int m_startposSwitcherLeftKey = GLFW_KEY_Q;
    int m_startposSwitcherRightKey = GLFW_KEY_E;

    int m_autoDeafenKey = 0;

    bool m_waitingForBindKey = false;
    int m_keyToSet = -1;

    #endif

    void renderKeyButton(const std::string& label, int& key, bool withoutKeybindsMode = false);

    void Render();
    void Init();
    void Toggle();
    void License();
private:
    Gui() = default;

    void animateAlpha();
    void updateCursorState();
};

namespace ImGuiH {
    struct PopupMessage {
        std::string caption;
        float expiry_time;
        float start_time;
    };

    class Popup {
    public:
        static Popup& get() {
            static Popup instance;
            return instance;
        }

        Popup& operator=(const Popup&) = delete;
        Popup(const Popup&) = delete;

        float popupDuration = 5.0f;
        std::vector<PopupMessage> messages;

        void add_popup(const std::string& caption) {
            float currentTime = static_cast<float>(ImGui::GetTime());
            messages.insert(messages.begin(), {caption, currentTime + popupDuration, currentTime});
        }

        void render() {
            auto& gui = Gui::get();
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            float currentY = displaySize.y - 10.0f;
            float spacing = 10.0f;
            float scale = gui.m_scale;

            messages.erase(std::remove_if(messages.begin(), messages.end(),
                [](const PopupMessage& msg) { return ImGui::GetTime() > msg.expiry_time; }), messages.end());

            for (const auto& message : messages) {
                float currentTime = static_cast<float>(ImGui::GetTime());

                float alpha = 1.0f;
                float fadeDuration = 0.1f;

                if (currentTime < message.start_time + fadeDuration) {
                    alpha = (currentTime - message.start_time) / fadeDuration;
                } else if (currentTime > message.expiry_time - fadeDuration) {
                    alpha = (message.expiry_time - currentTime) / fadeDuration;
                }

                ImVec2 textSize = ImGui::CalcTextSize(message.caption.c_str());
                float popupWidth = textSize.x + 20.0f * scale;
                float popupHeight = textSize.y + 20.0f * scale;

                ImGui::SetNextWindowPos({displaySize.x - popupWidth - 10.0f * scale, currentY - popupHeight});
                ImGui::SetNextWindowSize({popupWidth, popupHeight});

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

                if (ImGui::Begin(("Popup##" + std::to_string(reinterpret_cast<std::uintptr_t>(&message))).c_str(),
                                nullptr,
                                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | 
                                ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::TextUnformatted(message.caption.c_str());
                    ImGui::End();
                }

                ImGui::PopStyleVar();

                currentY -= (popupHeight + spacing);
            }
        }
    private:
        Popup() = default;
    };

    static std::string removeTrailingHash(std::string_view str) {
        auto it = str.find("##");
        if (it != std::string::npos) {
            return std::string(str.substr(0, it));
        }
        return std::string(str);
    }

    static bool CircularButton(const char* id, float radius, ImColor color, bool border = false, ImColor border_color = ImColor(255, 255, 255))
    {
        ImGui::PushID(id);

        ImVec2 center = ImGui::GetCursorScreenPos();
        center.x += radius;
        center.y += radius;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        if (border)
        {
            draw_list->AddCircleFilled(center, radius + 1.f, border_color);
        }

        draw_list->AddCircleFilled(center, radius, color);

        ImGui::InvisibleButton(id, ImVec2(radius * 2, radius * 2));
        bool is_clicked = ImGui::IsItemClicked();

        ImGui::PopID();
        return is_clicked;
    }

    static bool Checkbox(const char* label, bool* v, float scale)
    {
        auto &gui = Gui::get();
        auto &config = Config::get();

        ImVec2 buttonPosition = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        float buttonWidth = 35.0f * scale;
        float buttonHeight = 22.0f * scale;
        float buttonRadius = buttonHeight * 0.50f; 

        ImGui::InvisibleButton(label, ImVec2(buttonWidth + ImGui::CalcTextSize(removeTrailingHash(label).c_str()).x + 4.f * scale, buttonHeight));    

        bool buttonClicked = false;
        if (ImGui::IsItemClicked()) {
            *v = !*v;
            buttonClicked = true;
        }

        float interpolationFactor = *v ? 1.0f : 0.0f;
        ImGuiContext& imguiContext = *GImGui;
        float animationSpeed = 0.08f;
        if (imguiContext.LastActiveId == imguiContext.CurrentWindow->GetID(label)) {
            float animationProgress = ImSaturate(imguiContext.LastActiveIdTimer / animationSpeed);
            interpolationFactor = *v ? animationProgress : 1.0f - animationProgress;
        }
        
        int color_index = config.get<int>("gui_color_index", 0);
        auto theme = themes[color_index];

        const ImU32 backgroundColor = ImGui::GetColorU32(ImLerp(ImGui::IsItemHovered() ? ImColor(81, 87, 94).Value : ImColor(67, 72, 78).Value, ImColor(theme.color_bg.r, theme.color_bg.g, theme.color_bg.b).Value, interpolationFactor));
        const ImVec2 buttonEndPosition(buttonPosition.x + buttonWidth, buttonPosition.y + buttonHeight);
        drawList->AddRectFilled(buttonPosition, buttonEndPosition, backgroundColor, buttonHeight * 0.5f);
        const ImU32 circleColor = *v ? IM_COL32(theme.color_circle.r, theme.color_circle.g, theme.color_circle.b, int(ImGui::GetStyle().Alpha * 255)) : IM_COL32(140, 144, 153, int(ImGui::GetStyle().Alpha * 255));
        const float circlePositionX = buttonPosition.x + buttonRadius + interpolationFactor * (buttonWidth - buttonRadius * 2.0f);
        drawList->AddCircleFilled(ImVec2(circlePositionX, buttonPosition.y + buttonRadius), buttonRadius - 1.5f, circleColor);

        const ImVec2 textPosition(buttonPosition.x + buttonWidth + 5.0f, buttonPosition.y + buttonHeight / 2.0f - ImGui::GetTextLineHeight() / 2.0f);
        drawList->AddText(textPosition, ImGui::GetColorU32(ImGuiCol_Text), removeTrailingHash(label).c_str());

        return buttonClicked;
    }
    
    static bool RadioButton2(const char* label, bool active, float scale)
    {
        auto &gui = Gui::get();
        auto &config = Config::get();

        ImVec2 buttonPosition = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        float buttonWidth = 22.0f * scale;
        float buttonHeight = 22.0f * scale;

        ImGui::InvisibleButton(label, ImVec2(buttonWidth + ImGui::CalcTextSize(label).x + 4.f, buttonHeight));    

        bool buttonClicked = false;
        if (ImGui::IsItemClicked()) {
            active = true;
            buttonClicked = true;
        }

        float interpolationFactor = active ? 1.0f : 0.0f;
        ImGuiContext& imguiContext = *GImGui;
        float animationSpeed = 0.15f;
        if (imguiContext.LastActiveId == imguiContext.CurrentWindow->GetID(label)) {
            float animationProgress = ImSaturate(imguiContext.LastActiveIdTimer / animationSpeed);
            interpolationFactor = active ? animationProgress : 1.0f - animationProgress;
        }

        int color_index = config.get<int>("gui_color_index", 0);
        auto theme = themes[color_index];

        const ImU32 backgroundColor = ImGui::GetColorU32(ImLerp(ImColor(67, 72, 78).Value, ImColor(theme.color_bg.r, theme.color_bg.g, theme.color_bg.b).Value, interpolationFactor));
        const ImVec2 buttonEndPosition(buttonPosition.x + buttonWidth, buttonPosition.y + buttonHeight);
        drawList->AddRectFilled(buttonPosition, buttonEndPosition, backgroundColor, buttonHeight * 0.5f);

        const ImVec2 textPosition(buttonPosition.x + buttonWidth + 5.0f, buttonPosition.y + buttonHeight / 2.0f - ImGui::GetTextLineHeight() / 2.0f);
        drawList->AddText(textPosition, ImGui::GetColorU32(ImGuiCol_Text), label);

        return buttonClicked;
    }

    static bool RadioButton(const char* label, int* v, int v_button, float scale)
    {
        const bool pressed = RadioButton2(label, *v == v_button, scale);
        if (pressed)
            *v = v_button;
        return pressed;
    }
    
    static bool Button(const char *label, const ImVec2 &size = ImVec2(0, 0))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, Config::get().get("gui_inverted", false) ? ImGui::GetStyle().Colors[ImGuiCol_Text] : ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
        auto ret = ImGui::Button(label, size);
        ImGui::PopStyleColor();
        return ret;
    }

    static bool ArrowButton(const char *str_id, ImGuiDir dir)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, Config::get().get("gui_inverted", false) ? ImGui::GetStyle().Colors[ImGuiCol_Text] : ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
        auto ret = ImGui::ArrowButton(str_id, dir);
        ImGui::PopStyleColor();
        return ret;
    }

    static bool Combo(const char *label, int *current_item, const char *const *items, int items_count, int popup_max_height_in_items = -1)
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, Config::get().get("gui_inverted", false) ? ImGui::GetStyle().Colors[ImGuiCol_Text] : ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
        auto ret = ImGui::Combo(label, current_item, items, items_count, popup_max_height_in_items);
        ImGui::PopStyleColor();
        return ret;
    }
}
