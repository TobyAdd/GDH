#include "includes.hpp"
#include <subprocess.hpp>
#include <CCGL.h>
#include <mutex>
#include <thread>
#include "gd.hpp"

class RenderTexture {
public:
    int width, height;
    CCTexture2D* texture;
    int oldFBO, oldRBO;
    unsigned currentFBO;

    void begin();
    void capture_frame(std::mutex& lock, std::vector<uint8_t>& data, volatile bool& frame_has_data);
    void end();
};

class Recorder {
    public:
        std::string video_name = "video.mp4";

        bool playLayerVisiting = false;
        bool shaderLayerVisiting = false;

        bool enabled = false;
        bool is_recording = false;
        int width = 1280;
        int height = 720;
        int fps = 60;
        std::string bitrate = "30M";
        std::string codec = "libx264";
        std::string extra_args = "-preset ultrafast";

        std::vector<uint8_t> current_frame;
        bool frame_has_data = false;
        RenderTexture texture;

        double last_frame_time, extra_time;

        std::mutex lock;

        float after_end_extra_time;
        bool has_level_complete_menu = false;
        float after_end_duration = 3.f;
        
        void start();
        void stop();
        void render_frame();
        void handle_recording(gd::PlayLayer* playLayer, float dt);  
};


extern Recorder recorder;

class RecorderAudio {
    public:
        std::string audio_name = "audio.mp3";

        bool enabled = false;
        bool is_recording = false;

        bool showcase_mode = true;

        float after_end_extra_time;
        float after_end_duration = 3.f;

        std::string extra_args;
        
        void start();
        void stop(); 
        void handle_recording(gd::PlayLayer* playLayer, float dt);  
};


extern RecorderAudio recorderAudio;