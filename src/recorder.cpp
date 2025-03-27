#include "recorder.hpp"
#include "hacks.hpp"
#include "replayEngine.hpp"
#include <Geode/modify/ShaderLayer.hpp>
#include "gui.hpp"
#include <imgui-cocos.hpp>
#ifdef GEODE_IS_WINDOWS
#include <subprocess.hpp>
#endif
#include "flvc.hpp"
#ifdef GEODE_IS_ANDROID64
#include "h264_encoder.hpp"
#endif
#include "utils.hpp"

void RenderTexture::begin() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
}

void RenderTexture::capture_frame(std::mutex& lock, std::vector<uint8_t>& data, volatile bool& frame_has_data) {
    auto& recorder = Recorder::get();

    if (recorder.overlay_mode) {
        lock.lock();
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.data());
        frame_has_data = true;
        lock.unlock();
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glViewport(0, 0, width, height);

    auto director = cocos2d::CCDirector::get();
    auto scene = PlayLayer::get();

    #ifdef GEODE_IS_ANDROID
    scene->setScaleY(-1);
    #endif
    scene->visit();
    #ifdef GEODE_IS_ANDROID
    scene->setScaleY(1);
    #endif

    lock.lock();
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    frame_has_data = true;
    lock.unlock();

    glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);

    director->setViewport();
}

void RenderTexture::end() {
    if (textureId) {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }
    if (fbo) {
        glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }
}

void Recorder::applyWinSize() {
    auto pl = PlayLayer::get();
    if (pl && pl->m_isPaused) return;
    if (newDesignResolution.width != 0 && newDesignResolution.height != 0) {    
        cocos2d::CCDirector::get()->m_obWinSizeInPoints = newDesignResolution;        
        auto view = cocos2d::CCEGLView::get();
        view->setDesignResolutionSize(newDesignResolution.width, newDesignResolution.height, ResolutionPolicy::kResolutionExactFit);
        view->m_fScaleX = newScreenScale.width;
        view->m_fScaleY = newScreenScale.height;
    }
}

void Recorder::restoreWinSize() {
    if (oldDesignResolution.width != 0 && oldDesignResolution.height != 0) {
        cocos2d::CCDirector::get()->m_obWinSizeInPoints = oldDesignResolution;
        auto view = cocos2d::CCEGLView::get();
        view->setDesignResolutionSize(oldDesignResolution.width, oldDesignResolution.height, ResolutionPolicy::kResolutionExactFit);
        view->m_fScaleX = originalScreenScale.width;
        view->m_fScaleY = originalScreenScale.height;
    }
}

void Recorder::start(std::string command) {
    bool use_flvc = video_name.ends_with(".flvc");

    is_recording = true;

    need_remove_black = false;
    need_visible_lc = false;

    last_frame_time = extra_time = 0;

    after_end_extra_time = 0.f;

    auto view = cocos2d::CCEGLView::get();
    
    oldDesignResolution = view->getDesignResolutionSize();
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    newDesignResolution = cocos2d::CCSize(roundf(320.f * aspectRatio), 320.f);

    originalScreenScale = cocos2d::CCSize(view->m_fScaleX, view->m_fScaleY);
    newScreenScale = cocos2d::CCSize(static_cast<float>(width) / newDesignResolution.width, static_cast<float>(height) / newDesignResolution.height);

    frame_has_data = false;
    current_frame.resize(width * height * 3, 0);
    is_recording = true;
    texture.height = height;
    texture.width = width;
    texture.begin();

    std::thread([&, command, use_flvc] {
        if (use_flvc) {
            FLVCEncoder encoder(width, height, fps, folderShowcasesPath / video_name);
            while (is_recording || frame_has_data) {
                lock.lock();
                if (frame_has_data) {
                    const auto frame = current_frame;
                    frame_has_data = false;
                    encoder.writeFrame(frame);
                }
                lock.unlock();
            }
            encoder.close();
        } else {
            #ifdef GEODE_IS_WINDOWS
            auto process = subprocess::Popen(command);
            while (is_recording || frame_has_data) {
                lock.lock();
                if (frame_has_data) {
                    const auto frame = current_frame;
                    frame_has_data = false;
                    process.m_stdin.write(frame.data(), frame.size());
                }
                lock.unlock();
            }
            if (process.close()) {
                return;
            }
            #elif defined(GEODE_IS_ANDROID64) 
            auto convert_bitrate = [](const std::string& str) -> int64_t {
                size_t len = str.length();                
                if (len == 0) return 0;                
                bool isMegabit = (str[len - 1] == 'M' || str[len - 1] == 'm');                
                std::string num_str = isMegabit ? str.substr(0, len - 1) : str;

                if (utilsH::isNumeric(num_str)) {
                    return std::stoll(num_str) * (isMegabit ? 1000000 : 1);
                }

                return 0;                
            };

            H264Encoder encoder(width, height, fps, convert_bitrate(bitrate), folderShowcasesPath / video_name);
            while (is_recording || frame_has_data) {
                lock.lock();
                if (frame_has_data) {
                    const auto frame = current_frame;
                    frame_has_data = false;
                    encoder.writeFrame(frame);
                }
                lock.unlock();
            }
            encoder.finish();
            #endif
        }
    }).detach();
}

