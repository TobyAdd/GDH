#include "bypassTab.h"
#include "creatorTab.h"
#include "fpsbypassTab.h"
#include "nongTab.h"
#include "playerTab.h"
#include "speedhackTab.h"
#include "toolsTab.h"
#include "universalTab.h"
#include "PlayLayer.h"
#include "fpsbypass.h"
#include "spamBot.h"
#include <commdlg.h>

bool toolsTab::toolsInit = false;
int toolsTab::invisframes = 0;
bool toolsTab::frameadvance = false;
bool toolsTab::nextframe = false;
bool opentree = false;

void ResetWindowInit() {
    bypassTab::bypassInit = false;
    creatorTab::creatorInit = false;
    fpsbypassTab::fpsbypassInit = false;
    nongTab::nongTabInit = false;
    nongTab::nongTabInit = false;
    playerTab::playerInit = false;
    speedhackTab::speedhackInit = false;
    universalTab::universalInit = false;
    toolsTab::toolsInit = false;
}

bool NextMoney() {
    for (int i = 0; i < (int)gd::PlayLayer::get()->m_pObjects->count(); i++) {
		gd::GameObject* obj = (gd::GameObject*)gd::PlayLayer::get()->m_pObjects->objectAtIndex(i);

		if (obj->m_nObjectType == gd::GameObjectType::kGameObjectTypeSecretCoin || obj->m_nObjectType == gd::GameObjectType::kGameObjectTypeUserCoin) {
            if (obj->getPositionX() > gd::PlayLayer::get()->m_pPlayer1->m_position.x) {
                gd::PlayLayer::get()->m_pPlayer1->m_position.x = obj->getPositionX();
                gd::PlayLayer::get()->m_pPlayer1->m_position.y = obj->getPositionY();
                return true;
            }
		}
	}
    return false;
}

namespace toolsTab {

    void toolsTab::init() {
        string line;
        fstream fileConfig("GDH\\toolstab.data", std::ios::in);
        if (fileConfig.is_open()) {
            getline(fileConfig, line);
            PlayLayer::rgbIcons = uselessShit::stringToBool(line);
            getline(fileConfig, line);
            uselessShit::overlaysize = stof(line);
            getline(fileConfig, line);
            PlayLayer::hideatt = uselessShit::stringToBool(line);
            getline(fileConfig, line);
            PlayLayer::showallatt = uselessShit::stringToBool(line);
            getline(fileConfig, line);
            PlayLayer::layoutMode = uselessShit::stringToBool(line);
            getline(fileConfig, line);
            DashReplayEngine::onlydashreplay = uselessShit::stringToBool(line);
        }
    }

