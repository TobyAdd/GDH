#include "gui_mobile.hpp"
#include "config.hpp"
#include "hacks.hpp"
#include "labels.hpp"
#include "replayEngine.hpp"
#include "utils.hpp"
#include "recorder.hpp"
#ifdef GEODE_IS_ANDROID
#include "h264_encoder.hpp"
#endif

using namespace geode::prelude;

ReplaySelectLayer* ReplaySelectLayer::create(geode::TextInput* textInput) {
    auto ret = new ReplaySelectLayer();
    if (ret->initAnchored(200.f, 240.f, "GJ_square01.png")) {
        ret->autorelease();
        ret->input = textInput;
        return ret;
    }

    delete ret;
    return nullptr;
}

bool ReplaySelectLayer::setup() {
    auto& engine = ReplayEngine::get();

    this->setTitle("Select Replay");

    auto m_scrollLayer = ScrollLayer::create({400.f, 205.f});
    m_scrollLayer->m_contentLayer->setLayout(
        geode::ColumnLayout::create()
            ->setAutoScale(false)
            ->setAxisReverse(true)
            ->setAutoGrowAxis(m_scrollLayer->getContentHeight())
            ->setAxisAlignment(geode::AxisAlignment::End)
            ->setGap(0)
    );
    m_scrollLayer->setPosition({-100.f, 5.f});
    m_scrollLayer->m_peekLimitTop = 15;
    m_scrollLayer->m_peekLimitBottom = 15;

    for (const auto &entry : std::filesystem::directory_iterator(folderMacroPath)) {
        std::string ext = entry.path().filename().extension().string();
        if (!engine.engine_v2 ? (ext == ".re" || ext == ".re3") : (ext == ".re21" || ext == ".re2")) {
            auto menu = CCMenu::create();
            menu->setContentSize({200, 35});

            auto button = ButtonSprite::create(entry.path().filename().string().c_str(), 120, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
            auto buttonClick = CCMenuItemExt::createSpriteExtra(button, [this, &engine, entry](CCMenuItemSpriteExtra* sender) {
                std::string extension = entry.path().filename().extension().string();
                if (extension != ".re" && extension != ".re2")
                    engine.replay_name = entry.path().filename().replace_extension().string();
                else
                    engine.replay_name = entry.path().filename().string();

                input->setString(engine.replay_name);

                m_closeBtn->activate();
            });
            buttonClick->setPosition({200.f, 10.f});
            menu->addChild(buttonClick);

            m_scrollLayer->m_contentLayer->addChild(menu);

        }
    }

    m_scrollLayer->m_contentLayer->updateLayout();  
    m_scrollLayer->moveToTop();

    m_mainLayer->addChild(m_scrollLayer);

    return true;
}

HacksTab* HacksTab::create() {
    auto ret = new HacksTab();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void HacksTab::addToggle(const std::string& text, const std::string& desc, bool enabled, const std::function<void(bool)>& callback, std::function<void()> handlerCustomWindow) {
    auto toggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(HacksTab::onToggle), 0.75f);

    toggle->setPosition({ 15, 10 });
    toggle->toggle(enabled);

    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.5f);
    label->setPosition({ toggle->getPositionX() + 15.f, 10 });

    auto hack = CCMenu::create();
    hack->setContentSize({325, 25});
    hack->addChild(toggle);
    hack->addChild(label);

    if (!desc.empty()) {
        auto descSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        descSprite->setScale(0.4f);
        auto descClick = CCMenuItemExt::createSpriteExtra(descSprite, [this, desc](CCMenuItemSpriteExtra* sender) {
            FLAlertLayer::create("Desc", desc, "OK")->show();
        });
        descClick->setPosition(label->getPositionX() + label->getScaledContentWidth() + 5.f, label->getScaledContentHeight());
        hack->addChild(descClick);
    }

    if (handlerCustomWindow != nullptr) {
        auto moreSettingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        moreSettingsSprite->setScale(0.5f);
        auto moreSettingsSpriteClick = CCMenuItemExt::createSpriteExtra(moreSettingsSprite, [this, handlerCustomWindow](CCMenuItemSpriteExtra* sender) {
            handlerCustomWindow();
        });
        moreSettingsSpriteClick->setPosition(label->getPositionX() + label->getScaledContentWidth() + 25.f, label->getPositionY());
        hack->addChild(moreSettingsSpriteClick);
    }

    m_scrollLayer->m_contentLayer->addChild(hack);

    m_togglerCallbacks[toggle] = callback;
}