void Recorder::stop() {
    texture.end();    

    is_recording = false;
    enabled = false;
    
    auto pl = PlayLayer::get();
    
    if (pl && fade_out) {
        need_remove_black = true;
    }

    if (pl && hide_level_complete) {
        need_visible_lc = true;
    }

    needRevertOld = true;
    #ifdef GEODE_IS_WINDOWS
    ImGuiH::Popup::get().add_popup("Video recording stoped!");
    #elif defined(GEODE_IS_ANDROID64) 
    geode::queueInMainThread([] {
        FLAlertLayer::create("Recorder", "Video recording stoped!", "OK")->show();
    });
    #endif
    
}

void Recorder::render_frame() {
    while (frame_has_data) {}
    texture.capture_frame(lock, current_frame, frame_has_data);
}

void Recorder::handle_recording(float dt) {
    auto playLayer = PlayLayer::get();
    if (!playLayer->m_hasCompletedLevel || after_end_extra_time < after_end_duration) {
        if (playLayer->m_hasCompletedLevel) {
            after_end_extra_time += dt;
        }

        double frame_dt = 1.0 / static_cast<double>(fps);
        double time = (playLayer->m_gameState.m_levelTime - delay) + extra_time - last_frame_time;
        if (time >= frame_dt) {
            auto fmod = FMODAudioEngine::get();
            auto offset = (playLayer->m_levelSettings->m_songOffset + playLayer->m_gameState.m_levelTime) * 1000.0;
            fmod->setMusicTimeMS(offset, false, 0);

            // playLayer->startMusic();
            extra_time = time - frame_dt;
            last_frame_time = (playLayer->m_gameState.m_levelTime - delay);
            render_frame();
        }
    }
    else {
        stop();
    }   
}

std::string Recorder::compile_command() {
    std::string command = fmt::format("ffmpeg.exe -y -f rawvideo -pix_fmt rgb24 -s {}x{} -r {} -i -", width, height, fps);

    if (!codec.empty()) {
        command += fmt::format(" -c:v {}", codec);
    }

    if (!bitrate.empty()) {
        command += fmt::format(" -b:v {}", bitrate);
    }

    if (!extra_args.empty()) {
        command += fmt::format(" {}", extra_args);
    }
    else {
        command += " -pix_fmt yuv420p";
    }
    
    if (!vf_args.empty())
        command += fmt::format(" -vf {}", vf_args);

    command += fmt::format(" -an \"{}\\{}\"", folderShowcasesPath, video_name);

    return command;
}

void Recorder::compile_vf_args() {
    vf_args = "";
    if (vflip) {
        vf_args += "\"vflip\"";
    }

    if (fade_in) {
        if (vflip) {
            vf_args += ",";
        }

        vf_args += fmt::format("\"fade=t=in:st={}:d={}\"", fade_in_start, fade_in_end);
    }
}

void RecorderAudio::init() {
    auto system = FMODAudioEngine::get()->m_system;

    FMOD_DSP_DESCRIPTION desc = {};
    strcpy(desc.name, "DSP Recorder");
    desc.numinputbuffers = 1;
    desc.numoutputbuffers = 1;
    desc.read = [](FMOD_DSP_STATE* dspState, float* inBuffer, float* outBuffer, unsigned int length, int inChannels, int* outChannels) {
        auto recorder = &RecorderAudio::get();
        if (!recorder->is_recording) return FMOD_OK;

        auto channels = *outChannels;
        {
            std::lock_guard<std::mutex> lock(recorder->m_data_mutex);
            recorder->m_data.insert(recorder->m_data.end(), inBuffer, inBuffer + length * channels);
        }

        std::memcpy(outBuffer, inBuffer, length * channels * sizeof(float));

        return FMOD_OK;
    };

    system->createDSP(&desc, &m_dsp);
    system->getMasterChannelGroup(&m_masterGroup);
}

