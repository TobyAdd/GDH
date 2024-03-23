#include "includes.hpp"
#include "speedhackAudio.hpp"
#include "gd.hpp"
#include "hacks.hpp"

float SpeedhackAudio::speed = 1.0f;

namespace SpeedhackAudio {
    void update() {
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
                    channel->setFrequency(freq * speed);
                else
                    channel->setFrequency(freq * CCDirector::sharedDirector()->getScheduler()->getTimeScale());
            }
        }
    }
}
