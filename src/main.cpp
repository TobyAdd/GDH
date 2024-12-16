#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <imgui-cocos.hpp>
#include "gui.hpp"
#include "config.hpp"
#include "hacks.hpp"
#include "gui_mobile.hpp"

void CheckDir(const std::filesystem::path &path)
{
    if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }
}

$execute {
    ImGuiCocos::get().setForceLegacy(true);

	auto& config = Config::get();
    config.load(fileDataPath);

    if (config.get<float>("speedhack_value", 1.f) < 0.01f)
        config.set<float>("speedhack_value", 1.f);

    if (config.get<float>("tps_value", 60.f) < 1.f)
        config.set<float>("tps_value", 60.f);

    CheckDir(folderMacroPath);
    CheckDir(Config::get().get<std::filesystem::path>("showcases_path", folderPath / "Showcases"));
}

static bool inited = false;
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        // #ifdef GEODE_IS_WINDOWS
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
        // #endif

        // #ifdef GEODE_IS_ANDROID
        auto myButton = CCMenuItemSpriteExtra::create(
			cocos2d::CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(MyMenuLayer::onMyButton)
		);
		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);

		myButton->setID("hacks-button"_spr);

		menu->updateLayout();
        // #endif

        return true;
    }

    void onMyButton(CCObject*) {
		HacksLayer::create()->show();
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