void RecorderAudio::start() {
    if (is_recording) return;

    init();
    
    is_recording = true;
    after_end_extra_time = 0;

    auto fmod_engine = FMODAudioEngine::get();
    old_volume_music = fmod_engine->getBackgroundMusicVolume();
    old_volume_sfx = fmod_engine->getEffectsVolume();

    fmod_engine->setBackgroundMusicVolume(volume_music);
    fmod_engine->setEffectsVolume(volume_sfx);

    m_masterGroup->addDSP(0, m_dsp);

    {
        std::lock_guard<std::mutex> lock(m_data_mutex);
        m_data.clear();
    }
}

void RecorderAudio::stop() {
    if (!is_recording) return;

    enabled = false;
    is_recording = false;
    want_to_stop = false;

    m_masterGroup->removeDSP(m_dsp);

    auto fmod_engine = FMODAudioEngine::get();
    fmod_engine->setBackgroundMusicVolume(old_volume_music);
    fmod_engine->setEffectsVolume(old_volume_sfx);

    #ifdef GEODE_IS_WINDOWS
    ImGuiH::Popup::get().add_popup("Audio recording stopped!");
    #elif defined(GEODE_IS_ANDROID64) 
    geode::queueInMainThread([] {
        FLAlertLayer::create("Recorder", "Audio recording stopped!", "OK")->show();
    });
    #endif

    save_to_wav(Recorder::get().folderShowcasesPath / audio_name);
}

std::vector<float> RecorderAudio::get_data() {
    std::lock_guard<std::mutex> lock(m_data_mutex);
    auto data = m_data;
    m_data.clear();
    return data;
}

void RecorderAudio::handle_recording(float dt) {
    auto playLayer = GameManager::sharedState()->getPlayLayer();
    if (showcase_mode) {
        if (!playLayer->m_hasCompletedLevel || after_end_extra_time < after_end_duration) {
            if (playLayer->m_hasCompletedLevel) {
                after_end_extra_time += dt;
            }
        } else {
            stop();
        }
    }
}

void RecorderAudio::save_to_wav(const std::filesystem::path& filename) {
    std::lock_guard<std::mutex> lock(m_data_mutex);
    if (m_data.empty()) return;

    int sampleRate, channels;
    FMODAudioEngine::get()->m_system->getSoftwareFormat(&sampleRate, nullptr, &channels);

    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) return;

    struct WavHeader {
        char chunkId[4] = {'R', 'I', 'F', 'F'};
        uint32_t chunkSize;
        char format[4] = {'W', 'A', 'V', 'E'};
        char subchunk1Id[4] = {'f', 'm', 't', ' '};
        uint32_t subchunk1Size = 16;
        uint16_t audioFormat = 3;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample = 32;
        char subchunk2Id[4] = {'d', 'a', 't', 'a'};
        uint32_t subchunk2Size;
    };

    WavHeader header;
    header.numChannels = static_cast<uint16_t>(channels);
    header.sampleRate = static_cast<uint32_t>(sampleRate);
    header.byteRate = header.sampleRate * header.numChannels * (header.bitsPerSample / 8);
    header.blockAlign = header.numChannels * (header.bitsPerSample / 8);
    header.subchunk2Size = static_cast<uint32_t>(m_data.size() * sizeof(float));
    header.chunkSize = 36 + header.subchunk2Size;

    outFile.write(reinterpret_cast<char*>(&header), sizeof(header));
    outFile.write(reinterpret_cast<char*>(m_data.data()), m_data.size() * sizeof(float));
}

std::string RecorderAudio::get_data_size() {
    std::lock_guard<std::mutex> lock(m_data_mutex);
    size_t size_in_bytes = m_data.size() * sizeof(float);

    if (size_in_bytes < 1024) {
        return fmt::format("{} B", size_in_bytes);
    } else if (size_in_bytes < 1024 * 1024) {
        return fmt::format("{:.2f} KB", size_in_bytes / 1024.0);
    } else if (size_in_bytes < 1024 * 1024 * 1024) {
        return fmt::format("{:.2f} MB", size_in_bytes / (1024.0 * 1024.0));
    } else {
        return fmt::format("{:.2f} GB", size_in_bytes / (1024.0 * 1024.0 * 1024.0));
    }
}