bool HacksTab::init() {
    if (!CCMenu::init())
        return false;

    setPosition({0, 0});

    m_scrollLayer = ScrollLayer::create({325.f, 210.f});
    m_scrollLayer->m_contentLayer->setLayout(
        geode::ColumnLayout::create()
            ->setAutoScale(false)
            ->setAxisReverse(true)
            ->setAutoGrowAxis(m_scrollLayer->getContentHeight())
            ->setAxisAlignment(geode::AxisAlignment::End)
            ->setGap(0)
    );
    m_scrollLayer->setPosition({128.f, 10.f});
    m_scrollLayer->m_peekLimitTop = 15;
    m_scrollLayer->m_peekLimitBottom = 15;
    
    addChild(m_scrollLayer);
    
    return true;
}

void HacksTab::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    if (!toggler) return;

    auto it = m_togglerCallbacks.find(toggler);
    if (it != m_togglerCallbacks.end())
        it->second(!toggler->isToggled());
}

CCLabelBMFont* AddTextToToggle(const char *str, CCMenuItemToggler* toggler, float x_space = 15.f) {
    auto label = CCLabelBMFont::create(str, "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setPosition({toggler->getPositionX() + x_space, toggler->getPositionY()});
    label->setScale(0.5f);
    return label;
}

bool HacksLayer::setup() {
    auto& config = Config::get();
    auto& hacks = Hacks::get();

    this->setTitle("GDH");

    auto background = extension::CCScale9Sprite::create("square02_small.png");
    background->setPosition({290.f, 115.f});
    background->setContentSize({325.f, 210.f});
    background->setOpacity(100);
    m_mainLayer->addChild(background);

    int i = 0;
    float tabPositionY = 205.f;

    for (auto& win : hacks.m_windows) {
        if (win.name == "Framerate" || win.name == "GDH Settings" || win.name == "Variables" || win.name == "Shortcuts")
            continue;

        auto button = ButtonSprite::create(win.name.c_str(), 90, true, "bigFont.fnt", (i == m_index) ? "GJ_button_02.png" : "GJ_button_01.png", 30.f, 0.7f);

        auto buttonClick = CCMenuItemExt::createSpriteExtra(button, [this, i](CCMenuItemSpriteExtra* sender) {
            switchTab(i);
        });
        buttonClick->setPosition({65.f, tabPositionY});
        m_buttonTabs.push_back(buttonClick);
        m_buttonMenu->addChild(buttonClick);

        auto tab = HacksTab::create();
        tab->setVisible(i == 0);
        tab->setID(fmt::format("{}"_spr, win.name));
        m_mainLayer->addChild(tab);

        if (win.name == "Replay Engine") {
            auto& engine = ReplayEngine::get();
            auto engineTab = CCMenu::create();
            engineTab->setContentSize({325, 210});

            auto info_label = CCLabelBMFont::create(fmt::format("Frame: {}\nReplay Size: {}", engine.get_frame(), engine.get_actions_size()).c_str(), "chatFont.fnt");
            info_label->setAnchorPoint({0.f, 0.5f});
            info_label->setPosition({8, 15});
            info_label->setScale(0.5f);
            engineTab->addChild(info_label);

            record_toggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, &config, &engine, info_label](CCMenuItemToggler* sender) {
                play_toggle->toggle(false);
                if (!sender->isOn()) {
                    bool canRecord = (!engine.engine_v2 && config.get<bool>("tps_enabled", false)) || (engine.engine_v2 && !config.get<bool>("tps_enabled", false));
                    
                    if (canRecord)
                    {
                        engine.clear();
                        engine.mode = state::record;
                        info_label->setString(fmt::format("Frame: {}\nReplay Size: {}/{}", engine.get_frame(), engine.get_current_index(), engine.get_actions_size()).c_str());
                    }
                    else
                    {
                        sender->toggle(true);
                        engine.mode = state::disable;
                        FLAlertLayer::create("Replay Engine", engine.engine_v2 ? "Disable TPS Bypass to record the replay" : "Enable TPS Bypass to record the replay", "OK")->show();
                    }
                }
                else {
                    engine.mode = state::disable;
                }
            });
            record_toggle->setPosition({20.f, 190.f});
            if (engine.mode == state::record) record_toggle->toggle(true);
            engineTab->addChild(record_toggle);

            auto record_label = AddTextToToggle("Record", record_toggle);
            engineTab->addChild(record_label);

            play_toggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, &config, &engine](CCMenuItemToggler* sender) {
                record_toggle->toggle(false);
                if (!play_toggle->isOn()) {
                    bool canPlay = (!engine.engine_v2 && config.get<bool>("tps_enabled", false)) || (engine.engine_v2 && !config.get<bool>("tps_enabled", false));
                    
                    if (canPlay) {
                        engine.mode = state::play;
                    }
                    else
                    {
                        sender->toggle(true);
                        engine.mode = state::disable;
                        FLAlertLayer::create("Replay Engine", engine.engine_v2 ? "Disable TPS Bypass to playback the replay" : "Enable TPS Bypass to playback the replay", "OK")->show();
                    }
                }
                else {
                    engine.mode = state::disable;
                }
            });
            play_toggle->setPosition({record_label->getPositionX() + record_label->getScaledContentWidth() + 20.f, 190.f});
            if (engine.mode == state::play) play_toggle->toggle(true);
            engineTab->addChild(play_toggle);

            auto play_label = AddTextToToggle("Play", play_toggle);
            engineTab->addChild(play_label);

            TextInput* replay_name_input = TextInput::create(220, "Replay Name", "chatFont.fnt");
            replay_name_input->setPosition({118.f, 155.f});
            replay_name_input->setString(engine.replay_name);
            replay_name_input->setCallback([&engine](const std::string& text) {
                engine.replay_name = text;
            });
            engineTab->addChild(replay_name_input);

            auto replayList = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
            replayList->setScale(0.65f);
            replayList->setRotation(-90.f);
            auto replayListClick = CCMenuItemExt::createSpriteExtra(replayList, [this, replay_name_input](CCMenuItemSpriteExtra* sender) {
                ReplaySelectLayer::create(replay_name_input)->show();
            });
            replayListClick->setPosition({255.f, 155.f});
            engineTab->addChild(replayListClick);

            auto saveButton = ButtonSprite::create("Save", 40, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
            auto saveButtonClick = CCMenuItemExt::createSpriteExtra(saveButton, [this, &engine, info_label](CCMenuItemSpriteExtra* sender) {
                FLAlertLayer::create("Info", engine.save(engine.replay_name).c_str(), "OK")->show();
            });
            saveButtonClick->setPosition({35, 120});
            engineTab->addChild(saveButtonClick);

            auto loadButton = ButtonSprite::create("Load", 40, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
            auto loadButtonClick = CCMenuItemExt::createSpriteExtra(loadButton, [this, &engine, info_label](CCMenuItemSpriteExtra* sender) {
                FLAlertLayer::create("Info", engine.load(engine.replay_name).c_str(), "OK")->show();
                info_label->setString(fmt::format("Frame: {}\nReplay Size: {}/{}", engine.get_frame(), engine.get_current_index(), engine.get_actions_size()).c_str());
            });
            loadButtonClick->setPosition({95, 120});
            engineTab->addChild(loadButtonClick);

            auto cleanButton = ButtonSprite::create("Clear", 40, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
            auto cleanButtonClick = CCMenuItemExt::createSpriteExtra(cleanButton, [this, &engine, info_label](CCMenuItemSpriteExtra* sender) {
                engine.clear();
                FLAlertLayer::create("Info", "Replay has been cleared", "OK")->show();
                info_label->setString(fmt::format("Frame: {}\nReplay Size: {}/{}", engine.get_frame(), engine.get_current_index(), engine.get_actions_size()).c_str());
            });
            cleanButtonClick->setPosition({155, 120});
            engineTab->addChild(cleanButtonClick);

            auto accuracy_fix_toggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, &config, &engine](CCMenuItemToggler* sender) {
                engine.accuracy_fix = !sender->isOn();
            });
            accuracy_fix_toggle->toggle(engine.accuracy_fix);
            accuracy_fix_toggle->setPosition({20.f, 85.f});
            engineTab->addChild(accuracy_fix_toggle);

            auto accuracy_fix_label = AddTextToToggle("Accuracy Fix", accuracy_fix_toggle);
            engineTab->addChild(accuracy_fix_label);

            auto rotation_fix_toggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, &config, &engine](CCMenuItemToggler* sender) {
                engine.rotation_fix = !sender->isOn();
            });
            rotation_fix_toggle->toggle(engine.rotation_fix);
            rotation_fix_toggle->setPosition({170.f, 85.f});
            engineTab->addChild(rotation_fix_toggle);

            auto rotation_fix_label = AddTextToToggle("Rotation Fix", rotation_fix_toggle);
            engineTab->addChild(rotation_fix_label);
            
            auto engineV2_toggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, &config, &engine](CCMenuItemToggler* sender) {
                engine.engine_v2 = !sender->isOn();
                config.set<bool>("engine::v2", !sender->isOn());
            });
            engineV2_toggle->setPosition({20.f, 55.f});
            engineV2_toggle->toggle(engine.engine_v2);
            engineTab->addChild(engineV2_toggle);

            auto engineV2_label = AddTextToToggle("Engine v2.1 (Beta)", engineV2_toggle);
            engineTab->addChild(engineV2_label);

            auto recorderButton = ButtonSprite::create("Recorder", 80, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
            auto recorderButtonClick = CCMenuItemExt::createSpriteExtra(recorderButton, [this](CCMenuItemSpriteExtra* sender) {
                RecorderLayer::create()->show();
            });
            recorderButtonClick->setPosition({170, 25});
            engineTab->addChild(recorderButtonClick);

            auto recorderAudioButton = ButtonSprite::create("Audio", 80, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
            auto recorderAudioButtonClick = CCMenuItemExt::createSpriteExtra(recorderAudioButton, [this](CCMenuItemSpriteExtra* sender) {
                RecorderAudioLayer::create()->show();
            });
            recorderAudioButtonClick->setPosition({270, 25});
            engineTab->addChild(recorderAudioButtonClick);

            auto mixerButton = ButtonSprite::create("Mixer", 80, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
            auto mixerButtonButtonClick = CCMenuItemExt::createSpriteExtra(mixerButton, [this](CCMenuItemSpriteExtra* sender) {

            });
            mixerButtonButtonClick->setPosition({270, 60});
            engineTab->addChild(mixerButtonButtonClick);

            tab->m_scrollLayer->m_contentLayer->addChild(engineTab);
        }
        else if (win.name == "Labels") {
            auto& labels = Labels::get();
            labels.initMobileContext(tab->m_scrollLayer);
        }

        for (auto& hck : win.hacks) {
            tab->addToggle(hck.name, hck.desc, config.get<bool>(hck.config, false), [&config, &hck](bool enabled) {
                config.set(hck.config, enabled);
                if (!hck.game_var.empty())
                    GameManager::get()->setGameVariable(hck.game_var.c_str(), enabled);
                if (hck.handlerFunc) hck.handlerFunc(enabled);
            }, hck.handlerCustomWindow);          
        }

        tab->m_scrollLayer->m_contentLayer->updateLayout();
        tab->m_scrollLayer->moveToTop();

        m_tabs.push_back(tab);

        tabPositionY -= 35;
        i++;
    }

    return true;
}

