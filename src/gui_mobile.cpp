#include "gui_mobile.hpp"
#include "config.hpp"
#include "hacks.hpp"
#include "labels.hpp"
#include "replayEngine.hpp"
#include "utils.hpp"
#include "recorder.hpp"

#ifdef GEODE_IS_ANDROID
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libavformat/avformat.h>
}

static int encode_video_frame(AVCodecContext *enc_ctx, AVFrame *frame, 
                            AVPacket *pkt, AVFormatContext *fmt_ctx)
{
    int ret;

    // Send frame to encoder
    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        FLAlertLayer::create("Encoding Error", fmt::format("Error sending frame for encoding: {}", av_err2str(ret)), "OK")->show();
        return ret;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return 0;
        else if (ret < 0) {
            FLAlertLayer::create("Encoding Error", fmt::format("Error during encoding: {}", av_err2str(ret)), "OK")->show();
            return ret;
        }

        // Rescale output packet timestamp values from codec to stream timebase
        av_packet_rescale_ts(pkt, enc_ctx->time_base, fmt_ctx->streams[0]->time_base);
        pkt->stream_index = 0;

        // Write packet to file
        ret = av_interleaved_write_frame(fmt_ctx, pkt);
        if (ret < 0) {
            FLAlertLayer::create("File Error", fmt::format("Error writing packet: {}", av_err2str(ret)), "OK")->show();
            return ret;
        }
        av_packet_unref(pkt);
    }

    return 0;
}

