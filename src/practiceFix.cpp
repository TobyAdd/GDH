#include "practiceFix.hpp"
#include "replayEngine.hpp"
#include "config.hpp"
#include <Geode/modify/PlayLayer.hpp>

class $modify(PlayLayer) {
    struct Fields {
        std::unordered_map<CheckpointObject*, std::pair<checkpoint_data, checkpoint_data>> checkpoints; // checkpoint_p1, checkpoint_p2
    };

    void resetLevel() {
        auto fields = m_fields.self();
        if (!m_checkpointArray->count())
            fields->checkpoints.clear();
            
        PlayLayer::resetLevel();
    }

    void loadFromCheckpoint(CheckpointObject* checkpoint) {
        auto& config = Config::get();
        auto& engine = ReplayEngine::get();

        if (config.get<bool>("practice_fix", false) || engine.mode == state::record) {
            auto fields = m_fields.self();
            if (fields->checkpoints.contains(checkpoint)) {
                PlayLayer::loadFromCheckpoint(checkpoint);

                auto& [checkpoint_p1, checkpoint_p2] = fields->checkpoints[checkpoint];
                checkpoint_p1.apply(m_player1);
                if (m_gameState.m_isDualMode)
                    checkpoint_p2.apply(m_player2);

                return;
            }
        }

        PlayLayer::loadFromCheckpoint(checkpoint);
    }

    CheckpointObject* createCheckpoint() {
        auto& config = Config::get();
        auto& engine = ReplayEngine::get();

        auto checkpoint = PlayLayer::createCheckpoint();
        if (!checkpoint || (!config.get<bool>("practice_fix", false) || engine.mode == state::record))
            return checkpoint;

        if (m_gameState.m_currentProgress > 0) {
            checkpoint_data checkpoint_p1(m_player1);
            checkpoint_data checkpoint_p2;
            if (m_gameState.m_isDualMode)
                checkpoint_p2 = checkpoint_data(m_player2);
            
            auto fields = m_fields.self();
            auto pair = std::make_pair(std::move(checkpoint_p1), std::move(checkpoint_p2));
            m_fields->checkpoints[checkpoint] = std::move(pair);            
        }

        return checkpoint;
    }

    void removeCheckpoint(bool first) {
        if (!m_checkpointArray->count()) {
            PlayLayer::removeCheckpoint(first);
            return;
        }
    
        auto* checkpoint = static_cast<CheckpointObject*>(
            first ? m_checkpointArray->objectAtIndex(0) : m_checkpointArray->lastObject()
        );
    
        auto fields = m_fields.self();
        m_fields->checkpoints.erase(checkpoint);
    
        PlayLayer::removeCheckpoint(first);
    }    
};
