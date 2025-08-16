#pragma once
#include <string>
#include <cctype>
#include <fstream>
#include "config.hpp"
#include <Geode/binding/TextAlertPopup.hpp>

#ifdef GEODE_IS_WINDOWS
struct WindowStateBackup {
    HWND hwnd;
    LONG Style;
    LONG ExStyle;
    RECT ClientRect;
    POINT WindowPos;
};
#endif

namespace utilsH {
    #ifdef GEODE_IS_WINDOWS
    HWND find_hwnd();
    #endif
    bool isNumeric(const std::string& str);
    void UncompleteLevel();
    void getFolder();
    void setPitchShifter(float pitch);
}


static void unimplemented() {}

static void logMessage(const std::string& message) {
    std::ofstream logFile(folderPath / "h264_encode.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
    logFile.close();
}

static void benchmark(const std::string& name, const std::function<void()>& func, int runs = 1000) {
    using namespace std::chrono;

    long long total_ns = 0;

    for (int i = 0; i < runs; ++i) {
        auto start = high_resolution_clock::now();
        func();
        auto end = high_resolution_clock::now();

        total_ns += duration_cast<nanoseconds>(end - start).count();
    }

    double avg_ns = static_cast<double>(total_ns) / runs;
    double avg_ms = avg_ns / 1000.0 / 1000.0;

    geode::log::debug("[Benchmark] {}: avg = {:.0f} ns | {:.4f} ms ({} runs)\n",
               name, avg_ns, avg_ms, runs);
}

static void toast(const std::string& text, float delay = 1, float scale = 1, int opacity = 127) {
	auto alert = TextAlertPopup::create(text, delay, scale, opacity, "bigFont.fnt");
	auto cs = alert->getChildByType<cocos2d::extension::CCScale9Sprite>(0)->getScaledContentSize();
	alert->setPosition(
		cs.width / 2.f,
		cocos2d::CCDirector::get()->getWinSize().height - cs.height / 2.f
	);

	auto scene = cocos2d::CCScene::get();

	scene->addChild(alert);
}