int render_video()
{
    auto filefs=folderPath/"test_h264.mp4";
    const char *filename = filefs.c_str();
    int ret;
    AVCodecContext *c = nullptr;
    AVFormatContext *fmt_ctx = nullptr;
    AVFrame *frame = nullptr;
    AVPacket *pkt = nullptr;
    
    // Find x264 encoder
    const AVCodec *codec = avcodec_find_encoder_by_name("libx264");
    if (!codec) {
        FLAlertLayer::create("Codec Error", "Codec libx264 not found", "OK")->show();
        return -1;
    }

    // Allocate codec context
    c = avcodec_alloc_context3(codec);
    if (!c) {
        FLAlertLayer::create("Allocation Error", "Could not allocate video codec context", "OK")->show();
        return -1;
    }

    // Initialize format context
    ret = avformat_alloc_output_context2(&fmt_ctx, NULL, NULL, filename);
    if (!fmt_ctx) {
        FLAlertLayer::create("Format Error", "Could not allocate format context", "OK")->show();
        return 0;
    }

    // Add video stream
    AVStream *stream = avformat_new_stream(fmt_ctx, codec);
    if (!stream) {
        FLAlertLayer::create("Stream Error", "Could not allocate stream", "OK")->show();
        return 0;
    }

    // Set codec parameters
    c->bit_rate = 400000;
    c->width = 352;
    c->height = 288;
    c->time_base = (AVRational){1, 60};
    c->framerate = (AVRational){60, 1};
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    // Set x264 preset
    av_opt_set(c->priv_data, "preset", "medium", 0);

    // Set stream parameters
    stream->time_base = c->time_base;

    // Open codec
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        FLAlertLayer::create("Codec Error", fmt::format("Could not open codec: {}", av_err2str(ret)), "OK")->show();
        return 0;
    }

    // Copy codec parameters to stream
    ret = avcodec_parameters_from_context(stream->codecpar, c);
    if (ret < 0) {
        FLAlertLayer::create("Parameter Error", "Could not copy codec params to stream", "OK")->show();
        return 0;
    }

    // Open output file
    ret = avio_open(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE);
    if (ret < 0) {
        FLAlertLayer::create("File Error", fmt::format("Could not open {}: {}", filename, av_err2str(ret)), "OK")->show();
        return 0;
    }

    // Write header
    ret = avformat_write_header(fmt_ctx, NULL);
    if (ret < 0) {
        FLAlertLayer::create("Format Error", "Error writing header", "OK")->show();
        return 0;
    }

    // Allocate frame
    frame = av_frame_alloc();
    if (!frame) {
        FLAlertLayer::create("Allocation Error", "Could not allocate video frame", "OK")->show();
        return 0;
    }

    frame->format = c->pix_fmt;
    frame->width = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        FLAlertLayer::create("Buffer Error", "Could not allocate frame data", "OK")->show();
        return 0;
    }

    pkt = av_packet_alloc();
    if (!pkt) {
        FLAlertLayer::create("Allocation Error", "Could not allocate packet", "OK")->show();
        return 0;
    }

    // Encode frames
    for (int i = 0; i < 600; i++) {
        ret = av_frame_make_writable(frame);
        if (ret < 0) {
            FLAlertLayer::create("Frame Error", "Could not make frame writable", "OK")->show();
            return 0;
        }

        // Generate synthetic video data
        for (int y = 0; y < c->height; y++) {
            for (int x = 0; x < c->width; x++) {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }
        for (int y = 0; y < c->height/2; y++) {
            for (int x = 0; x < c->width/2; x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }

        frame->pts = i;
        
        ret = encode_video_frame(c, frame, pkt, fmt_ctx);
        if (ret < 0) {
            return 0;
        }
    }

    // Flush encoder
    ret = encode_video_frame(c, NULL, pkt, fmt_ctx);
    if (ret < 0) {
        return 0;
    }

    // Write trailer
    av_write_trailer(fmt_ctx);

    // Success message
    FLAlertLayer::create("Success", fmt::format("Video encoding completed. Output saved to {}", filename), "OK")->show();

    // Clean up
    if (fmt_ctx && fmt_ctx->pb) {
        avio_closep(&fmt_ctx->pb);
    }
    avformat_free_context(fmt_ctx);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    return ret < 0 ? ret : 0;
}
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

CCLabelBMFont* AddTextToToggle(const char *str, CCMenuItemToggler* toggler) {
    auto label = CCLabelBMFont::create(str, "bigFont.fnt");
    label->setAnchorPoint({0.f, 0.5f});
    label->setPosition({toggler->getPositionX() + 15.f, toggler->getPositionY()});
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
                        config.set<bool>("practice_fix", true);
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

            auto exampleButton = ButtonSprite::create("H264 Example", 80, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
            auto exampleButtonClick = CCMenuItemExt::createSpriteExtra(exampleButton, [this](CCMenuItemSpriteExtra* sender) {
                #ifdef GEODE_IS_ANDROID
                render_video();
                #endif
            });
            exampleButtonClick->setPosition({170, 25});
            engineTab->addChild(exampleButtonClick);

            auto justATestButton = ButtonSprite::create("FLVC Test", 80, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.7f);
            auto justATestButtonClick = CCMenuItemExt::createSpriteExtra(justATestButton, [this](CCMenuItemSpriteExtra* sender) {
                auto& recorder = Recorder::get();

                if (recorder.is_recording) {
                    recorder.stop();
                    FLAlertLayer::create("Recorder", "Recording stopped!", "OK")->show();
                    return;
                }

                geode::utils::file::pick(geode::utils::file::PickMode::OpenFolder, {std::nullopt, {}}).listen(
                        [&](geode::Result<std::filesystem::path>* path) {
                    if (!path->isErr()) {
                        auto path_final = path->unwrap();
                        auto pl = PlayLayer::get();
                        if (pl) {
                            recorder.folderShowcasesPath = path_final;
                            
                            recorder.video_name = fmt::format("{}.flvc", pl->m_level->m_levelName);

                            auto size = CCEGLView::get()->getFrameSize();
                            recorder.width = size.width;
                            recorder.height = size.height;

                            if (!recorder.is_recording) {
                                recorder.start("");
                                FLAlertLayer::create("Recorder", fmt::format("Recording started!\nFilename: {}\nPath {}\nSize: {}x{}\nFPS: {}",
                                                recorder.video_name, recorder.folderShowcasesPath, recorder.width, recorder.height, recorder.fps).c_str(), "OK")->show();
                            }

                        }
                        else {
                            FLAlertLayer::create("Recorder", "Not in PlayLayer", "OK")->show();
                        }                        
                    }
                    else {
                        FLAlertLayer::create("Recorder", "Not selected directory", "OK")->show();
                    }
                });
            });
            justATestButtonClick->setPosition({270, 25});
            engineTab->addChild(justATestButtonClick);

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