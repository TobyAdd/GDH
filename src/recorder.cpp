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

class $modify(ShaderLayer)
{
	void visit()
	{
        auto& recorder = Recorder::get();
		recorder.shader_visiting = true;
		ShaderLayer::visit();
		recorder.shader_visiting = false;
	}
};

void RenderTexture::begin() {   
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);

    texture = new cocos2d::CCTexture2D;
    {
        auto data = malloc(width * height * 3);
        memset(data, 0, width * height * 3);
        texture->initWithData(data, cocos2d::kCCTexture2DPixelFormat_RGB888, width, height, cocos2d::CCSize(static_cast<float>(width), static_cast<float>(height)));
        free(data);
    }

    glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRBO);

    glGenFramebuffers(1, &currentFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getName(), 0);
    
    texture->setAliasTexParameters();
    texture->autorelease();

    glBindRenderbuffer(GL_RENDERBUFFER, oldRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
}

void RenderTexture::capture_frame(std::mutex& lock, std::vector<uint8_t>& data, volatile bool& frame_has_data) {
    auto& recorder = Recorder::get();
    glViewport(0, 0, width, height);

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);

    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = GameManager::sharedState()->getPlayLayer();

    recorder.playlayer_visiting = true;
    scene->visit();
    recorder.playlayer_visiting = false;

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    lock.lock();
    frame_has_data = true;
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    lock.unlock();

    glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
    director->setViewport();
}

void RenderTexture::end() {
    if (texture)
        texture->release();
}

void Recorder::applyWinSize() {
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

    if (oldDesignResolution != newDesignResolution)
        applyWinSize();

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

void RecorderAudio::start() {
    is_recording = true;
    after_end_extra_time = 0;

    auto fmod_engine = FMODAudioEngine::get();
    
    old_volume_music = fmod_engine->getBackgroundMusicVolume();
    old_volume_sfx = fmod_engine->getEffectsVolume();

    fmod_engine->setBackgroundMusicVolume(1.f);
    fmod_engine->setEffectsVolume(1.f);

    fmod_engine->m_system->setOutput(FMOD_OUTPUTTYPE_WAVWRITER);
}

void RecorderAudio::stop() {
    enabled = false;
    is_recording = false;

    auto fmod_engine = FMODAudioEngine::get();
    fmod_engine->m_system->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);

    fmod_engine->setBackgroundMusicVolume(old_volume_music);
    fmod_engine->setEffectsVolume(old_volume_sfx);

    ImGuiH::Popup::get().add_popup("Audio recording stoped!");

    if (std::filesystem::exists("fmodoutput.wav")) {
        try {
            std::filesystem::rename("fmodoutput.wav", Recorder::get().folderShowcasesPath / audio_name);
        }
        catch (const std::filesystem::filesystem_error& e) {
            geode::log::error("Error moving file: {}", e.what());
        }
    }    
}

void RecorderAudio::handle_recording(float dt) {
    auto playLayer = GameManager::sharedState()->getPlayLayer();
    if (showcase_mode) {
        if (!playLayer->m_hasCompletedLevel || after_end_extra_time < after_end_duration) {
            if (playLayer->m_hasCompletedLevel) {
                after_end_extra_time += dt;
            }
        }
        else {
            stop();
        } 
    }  
}
