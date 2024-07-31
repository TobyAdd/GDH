#include "includes.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <imgui-cocos.hpp>
#include <imgui-theme.hpp>
#include <font.hpp>
#include "hacks.hpp"
#include "gui.hpp"
#include "hooks.hpp"
#include "replayEngine.hpp"

void CheckDir(const std::filesystem::path &path)
{
    if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }
}

$execute {
    ImGuiCocos::get().setForceLegacy(true);
    CheckDir(hacks::folderMacroPath);
    hacks::load(hacks::fileDataPath, hacks::windows);
    hacks::update_framerate();
    hacks::init();
}

static bool inited = false;
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (!inited) {
            inited = true;
            ImGuiCocos::get().setup([] {
                gui::Unload();
                ApplyColor();
                ApplyStyle(gui::scale);
                ImGuiIO &io = ImGui::GetIO();
                io.IniFilename = NULL;
                io.Fonts->Clear();
                io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 18.f * gui::scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
                io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 32.f * gui::scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
                io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 20.f * gui::scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
                io.Fonts->Build();
            }).draw([] {
                gui::RenderMain();
            });
        }

        return true;
    }
};

class $modify(cocos2d::CCEGLView) {
    void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        CCEGLView::onGLFWKeyCallback(window, key, scancode, action, mods);

        if (inited) {
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                if (!gui::change_keybind) {
                    if (key == gui::menu_key) gui::Toggle();
                    else if (key == hacks::speed_key) hacks::speed_enabled = !hacks::speed_enabled;
                    else if (key == hacks::playback_key) {
                        engine.mode = (engine.mode == state::play) ? state::disable : state::play;
                    }

                    if (key == hacks::frame_advance_key) {
                        engine.frame_advance = true;
                        engine.next_frame = true;
                    } 
                    else if (key == hacks::frame_advance_disable_key) engine.frame_advance = false;

                    startpos_switcher::handleKeyPress(key);  
                    gui::toggleKeybinds(key);
                }
                else {
                    gui::change_keybind = false;
                    if (key != GLFW_KEY_BACKSPACE)
                        gui::keybind_key = key;
                    else
                        gui::keybind_key = 0;
                }
            }
        }        
    }
};