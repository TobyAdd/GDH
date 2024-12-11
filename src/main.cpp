#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <imgui-cocos.hpp>
#include "gui.hpp"
#include "config.hpp"
#include "hacks.hpp"

#include <thread>
#include <Geode/modify/EditorUI.hpp>

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

void saveVectorToFile(const std::vector<int>& vec, const std::string& filename) {
    std::ofstream outFile(filename);

    if (!outFile) {
        std::cerr << "Ошибка: невозможно открыть файл " << filename << std::endl;
        return;
    }

    for (const int& num : vec) {
        outFile << num << "\n"; // Записываем числа с новой строки
    }

    outFile.close();

    if (outFile.fail()) {
        std::cerr << "Ошибка: не удалось корректно закрыть файл " << filename << std::endl;
    }
}

int current_id = 0;
bool isDied = false;
bool isNotDied = false;
std::vector<int> bad_ids = {14};
std::vector<int> deco;
std::vector<int> danger;

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

                if (key == GLFW_KEY_M) {
                    std::thread([&]() {
                        while (true) {
                            if (std::find(bad_ids.begin(), bad_ids.end(), current_id) != bad_ids.end())
                                current_id++;

                            geode::log::debug("checking {}", current_id);
                            geode::Loader::get()->queueInMainThread([]() {
                                auto ui = EditorUI::get();
                                ui->m_selectedObjectIndex = current_id;
                            });
                            
                            SetCursorPos(875, 609);
                            Sleep(50);
                            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                            Sleep(50);

                            geode::Loader::get()->queueInMainThread([]() {
                                auto ui = EditorUI::get();
                                ui->onPlaytest(nullptr);
                            });

                            Sleep(200);

                            geode::Loader::get()->queueInMainThread([&]() {
                                auto gj = LevelEditorLayer::get();
                                auto ui = EditorUI::get();
                                if (gj && gj->m_playbackMode == PlaybackMode::Playing && gj->m_player1 && gj->m_player1->m_position.x > 10) {
                                    ui->onStopPlaytest(nullptr);
                                    isNotDied = true;
                                }
                                else if (gj->m_playbackMode != PlaybackMode::Playing) {
                                    isDied = true;
                                }          
                            });
                            Sleep(100);

                            if (isNotDied) {
                                geode::log::debug("{} probably deco", current_id);
                                deco.push_back(current_id);
                            }
                            else if (isDied) {
                                geode::log::debug("{} is danger", current_id);
                                danger.push_back(current_id);
                            }

                            isDied = false;
                            isNotDied = false;
                            current_id++;

                            for (int i = 0; i < 3; i++) {
                                SetCursorPos(377, 231);
                                Sleep(50);
                                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                                Sleep(50);
                            }

                            SetCursorPos(383, 765);
                            Sleep(50);
                            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                            Sleep(50);
                        }
                    }).detach();
                }
			}
        }     
    }
};
#endif