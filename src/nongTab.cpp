#include "nongTab.h"

bool nongTab::nongTabInit = false;
char nongTab::youtubelink[256] = "https://youtu.be/dQw4w9WgXcQ";
int nongTab::songid = 228;
vector<string> nongTab::nongConfig;
string nongTab::nameofgd;


namespace nongTab {
    void nongTab::init() {
        string line;
        fstream fileConfig("GDH\\nong.data", std::ios::in);
        if (fileConfig.is_open()) {
            getline(fileConfig, line);
            strcpy(youtubelink, line.c_str()); 
            getline(fileConfig, line);
            songid = stoi(line);
            getline(fileConfig, line);
            nongConfig.push_back(string(youtubelink));
            nongConfig.push_back(to_string(songid));
        }

        TCHAR szExeFileName[MAX_PATH]; 
        GetModuleFileName(NULL, szExeFileName, MAX_PATH);
        nameofgd = regex_replace(uselessShit::GetFilenameFromPath(string(szExeFileName)), std::regex(".exe"), "");
    }

    void nongTab::draw() {
        ImGui::Begin("Nong Downloader", 0, ImGuiWindowFlags_NoCollapse);
        if (!nongTabInit) {
            ImGui::SetWindowPos(ImVec2(610 * uselessShit::overlaysize, 320 * uselessShit::overlaysize));
            ImGui::SetWindowSize(ImVec2(190 * uselessShit::overlaysize, 180 * uselessShit::overlaysize));
            ImGui::SetWindowFontScale(uselessShit::overlaysize);
            nongTabInit = true;
        }    
        ImGui::PushItemWidth(175.f * uselessShit::overlaysize);
        ImGui::Text("Youtube link:");
        ImGui::InputText("##youtubelink", youtubelink, 256);
        ImGui::Text("Song ID:");
        ImGui::PushItemWidth(175.f * uselessShit::overlaysize);
        ImGui::InputInt("##songid", &songid);
        if (ImGui::Button("Download", { 175.f * uselessShit::overlaysize, 22.f * uselessShit::overlaysize})) {
			std::string command = "/c GDH\\nong\\downloader.bat " + to_string(songid) + " \"" + string(youtubelink) + "\" " + nameofgd + " & exit";
            ShellExecuteA(0, "open", "cmd", command.c_str(), NULL, SW_SHOWNORMAL);
        }
        if (ImGui::Button("Delete Audio", { 175.f * uselessShit::overlaysize, 22.f * uselessShit::overlaysize})) {
			char* sAppDataPath; size_t iAppDataPathLength;
			_dupenv_s(&sAppDataPath, &iAppDataPathLength, "LOCALAPPDATA");
			std::string stdAppData(sAppDataPath);
			std::string delcommand = stdAppData + '\\' + nameofgd + '\\' + to_string(songid)  + ".mp3";
			std::filesystem::remove(delcommand.c_str());
        }
        ImGui::End();
    }

    void nongTab::close() {
        nongConfig.clear();
        nongConfig.push_back(string(youtubelink));
        nongConfig.push_back(to_string(songid));
        fstream file("GDH\\nong.data", std::ios::out);
        if (file.is_open()) {
            for (int i = 0; i < (int)nongConfig.size(); i++) {
                file << nongConfig[i] << "\n";
            }
        }
        file.close();
    }
}