void HacksLayer::switchTab(int newIndex) {
    if (newIndex < 0 || newIndex >= m_tabs.size()) return;

    m_index = newIndex;

    for (int i = 0; i < m_tabs.size(); i++) {
        m_tabs[i]->setVisible(i == m_index);
    }

    for (int i = 0; i < m_buttonTabs.size(); i++) {
        auto* button = m_buttonTabs[i];
        if (button) {
            auto* btnSprite = static_cast<ButtonSprite*>(button->getChildren()->objectAtIndex(0));
            if (btnSprite) {
                btnSprite->updateBGImage(i == m_index ? "GJ_button_02.png" : "GJ_button_01.png");
            }
        }
    }
}

HacksLayer* HacksLayer::create() {
    auto ret = new HacksLayer();
    if (ret->initAnchored(460.f, 260.f, "GJ_square01.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void HacksLayer::onExit() {
    Config::get().save(fileDataPath);
    Labels::get().save();
    RGBIcons::get().save();
    geode::Popup<>::onExit();
}

RecorderLayer* RecorderLayer::create() {
    auto ret = new RecorderLayer();
    if (ret->initAnchored(360.f, 240.f, "GJ_square01.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool RecorderLayer::setup() {
    auto& recorder = Recorder::get();
    this->setTitle("Recorder");

    auto menu = CCMenu::create();
    menu->setPosition({0, 0});

    auto createTextInput = [&menu, &recorder](const cocos2d::CCPoint& position, const std::string& filter, const std::string& initial_value, float width, const std::function<void(const std::string&)>& callback) {
        TextInput* input = TextInput::create(width, "Input", "chatFont.fnt");
        input->setFilter(filter);
        input->setPosition(position);
        input->setString(initial_value);
        input->setCallback(callback);
        menu->addChild(input);
        return input;
    };

    auto createLabel = [&menu](const std::string& text, const cocos2d::CCPoint& position) {
        auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition(position);
        label->setScale(0.5f);
        menu->addChild(label);
        return label;
    };

    CCMenuItemToggler* record_toggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, &recorder](CCMenuItemToggler* sender) {
        auto pl = PlayLayer::get();
        auto& config = Config::get();
        auto& engine = ReplayEngine::get();

        recorder.enabled = !sender->isOn();

        if (pl && pl->m_hasCompletedLevel) {
            FLAlertLayer::create("Recorder", "Restart level to start recording", "OK");
            recorder.enabled = false;
            sender->toggle(true);
        }
        else {                                    
            bool fps_enabled = config.get<bool>("tps_enabled", false);
            bool check = recorder.fps <= config.get<float>("tps_value", 60.f);
            bool check2 = recorder.fps >= 60 && recorder.fps <= 240;        
            if (engine.engine_v2 ? (!fps_enabled && check2) : (fps_enabled && check)) {
                if (recorder.enabled) {
                    geode::utils::file::pick(geode::utils::file::PickMode::OpenFolder, {std::nullopt, {}}).listen(
                            [&](geode::Result<std::filesystem::path>* path) {
                        if (!path->isErr()) {
                            auto path_final = path->unwrap();

                            recorder.folderShowcasesPath = path_final;
                            
                            recorder.video_name = fmt::format("{}.{}", recorder.video_name2, recorder.flvc_enabled ? "flvc" : "mp4");

                            if (!recorder.is_recording) {
                                recorder.start("");
                                FLAlertLayer::create("Recorder", fmt::format("Recording started!\nFilename: {}\nPath {}\nSize: {}x{}\nFPS: {}",
                                                recorder.video_name, recorder.folderShowcasesPath, recorder.width, recorder.height, recorder.fps), "OK")->show();
                            }                   
                        }
                        else {
                            FLAlertLayer::create("Recorder", "Not selected directory", "OK")->show();
                            sender->toggle(false);
                        }
                    });
                }                        
                else {
                    recorder.stop();
                    FLAlertLayer::create("Recorder", "Recording stopped!", "OK")->show();
                }
            }
            else {
                recorder.enabled = false;
                sender->toggle(true);
                if (engine.engine_v2) {
                    if (fps_enabled)
                        FLAlertLayer::create("Recorder", "Disable TPS Bypass to start render", "OK")->show();

                    if (!check2)
                        FLAlertLayer::create("Recorder", "Recorder FPS values must be within the range 60 to 240", "OK")->show();
                }
                else {
                    recorder.enabled = false;
                    sender->toggle(true);

                    if (!fps_enabled)
                        FLAlertLayer::create("Recorder", "Enable TPS Bypass to start render", "OK")->show();

                    if (!check)
                        FLAlertLayer::create("Recorder", "Recorder FPS should not be more than the current TPS", "OK")->show();
                }

            }
        }
    });
    record_toggle->toggle(recorder.enabled);
    record_toggle->setPosition({25.f, 190.f});
    menu->addChild(record_toggle);
    auto record_label = AddTextToToggle("Start Record", record_toggle);
    menu->addChild(record_label);

    createTextInput({260.f, 190.f}, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", recorder.video_name2, 180.f, [&recorder](const std::string& text) {
        if (utilsH::isNumeric(text)) {
            recorder.video_name2 = text;
        }
    });

    createLabel("Resolution:", {15.f, 165.f});
    auto width_input = createTextInput({35.f, 140.f}, "1234567890", std::to_string(recorder.width), 40.f, [&recorder](const std::string& text) {
        if (utilsH::isNumeric(text)) {
            recorder.width = std::stoi(text);
        }
    });
    createLabel("x", {57.f, 140.f});
    auto height_input = createTextInput({92.f, 140.f}, "1234567890", std::to_string(recorder.height), 40.f, [&recorder](const std::string& text) {
        if (utilsH::isNumeric(text)) {
            recorder.height = std::stoi(text);
        }
    });
    createLabel("@", {115.f, 140.f});
    auto fps_input = createTextInput({152.f, 140.f}, "1234567890", std::to_string(recorder.fps), 40.f, [&recorder](const std::string& text) {
        if (utilsH::isNumeric(text)) {
            recorder.fps = std::stoi(text);
        }
    });

    // auto fullSizeButton = ButtonSprite::create("Full Size", 60, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
    // auto fullSizeButtonClick = CCMenuItemExt::createSpriteExtra(fullSizeButton, [this, &recorder, width_input, height_input](CCMenuItemSpriteExtra* sender) {
    //     auto size = CCEGLView::get()->getFrameSize();
    //     recorder.width = size.width;
    //     recorder.height = size.height;
    //     width_input->setString(std::to_string(recorder.width));
    //     height_input->setString(std::to_string(recorder.height));
    // });
    // fullSizeButtonClick->setPosition({215, 140});
    // menu->addChild(fullSizeButtonClick);

    createLabel("Encoder Settings:", {15.f, 110.f});
    auto bitrate_input = createTextInput({35.f, 85.f}, "1234567890M", recorder.bitrate, 40.f, [&recorder](const std::string& text) {
        recorder.bitrate = text;
    });

    CCMenuItemToggler* flvc_toggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, &recorder](CCMenuItemToggler* sender) { 
        recorder.flvc_enabled = !sender->isOn();
    });
    flvc_toggle->setPosition({75.f, 85.f});
    flvc_toggle->toggle(recorder.flvc_enabled);
    menu->addChild(flvc_toggle);
    auto flvc_label = AddTextToToggle("FLVC", flvc_toggle);
    menu->addChild(flvc_label);

    CCMenuItemToggler* hide_level_complete_toggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, &recorder](CCMenuItemToggler* sender) {
        recorder.hide_level_complete = !sender->isOn();
    });
    hide_level_complete_toggle->setPosition({150.f, 85.f});
    hide_level_complete_toggle->toggle(recorder.hide_level_complete);
    menu->addChild(hide_level_complete_toggle);
    auto hide_level_complete_label = AddTextToToggle("Hide Level Complete", hide_level_complete_toggle);
    menu->addChild(hide_level_complete_label);

    createLabel("Level Settings:", {15.f, 55.f});
    auto seconds_to_render_after_input = createTextInput({35.f, 30.f}, "1234567890.", std::format("{:.2f}", recorder.after_end_duration), 40.f, [&recorder](const std::string& text) {
        if (utilsH::isNumeric(text)) {
            recorder.after_end_duration = std::stof(text);
        }
    });
    auto seconds_to_render_after_label = createLabel("Seconds to render after", {65.f, 30.f});

    m_mainLayer->addChild(menu);
    return true;
}

