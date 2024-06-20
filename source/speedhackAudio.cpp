#include "includes.hpp"
#include "speedhackAudio.hpp"
#include "gd.hpp"
#include "hacks.hpp"

namespace SpeedhackAudio {
    void update() {
        using namespace gd;
        FMOD::Channel *channel;
        FMOD::System *system = gd::FMODAudioEngine::sharedEngine()->m_system();
        for (auto i = 0; i < 2; i++) {
            system->getChannel(126 + i, &channel);
            if (channel) {
                FMOD::Sound *sound;
                channel->getCurrentSound(&sound);
                float freq;
                sound->getDefaults(&freq, nullptr);
                if (hacks::speedhack_audio)
                    channel->setFrequency(freq * hacks::speed_value);
                else
                    channel->setFrequency(freq * 1.f);
            }
        }
    }
}
