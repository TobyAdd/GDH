#include "recorder.hpp"
#include "hooks.hpp"

Recorder recorder;
RecorderAudio recorderAudio;

void RenderTexture::begin() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &oldFBO);

    texture = new CCTexture2D;
    {
        auto data = malloc(width * height * 3);
        memset(data, 0, width * height * 3);
        texture->initWithData(data, kCCTexture2DPixelFormat_RGB888, width, height, CCSize(static_cast<float>(width), static_cast<float>(height)));
        free(data);
    }

    glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &oldRBO);

    glGenFramebuffersEXT(1, &currentFBO);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, currentFBO);

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture->getName(), 0);
    
    texture->setAliasTexParameters();
    //texture->autorelease();

    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, oldRBO);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, oldFBO);    
}

void RenderTexture::capture_frame(std::mutex& lock, std::vector<uint8_t>& data, volatile bool& frame_has_data) {
    glViewport(0, 0, width, height);

    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &oldFBO);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, currentFBO);

    auto director = CCDirector::sharedDirector();
    auto scene = hooks::pl;

    recorder.playLayerVisiting = true;
    scene->visit();
    recorder.playLayerVisiting = false;

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    lock.lock();
    frame_has_data = true;
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    lock.unlock();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, oldFBO);
    director->setViewport();
}

void RenderTexture::end() {
    texture->release();
}

void Recorder::start() {
    last_frame_time = extra_time = 0;

    after_end_extra_time = 0.f;

    frame_has_data = false;
    current_frame.resize(width * height * 3, 0);
    is_recording = true;
    texture.height = height;
    texture.width = width;
    texture.begin();
    std::string video_name_ = video_name;

    std::thread([&, video_name_] {
        std::string command = "ffmpeg.exe -y -f rawvideo -pix_fmt rgb24 -s " +
            std::to_string(width) + "x" + std::to_string(height) +
            " -r " + std::to_string(fps) + " -i -";

        if (!codec.empty()) {
            command += " -c:v " + codec;
        }

        if (!bitrate.empty()) {
            command += " -b:v " + bitrate;
        }

        if (!extra_args.empty()) {
            command += " " + extra_args;
        }

        command += " -pix_fmt yuv420p -vf \"vflip\" \"" + video_name_ + "\"";
            
        auto process = subprocess::Popen(command);
        while (is_recording || frame_has_data) {
            lock.lock();
            if (frame_has_data) {
                const auto frame = current_frame;
                frame_has_data = false;
                lock.unlock();
                process.m_stdin.write(frame.data(), frame.size());
            } else lock.unlock();
        }
        if (process.close()) {
            Console::WriteLn("vsem pizda");
            return;
        }
        Console::WriteLn("zaebis");
    }).detach();
}

void Recorder::stop() {
    texture.end();
    is_recording = false;
    enabled = false;
}

void Recorder::render_frame() {
    while (frame_has_data) {}
    texture.capture_frame(lock, current_frame, frame_has_data);
}

void Recorder::handle_recording(gd::PlayLayer* playLayer, float dt) {
    if (!playLayer->m_hasCompletedLevel() || after_end_extra_time < after_end_duration) {
        if (playLayer->m_hasCompletedLevel()) {
            after_end_extra_time += dt;
        }

        double frame_dt = 1.0 / static_cast<double>(fps);
        double time = playLayer->m_time() - last_frame_time + extra_time;
        if (time >= frame_dt) {
            extra_time = time - frame_dt;
            last_frame_time = playLayer->m_time();
            render_frame();
        }
    }
    else {
        stop();
    }   
}

void RecorderAudio::start() {
    is_recording = true;
    after_end_extra_time = 0;
    gd::FMODAudioEngine::sharedEngine()->m_system()->setOutput(3);
}

void RecorderAudio::stop() {
    enabled = false;
    is_recording = false;
    gd::FMODAudioEngine::sharedEngine()->m_system()->setOutput(0);
}

void RecorderAudio::handle_recording(gd::PlayLayer* playLayer, float dt) {
    if (showcase_mode) {
        if (!playLayer->m_hasCompletedLevel() || after_end_extra_time < after_end_duration) {
            if (playLayer->m_hasCompletedLevel()) {
                after_end_extra_time += dt;
            }
        }
        else {
            stop();
        } 
    }  
}