    void toolsTab::draw() {
        ImGui::Begin("Tools", 0, ImGuiWindowFlags_NoCollapse);
        if ((ImGui::GetWindowPos().x * uselessShit::overlaysize) + (ImGui::GetWindowPos().y * uselessShit::overlaysize) > CCDirector::sharedDirector()->getWinSizeInPixels().width) {
            uselessShit::overlaysize -= 0.001f;
            ResetWindowInit();
        }
        if (!toolsInit) {
            ImGui::SetWindowPos(ImVec2(810 * uselessShit::overlaysize, 10 * uselessShit::overlaysize));
            ImGui::SetWindowSize(ImVec2(190 * uselessShit::overlaysize, 500 * uselessShit::overlaysize));
            ImGui::SetWindowFontScale(uselessShit::overlaysize);
            toolsInit = true;
        } 

        if (ImGui::TreeNode("Show Info")) {		
            ImGui::Text("Soon!");	
			ImGui::TreePop();
		}

        if (ImGui::TreeNode("RGB Icons")) {	
            ImGui::Checkbox("Enable", &PlayLayer::rgbIcons);		
			ImGui::TreePop();
		}

        if (ImGui::TreeNode("GDH Settings")) {
            ImGui::PushItemWidth(150.f * uselessShit::overlaysize);	
            if (ImGui::DragFloat("##gdhsize", &uselessShit::overlaysize, 0.001f, 0.1f, FLT_MAX, "Overlay Size: %f")) {
                ResetWindowInit();
            }
			ImGui::TreePop();
		}

        if (ImGui::TreeNode("Attempt label")) {	
            ImGui::Checkbox("Hide Attempts", &PlayLayer::hideatt);
			ImGui::Checkbox("Show All Attempts", &PlayLayer::showallatt);
            ImGui::TreePop();
		}

        if (ImGui::TreeNode("Level Mods")) {
            ImGui::Checkbox("Layout mode", &PlayLayer::layoutMode);
            if (ImGui::Button("Teleport to next coin")) {
                if (gd::PlayLayer::get()) {
                    if (!NextMoney()) {
                        gd::PlayLayer::get()->m_pPlayer1->m_position.x = FLT_MAX;
                    }
                }
            }			
			ImGui::TreePop();
		}

        if (ImGui::TreeNode("Spam Bot")) {
            ImGui::Checkbox("Enable", &spambot::enable);
            ImGui::Checkbox("1 Player", &spambot::player1);
            ImGui::SameLine();
            ImGui::Checkbox("2 Player", &spambot::player2);
            ImGui::PushItemWidth(150.f * uselessShit::overlaysize);	
            ImGui::DragInt("##spampush", &spambot::push, 1, 1, INT_MAX, "Push: %i");
            ImGui::PushItemWidth(150.f * uselessShit::overlaysize);	
            ImGui::DragInt("##spamreelase", &spambot::release, 1, 1, INT_MAX, "Release: %i");
			ImGui::TreePop();
		}

        if (ImGui::TreeNode("Frame Advance")) {	
            ImGui::Text("Frame Advance: %i", frameadvance);
            ImGui::Text("C - Enable + Next Frame");
            ImGui::Text("F - Disable");
			ImGui::TreePop();
		}
        if (!opentree) {ImGui::SetNextItemOpen(true); opentree = true;}
        if (ImGui::TreeNode("DashReplay")) {
            ImGui::Text("Frame %i", DashReplayEngine::frame);	
            if (ImGui::RadioButton("Disable", &DashReplayEngine::mode, 0)) {

            }	
            if (ImGui::RadioButton("Record", &DashReplayEngine::mode, 1)) {
                DashReplayEngine::clearMacro();
                FPSMultiplier::g_enabled = true;
                FPSMultiplier::g_fps_enabled = true;
            }	
            if (ImGui::RadioButton("Play", &DashReplayEngine::mode, 2)) {
                FPSMultiplier::g_enabled = true;
                FPSMultiplier::g_fps_enabled = true;
            }	
            if (ImGui::Button("Clear")) {
                DashReplayEngine::clearMacro();
            }
            if (ImGui::Button("Save")) {
                OPENFILENAME ofn;
                char fileName[MAX_PATH] = "";
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lpstrFilter = "DashReplay Replay (*.txt)\0*.txt";
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.lpstrFile = fileName;
                ofn.nMaxFile = MAX_PATH;
                if (GetSaveFileName(&ofn)) {
                    string s(fileName);
                    s += ".txt";
                    DashReplayEngine::saveMacro(s);
                }
            }
            if (ImGui::Button("Load")) {
                OPENFILENAME ofn;
                char fileName[MAX_PATH] = "";
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lpstrFilter = "DashReplay Replay (*.txt)\0*.txt";
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.lpstrFile = fileName;
                ofn.nMaxFile = MAX_PATH;
                if (GetOpenFileName(&ofn))  {
                    string s(fileName);
                    DashReplayEngine::loadMacro(s);
                }
            }
            if (ImGui::TreeNode("Meta")) {
                auto pl = gd::PlayLayer::get();
                if (pl) {
                    ImGui::Text("Player 1:\nXP1: %f\nYP1: %f\nRP1: %f\nYVP1: %f\nDOWNP1:%i\nPlayer 2:\nXP2: %f\nYP2: %f\nRP2: %f\nYVP2: %f\nDOWNP2:%i",
                                        pl->m_pPlayer1->m_position.x, pl->m_pPlayer1->m_position.y, pl->m_pPlayer1->getRotation(), pl->m_pPlayer1->m_yAccel, DashReplayEngine::DownP1,
                                            pl->m_pPlayer2->m_position.x, pl->m_pPlayer2->m_position.y, pl->m_pPlayer2->getRotation(), pl->m_pPlayer2->m_yAccel, DashReplayEngine::DownP1);
                }                
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Noclip Frames")) {
                ImGui::InputInt("##InvisibleFrames", &invisframes);
                if (ImGui::Button("Add")) DashReplayEngine::invframes.push_back(invisframes);
                if (ImGui::Button("Clear")) DashReplayEngine::invframes.clear();
                for (int i : DashReplayEngine::invframes) {
                    ImGui::Text(to_string(i).c_str());
                }
                ImGui::TreePop();
            } 
            ImGui::Checkbox("Only DashReplay", &DashReplayEngine::onlydashreplay);
			ImGui::TreePop();
		}
        if (ImGui::TreeNode("GDH About")) {
            ImGui::Text("GDH v1.3 by TobyAdd");
            ImGui::Text("DashReplay Engine v3");
            ImGui::TreePop();
        } 
    
        if (ImGui::TreeNode("Special Thanks")) {
            ImGui::Text("Absolute");
            ImGui::Text("Adaf");
            ImGui::Text("Adarift");
            ImGui::Text("Eimaen");
            ImGui::Text("FFmpeg Creator");
            ImGui::Text("Fig");
            ImGui::Text("HJfod");
            ImGui::Text("Mat");
            ImGui::Text("PixelSuft");
            ImGui::Text("qb");
            ImGui::Text("Youtube-DLP Community");
            ImGui::Text("PixelSuft");
            ImGui::TreePop();
        } 
        ImGui::End();   
    }

    void toolsTab::close() {
        fstream file("GDH\\toolstab.data", std::ios::out);
        if (file.is_open()) {
            file << uselessShit::boolToString(PlayLayer::rgbIcons) << "\n";
            file << to_string(uselessShit::overlaysize) << "\n";
            file << uselessShit::boolToString(PlayLayer::hideatt) << "\n";
            file << uselessShit::boolToString(PlayLayer::showallatt) << "\n";
            file << uselessShit::boolToString(PlayLayer::layoutMode) << "\n";
            file << uselessShit::boolToString(DashReplayEngine::onlydashreplay) << "\n";
        }
        file.close();
    }
}