#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <imgui-cocos.hpp>
#ifdef GEODE_IS_WINDOWS
#include "gui.hpp"
#endif
#include "gui_mobile.hpp"
#include "config.hpp"
#include "hacks.hpp"

void CheckDir(const std::filesystem::path &path)
{
    if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }
}

$execute {
    auto *mod = geode::Mod::get();
    ImGuiCocos::get().setForceLegacy(mod->getSettingValue<bool>("legacy-render"));

	auto& config = Config::get();
    config.load(fileDataPath);

    float speedhack_value = config.get<float>("speedhack_value", 1.f);
    speedhack_value = std::clamp(speedhack_value, 0.25f, 500.f);
    config.set<float>("speedhack_value", speedhack_value);

    float tps_value = config.get<float>("tps_value", 60.f);
    tps_value = std::clamp(tps_value, 30.f, 240.f);
    config.set<float>("tps_value", tps_value);

    CheckDir(folderMacroPath);
    CheckDir(Config::get().get<std::filesystem::path>("showcases_path", folderPath / "Showcases"));
}

static bool inited = false;

void setupButton(cocos2d::CCNode* parent, const cocos2d::CCPoint& position, const std::string& buttonID) {
    auto sprite = cocos2d::CCSprite::create(Config::get().get<bool>("invisible_ui_button", false) ? "GDH_buttonInvisible.png"_spr : "GDH_buttonUI.png"_spr);
    sprite->setScale(0.85f);
    auto myButton = geode::prelude::CCMenuItemExt::createSpriteExtra(sprite, [](CCMenuItemSpriteExtra* sender) {
        auto& config = Config::get();
        if (!config.get<bool>("license_accepted", false)) {
            FLAlertLayer::create("GDH Beta Testing", "Note: GDH is currently in beta testing, so some elements may be unstable/unfinished\n\nIn case of any issues/crashes, please report the problem in the GDH issues section on GitHub", "OK")->show();
            config.set<bool>("license_accepted", true);
            return;
        }
        HacksLayer::create()->show();
    });
    myButton->setPosition(position);

    auto menu = cocos2d::CCMenu::create();
    menu->setPosition(0, 0);
    menu->addChild(myButton);
    parent->addChild(menu);

    myButton->setID(buttonID);
}

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (!inited) {
            inited = true;

            auto& hacks = Hacks::get();
            hacks.Init();
            hacks.loadKeybinds();

            #ifdef GEODE_IS_WINDOWS
            auto& gui = Gui::get();
            ImGuiCocos::get().setup([&gui] {
                gui.Init();
            }).draw([&gui] {
                gui.Render();
            });
            #endif
        }

        #ifdef GEODE_IS_ANDROID

        auto myButton = geode::prelude::CCMenuItemExt::createSpriteExtra(
            cocos2d::CCSprite::create(Config::get().get<bool>("invisible_ui_button", false) ? "GDH_buttonInvisible.png"_spr : "GDH_buttonUI.png"_spr), 
            [this](CCMenuItemSpriteExtra* sender) {
                auto& config = Config::get();
                if (!config.get<bool>("license_accepted", false)) {
                    FLAlertLayer::create("GDH Beta Testing", "<cr>Note: GDH is currently in beta testing, so some elements may be unstable/unfinished</c>\n\n<cg>In case of any issues/crashes, please report the problem in the GDH issues section on GitHub</c>", "OK")->show();
                    config.set<bool>("license_accepted", true);
                    return;
                }
                HacksLayer::create()->show();
            }
        );
        auto menu = this->getChildByID("bottom-menu");
        menu->addChild(myButton);

        myButton->setID("hacks-button"_spr);
        menu->updateLayout();

        #endif

        return true;
    }
};

#ifdef GEODE_IS_ANDROID

class $modify(PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        auto top = cocos2d::CCDirector::sharedDirector()->getScreenTop();
        static geode::Mod* clickSoundsMod = geode::Loader::get()->getLoadedMod("beat.click-sound");
        static bool hasClickSoundsMod = clickSoundsMod != nullptr && clickSoundsMod->isEnabled();
        setupButton(this, {40, hasClickSoundsMod ? top - 80.f : top - 45.f}, "hacks-button"_spr);
    }
};

class $modify(LevelSelectLayer) {
    bool init(int page) {
        if (!LevelSelectLayer::init(page)) return false;
        setupButton(this, {30, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 70.f}, "hacks-button"_spr);
        return true;
    }
};

class $modify(CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;
        setupButton(this, {30, cocos2d::CCDirector::sharedDirector()->getWinSize().height / 2}, "hacks-button"_spr);
        return true;
    }
};

class $modify(LevelBrowserLayer) {
    bool init(GJSearchObject* p0) {
        if (!LevelBrowserLayer::init(p0)) return false;
        setupButton(this, {30, (cocos2d::CCDirector::sharedDirector()->getWinSize().height / 2) - 45.f}, "hacks-button"_spr);
        return true;
    }
};

class $modify(EditorPauseLayer) {
    bool init(LevelEditorLayer* p0) {
        if (!EditorPauseLayer::init(p0)) return false;
        setupButton(this, {cocos2d::CCDirector::sharedDirector()->getScreenRight() - 25.f, 
                           cocos2d::CCDirector::sharedDirector()->getScreenTop() - 25.f}, "hacks-button"_spr);
        return true;
    }
};

class $modify(LevelSearchLayer) {
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;
        setupButton(this, {30, cocos2d::CCDirector::sharedDirector()->getWinSize().height / 2}, "hacks-button"_spr);
        return true;
    }
};

#endif


#ifdef GEODE_IS_WINDOWS
class $modify(cocos2d::CCEGLView) {
    void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        CCEGLView::onGLFWKeyCallback(window, key, scancode, action, mods);

        if (inited) {
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                auto& gui = Gui::get();
                auto& hacks = Hacks::get();
                if (gui.m_keybindMode && gui.m_waitingForBindKey) {
                    if (key == GLFW_KEY_BACKSPACE)
                        gui.m_keyToSet = 0;
                    else
                        gui.m_keyToSet = key;
                    gui.m_waitingForBindKey = false;
                }
                else if (!gui.m_keybindMode) {
                    if (key == gui.m_toggleKey) {
                        gui.Toggle();
                    }

                    hacks.toggleKeybinds(key);
                }
			}
        }     
    }
};

class $modify(MouseKeybindingsManagerCCEGLVHook, cocos2d::CCEGLView) {
    void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
        CCEGLView::onGLFWMouseCallBack(window, button, action, mods);
        if (inited) {
            if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
                auto& io = ImGui::GetIO();
                io.AddMouseButtonEvent(1, true);
            }
            else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_RIGHT) {
                auto& io = ImGui::GetIO();
                io.AddMouseButtonEvent(1, false);
            }
        }
    }
};
#endif