RecorderAudioLayer* RecorderAudioLayer::create() {
    auto ret = new RecorderAudioLayer();
    if (ret->initAnchored(360.f, 240.f, "GJ_square01.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool RecorderAudioLayer::setup() {
    auto& recorder = Recorder::get();
    auto& recorderAudio = RecorderAudio::get();
    this->setTitle("Recorder (Audio)");

    auto menu = CCMenu::create();
    menu->setPosition({0, 0});

    auto createTextInput = [&menu, &recorderAudio](const cocos2d::CCPoint& position, const std::string& filter, const std::string& initial_value, float width, const std::function<void(const std::string&)>& callback) {
        TextInput* input = TextInput::create(width, "Input", "chatFont.fnt");
        input->setFilter(filter);
        input->setPosition(position);
        input->setString(initial_value);
        input->setCallback(callback);
        menu->addChild(input);
        return input;
    };

    auto createLabel = [&menu](const std::string& text, const cocos2d::CCPoint& position) {
        auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition(position);
        label->setScale(0.5f);
        menu->addChild(label);
        return label;
    };

    CCMenuItemToggler* record_toggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, &recorder, &recorderAudio](CCMenuItemToggler* sender) {
        recorderAudio.enabled = !sender->isOn();

        if (recorderAudio.enabled) {
            geode::utils::file::pick(geode::utils::file::PickMode::OpenFolder, {std::nullopt, {}}).listen(
                    [&](geode::Result<std::filesystem::path>* path) {
                if (!path->isErr()) {
                    auto path_final = path->unwrap();
                    recorder.folderShowcasesPath = path_final;                        
                    recorderAudio.audio_name = fmt::format("{}.wav", recorderAudio.audio_name2);

                        if (!recorderAudio.showcase_mode) {
                            recorderAudio.start();
                        }
                        else
                            recorderAudio.first_start = true;

                        FLAlertLayer::create("Recorder (Audio)", 
                            recorderAudio.showcase_mode ? "Rendering begins when you re-enter the level (don't forget to set up the macro playback!!)" :
                                                        "Audio recording started!", "OK")->show();             
                }
                else {
                    FLAlertLayer::create("Recorder (Audio)", "Not selected directory", "OK")->show();
                    sender->toggle(false);
                }
            });
        }                        
        else {            
            recorderAudio.stop();
            FLAlertLayer::create("Recorder (Audio)", "Audio recording stoped!", "OK")->show();
        }
    });
    record_toggle->toggle(recorderAudio.enabled);
    record_toggle->setPosition({25.f, 190.f});
    menu->addChild(record_toggle);
    auto record_label = AddTextToToggle("Start Record", record_toggle);
    menu->addChild(record_label);

    CCMenuItemToggler* showcaseMode_toggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [this, &recorderAudio](CCMenuItemToggler* sender) {
        recorderAudio.showcase_mode = !sender->isOn();
    });
    showcaseMode_toggle->toggle(recorderAudio.showcase_mode);
    showcaseMode_toggle->setPosition({25.f, 160.f});
    menu->addChild(showcaseMode_toggle);
    auto showcaseMode_label = AddTextToToggle("Showcase Mode", showcaseMode_toggle);
    menu->addChild(showcaseMode_label);

    createTextInput({260.f, 190.f}, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", recorderAudio.audio_name2, 180.f, [&recorderAudio](const std::string& text) {
        if (utilsH::isNumeric(text)) {
            recorderAudio.audio_name2 = text;
        }
    });

    createLabel("Level Settings:", {15.f, 55.f});
    auto seconds_to_render_after_input = createTextInput({35.f, 30.f}, "1234567890.", std::format("{:.2f}", recorderAudio.after_end_duration), 40.f, [&recorderAudio](const std::string& text) {
        if (utilsH::isNumeric(text)) {
            recorderAudio.after_end_duration = std::stof(text);
        }
    });
    auto seconds_to_render_after_label = createLabel("Seconds to render after", {65.f, 30.f});

    m_mainLayer->addChild(menu);
    return true;
}