#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include "config.hpp"

std::vector<int> object_ids = {
    899, // color trigger
    900, // another color trigger?

    // god damn why are there so many color triggers?
    29,
    30,
    104,
    105,
    221,
    717,
    718,
    743,
    744,
    915,

    1006, // pulse trigger
    1007, // alpha trigger
    1520, // shake trigger
    2903, // gradient trigger

    // change background, ground and middle ground triggers
    3029,
    3030,
    3031,

    // middle ground triggers
    2999,
    3606,
    3612,

    // area tint triggers
    3010,
    3015,
    3021,

    // area fade triggers
    3009,
    3014,
    3020,

    // bg effect triggers
    1818,
    1819,

    // ghost triggers
    33,
    32,

    // hide player triggres
    1613,
    1612,

    3608, // spawn particle trigger
};

class $modify(PlayLayer) {    
    void addObject(GameObject* obj) {
        if (!Config::get().get("layout_mode", false))
            return PlayLayer::addObject(obj);

        bool dontAdd = false;

        bool containsID = std::find(object_ids.begin(), object_ids.end(), obj->m_objectID) != object_ids.end();      
        if (containsID)
            dontAdd = true;

        if (!dontAdd) {
            obj->m_activeMainColorID = -1;
            obj->m_activeDetailColorID = -1;
            obj->m_detailUsesHSV = false;
            obj->m_baseUsesHSV = false;
            obj->setOpacity(255);
            obj->setVisible(true);

            PlayLayer::addObject(obj);
        }            
    }
};

class $modify(GameObject) {
    void setOpacity(unsigned char p0) {
        if (Config::get().get("layout_mode", false) && m_objectType == GameObjectType::Decoration && m_objectID != 38 && m_objectID != 44 && m_objectID != 47 &&
            m_objectID != 13 && m_objectID != 660 && m_objectID != 745 && m_objectID != 10 && m_objectID != 2064 && m_objectID != 749 && m_objectID != 747)
        {
            return GameObject::setOpacity(0);
        }
        
        GameObject::setOpacity(p0);        
    }
};

class $modify(GJBaseGameLayer) {
    void updateColor(cocos2d::ccColor3B& color, float fadeTime, int colorID, bool blending, float opacity, cocos2d::ccHSVValue& copyHSV, int colorIDToCopy, bool copyOpacity, EffectGameObject* callerObject, int unk1, int unk2)
    {
        if (Config::get().get("layout_mode", false)) {
            if (colorID == 1000) // background
                color = {40, 125, 255};
            else if (colorID == 1001) // ground (no ground 2???)
                color = {0, 102, 255};
            else if (colorID == 1002) // line
                color = {255, 255, 255};
            else if (colorID == 1013 || colorID == 1014) // middle ground
                color = {40, 125, 255};
        }

        GJBaseGameLayer::updateColor(color, fadeTime, colorID, blending, opacity, copyHSV, colorIDToCopy, copyOpacity, callerObject, unk1, unk2);
    }
};