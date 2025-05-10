#include <mutex>
#include <thread>
#include "config.hpp"
#include "utils.hpp"

class RenderTexture {
public:
    int width, height;
    unsigned int fbo;
    unsigned int textureId;
    int oldFBO;

    void begin();
    void capture_frame(std::mutex& lock, std::vector<uint8_t>& data, volatile bool& frame_has_data);
    void end();
};

class Recorder {
public:
    static Recorder& get() {
        static Recorder instance;
        return instance;
    }

    Recorder& operator=(const Recorder&) = delete;
    Recorder(const Recorder&) = delete;

    std::filesystem::path folderShowcasesPath = folderPath / "Showcases";
    std::string video_name = "video.mp4";
    std::string video_name2 = "video";

    bool settings_openned = false;

    bool enabled = false;
    bool is_recording = false;
    int width = 1280;
    int height = 720;
    int fps = 60;

    bool lock_aspect_ratio = true;

    std::string bitrate = "25M";
    std::string codec = "libx264";
    std::string extra_args = "-pix_fmt yuv420p -preset ultrafast";
    std::string vf_args = "\"vflip\"";

    bool vflip = true;

    bool fade_in = false;
    float fade_in_start = 0;
    float fade_in_end = 0.5f;

    bool fade_out = false;

    bool hide_level_complete = true;

    double delay;

    std::vector<uint8_t> current_frame;
    bool frame_has_data = false;
    RenderTexture texture;

    double last_frame_time, extra_time;

    std::mutex lock;

    float after_end_extra_time;
    float after_end_duration = 3.f;
    
    void start(std::string command);
    void stop();
    void render_frame();
    void handle_recording(float dt);  

    bool need_remove_black = false;
    bool need_visible_lc = false;

    bool ffmpeg_installed = false;
    bool advanced_mode = false;
    std::string full_cmd;
    std::string compile_command();
    void compile_vf_args();

    void applyWinSize();
    void restoreWinSize();
    bool needRevertOld = false;

    bool flvc_enabled = false;

    bool overlay_mode = false;
    bool native_mode = false;
    float pauseLayerScale = 1.f;

    #ifdef GEODE_IS_WINDOWS
    WindowStateBackup backup;
    #endif
private:
    Recorder() = default;

    cocos2d::CCSize oldDesignResolution;
    cocos2d::CCSize newDesignResolution;
    cocos2d::CCSize originalScreenScale;
    cocos2d::CCSize newScreenScale;
};

class RecorderAudio {
public:
    static RecorderAudio& get() {
        static RecorderAudio instance;
        return instance;
    }

    RecorderAudio& operator=(const RecorderAudio&) = delete;
    RecorderAudio(const RecorderAudio&) = delete;

    std::string audio_name = "audio.wav";
    std::string audio_name2 = "audio";

    bool enabled = false;
    bool is_recording = false;

    bool showcase_mode = true;

    float after_end_extra_time;
    float after_end_duration = 3.f;

    std::string extra_args;

    float volume_music = 1.f;
    float volume_sfx = 1.f;

    float old_volume_music;
    float old_volume_sfx;

    bool first_start = false;
    bool want_to_stop = false;

    void start();
    void stop();
    void handle_recording(float dt);

    std::vector<float> get_data();
    std::string get_data_size();

private:
    RecorderAudio() = default;
    
    FMOD::DSP* m_dsp = nullptr;
    FMOD::ChannelGroup* m_masterGroup = nullptr;
    std::vector<float> m_data;
    std::mutex m_data_mutex;

    void init();
    void save_to_wav(const std::filesystem::path& filename);
};