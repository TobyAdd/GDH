#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <imgui-cocos.hpp>
#include "gui.hpp"
#include "config.hpp"
#include "hacks.hpp"

$execute {
    ImGuiCocos::get().setForceLegacy(true);
	
	auto& config = Config::get();
    config.load(fileDataPath);
}

static bool inited = false;
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (!inited) {
            inited = true;

            auto& hacks = Hacks::get();
            hacks.Init();

			auto& gui = Gui::get();
            ImGuiCocos::get().setup([&gui] {
				gui.Init();
            }).draw([&gui] {
                gui.Render();
            });
        }

        return true;
    }
};

#ifdef GEODE_IS_WINDOWS
class $modify(cocos2d::CCEGLView) {
    void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        CCEGLView::onGLFWKeyCallback(window, key, scancode, action, mods);

        if (inited) {
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
				if (key == GLFW_KEY_TAB) {
					auto& gui = Gui::get();
					gui.Toggle();
				}
			}
        }     
    }
};
#endif