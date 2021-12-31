#ifndef __EDITORUI_H__
#define __EDITORUI_H__

#include <gd.h>

namespace gd {

class GDH_DLL ExtendedLayer : public cocos2d::CCLayer {
    // todo
};

class GDH_DLL BoomScrollLayer : public cocos2d::CCLayer {
    public:
        cocos2d::CCArray* m_pDots;  // 0x11c
        PAD(64)
        ExtendedLayer* m_pLayer;    // 0x160
        PAD(72)
        int m_nPage;    // 0x1ac

        void instantMoveToPage(int page) {
            reinterpret_cast<void(__thiscall*)(BoomScrollLayer*, int)>(
                base + 0x12330
            )(this, page);
        }

        void moveToPage(int page) {
            reinterpret_cast<void(__thiscall*)(BoomScrollLayer*, int)>(
                base + 0x12400
            )(this, page);
        }
};

class GameManager;

class GDH_DLL EditButtonBar : public cocos2d::CCNode {
    public:
        cocos2d::CCPoint m_obPosition;
        int m_nUnknown;
        bool m_bUnknown;
        cocos2d::CCArray* m_pButtonArray = nullptr;
        BoomScrollLayer* m_pScrollLayer = nullptr;
        cocos2d::CCArray* m_pPagesArray = nullptr;

        virtual EditButtonBar* release(bool rel) {
            return reinterpret_cast<EditButtonBar*(__thiscall*)(
                EditButtonBar*, bool
            )>(
                base + 0x6e3c0
            )(this, rel);
        }

        bool init(cocos2d::CCArray* buttons, int idk, bool idkb, int rowCount, int columnCount, cocos2d::CCPoint pos) {
            auto ret = reinterpret_cast<bool(__thiscall*)(
                EditButtonBar*, cocos2d::CCArray*, int, bool, int, int, cocos2d::CCPoint
            )>(
                base + 0x6e550
            )(
                this, buttons, idk, idkb, rowCount, columnCount, pos
            );

            return ret;
        }

    public:
        static EditButtonBar* create(
            cocos2d::CCArray* buttons,
            cocos2d::CCPoint point,
            int idk,
            bool idk0,
            int rowCount,
            int columnCount
        ) {
            auto ret = reinterpret_cast<EditButtonBar*(__fastcall*)(
                cocos2d::CCArray*, int, bool, int, int, cocos2d::CCPoint
            )>(
                base + 0x6e450
            )(
                buttons, idk, idk0, rowCount, columnCount, point
            );

            __asm add esp, 0x14

            return ret;
        }

        ~EditButtonBar() {
            // free up memory
            CC_SAFE_RELEASE(this->m_pPagesArray);
            CC_SAFE_RELEASE(this->m_pButtonArray);
        }

        void loadFromItems(cocos2d::CCArray* buttons, int rowCount, int columnCount, bool idk) {
            reinterpret_cast<void(__thiscall*)(
                EditButtonBar*, cocos2d::CCArray*, int, int, bool
            )>(
                base + 0x6e5e0
            )(
                this, buttons, rowCount, columnCount, idk
            );
        }

        cocos2d::CCArray* getItems() { return this->m_pButtonArray; }

        void removeAllItems() {
            this->m_pButtonArray->removeAllObjects();

            this->reloadItemsInNormalSize();
        }

        void reloadItems(int rowCount, int columnCount) {
            if (this->m_pButtonArray)
                this->loadFromItems(this->m_pButtonArray, rowCount, columnCount, this->m_bUnknown);
        }
        void reloadItemsInNormalSize() {
            this->reloadItems(
                GameManager::sharedState()->getIntGameVariable("0049"),
                GameManager::sharedState()->getIntGameVariable("0050")
            );
        }

        void insertButton(CCMenuItemSpriteExtra* btn, unsigned int index, bool reload = true) {
            if (this->m_pButtonArray)
                this->m_pButtonArray->insertObject(btn, index);
            if (reload)
                this->reloadItemsInNormalSize();
        }

        void addButton(CCMenuItemSpriteExtra* btn, bool reload = true) {
            if (this->m_pButtonArray)
                this->m_pButtonArray->addObject(btn);
            if (reload)
                this->reloadItemsInNormalSize();
        }
};

class GDH_DLL CreateMenuItem : public CCMenuItemSpriteExtra {
    public:
        PAD(0x18)
        int m_nObjectID;
        int m_nBuildTabPage;
        int m_nBuildTab;
};

class GDH_DLL EditorPauseLayer : public gd::CCBlockLayer {
    public:
        PAD(0x8)
        gd::CCMenuItemSpriteExtra* m_pButton0;  // 0x1a4
        gd::CCMenuItemSpriteExtra* m_pButton1;  // 0x1a8
        gd::LevelEditorLayer* m_pEditorLayer;   // 0x1ac

        EditorPauseLayer* constructor() {
            return reinterpret_cast<EditorPauseLayer*(__fastcall*)(EditorPauseLayer*)>(
                base + 0x72f10
            )(this);
        }

        EditorPauseLayer() {
            constructor();
        }

        bool init(LevelEditorLayer* editor) {
            return reinterpret_cast<bool(__thiscall*)(EditorPauseLayer*, LevelEditorLayer*)>(
                base + 0x730e0
            )(this, editor);
        }

        static EditorPauseLayer* create(LevelEditorLayer* editor) {
            auto pRet = new EditorPauseLayer;

            if (pRet && pRet->init(editor)) {
                pRet->autorelease();
                return pRet;
            }

            CC_SAFE_DELETE(pRet);
            return nullptr;
        }

        virtual void keyBackClicked() {
            reinterpret_cast<void(__fastcall*)(EditorPauseLayer*)>(
                base + 0x758d0
            )(this);
        }

        void onResume(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorPauseLayer*, cocos2d::CCObject*)>(
                base + 0x74fe0
            )(this, pSender);
        }

        void onSaveAndPlay(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorPauseLayer*, cocos2d::CCObject*)>(
                base + 0x753d0
            )(this, pSender);
        }

        void onSaveAndExit(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorPauseLayer*, cocos2d::CCObject*)>(
                base + 0x75620
            )(this, pSender);
        }

        void onSave(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorPauseLayer*, cocos2d::CCObject*)>(
                base + 0x755a0
            )(this, pSender);
        }

        void onExitNoSave(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorPauseLayer*, cocos2d::CCObject*)>(
                base + 0x75700
            )(this, pSender);
        }

        void uncheckAllPortals(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorPauseLayer*, cocos2d::CCObject*)>(
                base + 0x74760
            )(this, pSender);
        }

        void saveLevel() {
            reinterpret_cast<void(__fastcall*)(EditorPauseLayer*)>(
                base + 0x75010
            )(this);
        }
};

class GJScaleControl;
class GJRotationControl;
class CCMenuItemSpriteExtra;
class CCMenuItemToggler;
class Slider;
class GameObject;

enum EditCommand {
    kEditCommandSmallLeft   = 1,
    kEditCommandSmallRight  = 2,
    kEditCommandSmallUp     = 3,
    kEditCommandSmallDown   = 4,

    kEditCommandLeft        = 5,
    kEditCommandRight       = 6,
    kEditCommandUp          = 7,
    kEditCommandDown        = 8,

    kEditCommandBigLeft     = 9,
    kEditCommandBigRight    = 10,
    kEditCommandBigUp       = 11,
    kEditCommandBigDown     = 12,

    kEditCommandTinyLeft    = 13,
    kEditCommandTinyRight   = 14,
    kEditCommandTinyUp      = 15,
    kEditCommandTinyDown    = 16,

    kEditCommandFlipX       = 17,
    kEditCommandFlipY       = 18,
    kEditCommandRotateCW    = 19,
    kEditCommandRotateCCW   = 20,
    kEditCommandRotateCW45  = 21,
    kEditCommandRotateCCW45 = 22,
    kEditCommandRotateFree  = 23,
    kEditCommandRotateSnap  = 24,
    
    kEditCommandScale       = 25,
};

class GDH_DLL EditorUI : public cocos2d::CCLayer,
    public gd::FLAlertLayerProtocol,
    public gd::ColorSelectDelegate,
    public gd::GJRotationControlDelegate,
    public gd::GJScaleControlDelegate,
    public gd::MusicDownloadDelegate {

    public:
        EditButtonBar* m_pButtonBar; // 0x134
        PAD(0x4)
        cocos2d::CCArray* m_pHideableUIElementArray; // 0x13c
        PAD(28)
        bool m_bMoveModifier; // 0x158
        PAD(3)
        int m_nRotationTouchID; // 0x15c
        int m_nScaleTouchID; // 0x160
        int m_nTouchID; // 0x164
        cocos2d::CCLabelBMFont* m_pObjectInfoLabel; // 0x168
        GJRotationControl* m_pRotationControl; // 0x16c
        cocos2d::CCPoint m_obScalePos;  // 0x170
        bool m_bTouchDown; // 0x178
        PAD(3)
        GJScaleControl* m_pScaleControl; // 0x17c
        cocos2d::CCDictionary* m_pEditButtonDict; // 0x180
        EditButtonBar* m_pCreateButtonBar; // 0x184
        EditButtonBar* m_pEditButtonBar; // 0x188
        Slider* m_pPositionSlider; // 0x18c
        float m_fUnknown0;      // 0x190
        float m_fMinYLimit;     // 0x194 (tf is this)
        float m_fUnknown2;      // 0x198
        bool m_bSwipeEnabled; // 0x19c
        PAD(3)
        bool m_bFreeMoveEnabled;  // 0x1a0
        PAD(15)
        cocos2d::CCArray* m_pUnknownArray2; // 0x1b0
        PAD(0x8)
        cocos2d::CCArray* m_pSelectedObjects; // 0x1bc
        cocos2d::CCMenu* m_pDeleteMenu; // 0x1c0
        cocos2d::CCArray* m_pUnknownArray4; // 0x1c4
        CCMenuItemSpriteExtra* m_pDeleteModeBtn; // 0x1c8
        CCMenuItemSpriteExtra* m_pBuildModeBtn; // 0x1cc
        CCMenuItemSpriteExtra* m_pEditModeBtn; // 0x1d0
        CCMenuItemSpriteExtra* m_pSwipeBtn; // 0x1d4
        CCMenuItemSpriteExtra* m_pFreeMoveBtn; // 0x1d8
        CCMenuItemSpriteExtra* m_pDeselectBtn; // 0x1dc
        CCMenuItemSpriteExtra* m_pSnapBtn; // 0x1e0
        CCMenuItemSpriteExtra* m_pRotateBtn; // 0x1e4
        CCMenuItemSpriteExtra* m_pPlaybackBtn; // 0x1e8
        CCMenuItemSpriteExtra* m_pPlaytestBtn; // 0x1ec
        CCMenuItemSpriteExtra* m_pPlaytestStopBtn; // 0x1f0
        CCMenuItemSpriteExtra* m_pTrashBtn; // 0x1f4
        CCMenuItemSpriteExtra* m_pLinkBtn; // 0x1f8
        CCMenuItemSpriteExtra* m_pUnlinkBtn; // 0x1fc
        CCMenuItemSpriteExtra* m_pUndoBtn; // 0x200
        CCMenuItemSpriteExtra* m_pRedoBtn; // 0x204
        CCMenuItemSpriteExtra* m_pEditObjectBtn; // 0x208
        CCMenuItemSpriteExtra* m_pEditGroupBtn; // 0x20c
        CCMenuItemSpriteExtra* m_pEditHSVBtn; // 0x210
        CCMenuItemSpriteExtra* m_pEditSpecialBtn; // 0x214
        CCMenuItemSpriteExtra* m_pCopyPasteBtn; // 0x218
        CCMenuItemSpriteExtra* m_pCopyBtn; // 0x21c
        CCMenuItemSpriteExtra* m_pPasteBtn; // 0x220
        CCMenuItemSpriteExtra* m_pCopyValuesBtn; // 0x224
        CCMenuItemSpriteExtra* m_pPasteStateBtn; // 0x228
        CCMenuItemSpriteExtra* m_pPasteColorBtn; // 0x22c
        CCMenuItemSpriteExtra* m_pGoToLayerBtn; // 0x230
        CCMenuItemToggler* m_pGuideToggle; // 0x234
        cocos2d::CCArray* m_pCreateButtonBars; // 0x238
        cocos2d::CCMenu* m_pTabsMenu; // 0x23c
        cocos2d::CCArray* m_pTabsArray; // 0x240
        cocos2d::CCSprite* m_pIdkSprite0; // 0x244
        cocos2d::CCSprite* m_pIdkSprite1; // 0x248
        CCMenuItemSpriteExtra* m_pButton27; // 0x24c
        CCMenuItemSpriteExtra* m_pButton28; // 0x250
        CCMenuItemSpriteExtra* m_pDeleteFilterNone; // 0x254
        CCMenuItemSpriteExtra* m_pDeleteFilterStatic; // 0x258
        CCMenuItemSpriteExtra* m_pDeleteFilterDetails; // 0x25c
        CCMenuItemSpriteExtra* m_pDeleteFilterCustom; // 0x260
        cocos2d::CCLabelBMFont* m_pCurrentLayerLabel; // 0x264
        CCMenuItemSpriteExtra* m_pLayerNextBtn; // 0x268
        CCMenuItemSpriteExtra* m_pLayerPrevBtn; // 0x26c
        CCMenuItemSpriteExtra* m_pGoToBaseBtn; // 0x270
        PAD(0x8)
        int m_nSelectedCreateObjectID; // 0x278
        cocos2d::CCArray* m_pCreateButtonArray;  // 0x280
        cocos2d::CCArray* m_pCustomObjectButtonArray; // 0x284
        cocos2d::CCArray* m_pUnknownArray9; // 0x288
        int m_nSelectedMode; // 0x28c
        LevelEditorLayer* m_pEditorLayer;  // 0x290
        cocos2d::CCPoint m_obSwipeStart;    // 0x294
        cocos2d::CCPoint m_obSwipeEnd;      // 0x29c
        PAD(0x20)
        GameObject* m_pSelectedObject;  // 0x2c4
        PAD(8)
        std::string m_sClipboard;   // 0x2d0
        PAD(8)
        int m_nSelectedTab; // 0x2f0
        PAD(36)
        bool m_bSpaceKeyPressed; // 0x318

        static constexpr const int Mode_Create = 2;
        static constexpr const int Mode_Delete = 1;
        static constexpr const int Mode_Edit = 3;
    
    public:
        GDH_ADD(
            static EditorUI* get() {
                auto lel = LevelEditorLayer::get();
                if (!lel) return nullptr;
                return lel->m_pEditorUI;
            }
        );

        cocos2d::CCArray* pasteObjects(std::string const& _str) {
            auto ret = reinterpret_cast<cocos2d::CCArray*(__thiscall*)(
                EditorUI*, std::string
            )>(
                base + 0x88240
            )(
                this, _str
            );

            return ret;
        }

        void deselectAll() {
            reinterpret_cast<void(__fastcall*)(EditorUI*)>(
                base + 0x86af0
            )(this);
        }

        void selectObject(GameObject* obj, bool idk) {
            reinterpret_cast<void(__thiscall*)(
                EditorUI*, GameObject*, bool
            )>(
                base + 0x86250
            )(
                this, obj, idk
            );
        }

        void selectObjects(cocos2d::CCArray* objs, bool idk) {
            reinterpret_cast<void(__thiscall*)(
                EditorUI*, cocos2d::CCArray*, bool
            )>(
                base + 0x864a0
            )(
                this, objs, idk
            );
        }

        void selectAll() {
            reinterpret_cast<void(__fastcall*)(EditorUI*)>(
                base + 0x86c40
            )(this);
        }

        void selectAllWithDirection(bool left) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, bool)>(
                base + 0x86d80
            )(this, left);
        }

        cocos2d::CCArray* getSelectedObjects() {
            return reinterpret_cast<cocos2d::CCArray*(__thiscall*)(EditorUI*)>(
                base + 0x86900
            )(this);
        }

        cocos2d::CCPoint getTouchPoint(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
            cocos2d::CCPoint res;

            reinterpret_cast<cocos2d::CCPoint*(__thiscall*)(
                EditorUI*, cocos2d::CCPoint*, cocos2d::CCTouch*, cocos2d::CCEvent*
            )>(
                base + 0x90620
            )(this, &res, touch, event);

            return res;
        }

        void onSelectBuildTab(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x887f0
            )(this, pSender);
        }

        void onCreateButton(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x854f0
            )(this, pSender);
        }

        void enableButton(CreateMenuItem* btn) {
            reinterpret_cast<void(__stdcall*)(CreateMenuItem*)>(
                base + 0x78990
            )(btn);
        }

        void disableButton(CreateMenuItem* btn) {
            reinterpret_cast<void(__stdcall*)(CreateMenuItem*)>(
                base + 0x78af0
            )(btn);
        }

        CreateMenuItem* getCreateBtn(int id, int bg) {
            auto ret = reinterpret_cast<CreateMenuItem*(__thiscall*)(
                EditorUI*, int, int
            )>(
                base + 0x85120
            )(
                this, id, bg
            );

            return ret;
        }

        CCMenuItemSpriteExtra* getSpriteButton(
            const char* sprite,
            cocos2d::SEL_MenuHandler callback,
            cocos2d::CCMenu* menu,
            float scale
        ) {
            return reinterpret_cast<CCMenuItemSpriteExtra*(__thiscall*)(
                EditorUI*, const char*, cocos2d::SEL_MenuHandler,
                cocos2d::CCMenu*, float
            )>(
                base + 0x78bf0
            )(
                this, sprite, callback, menu, scale
            );
        }

        cocos2d::CCPoint getGroupCenter(cocos2d::CCArray* objs, bool idk) {
            cocos2d::CCPoint res;

            reinterpret_cast<cocos2d::CCPoint*(__thiscall*)(
                EditorUI*, cocos2d::CCPoint*, cocos2d::CCArray*, bool
            )>(
                base + 0x8fc30
            )(
                this, &res, objs, idk
            );

            return res;
        }

        cocos2d::CCPoint offsetForKey(int objID) {
            cocos2d::CCPoint res;
            reinterpret_cast<cocos2d::CCPoint*(__thiscall*)(
                cocos2d::CCPoint*, int
            )>(base + 0x92310)(&res, objID - 9u);
            return res;
        }

        void updateButtons() {
            reinterpret_cast<void(__fastcall*)(
                EditorUI*
            )>( base + 0x78280 )(this);
        }

        void updateDeleteMenu() {
            reinterpret_cast<void(__fastcall*)(
                EditorUI*
            )>( base + 0x7c5d0 )(this);
        }
        
        void updateCreateMenu(bool updateTab) {
            reinterpret_cast<void(__thiscall*)(
                EditorUI*, bool
            )>( base + 0x85530 )(this, updateTab);
        }

        void toggleMode(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x7ad20
            )(this, pSender);
        }
        
        void moveObject(GameObject* obj, cocos2d::CCPoint position) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, GameObject*, cocos2d::CCPoint)>(base + 0x8ddb0)(this, obj, position);
        }

        // i cant get these to not crash

        void zoomIn(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x877c0
            )(this, pSender);
        }

        void zoomOut(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x87830
            )(this, pSender);
        }

        void updateZoom(float amt) {
            __asm movss xmm1, amt
            reinterpret_cast<void(__thiscall*)(EditorUI*)>(base + 0x878a0)(this);
        }

        void updateSlider() {
            reinterpret_cast<void(__fastcall*)(EditorUI*)>(
                base + 0x78f10
            )(this);
        }

        void rotateObjects(cocos2d::CCArray* objects, float angle, cocos2d::CCPoint center) {
            __asm movss xmm2, angle;
            reinterpret_cast<void(__thiscall*)(
                EditorUI*, cocos2d::CCArray*, cocos2d::CCPoint
            )>(base + 0x8ee80)(this, objects, center);
        }

        void scaleObjects(cocos2d::CCArray* objects, float scale, cocos2d::CCPoint center) {
            __asm movss xmm2, scale;
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCArray*, cocos2d::CCPoint)>(base + 0x8f150)(this, objects, center);
        }
    
        void updateGridNodeSize() {
            reinterpret_cast<void(__fastcall*)(EditorUI*)>(
                base + 0x78f60
            )(this);
        }

        void updateObjectInfoLabel() {
            reinterpret_cast<void(__fastcall*)(EditorUI*)>(
                base + 0x793b0
            )(this);
        }
    
        void updateSpecialUIElements() {
            reinterpret_cast<void(__fastcall*)(EditorUI*)>(
                base + 0x87030
            )(this);
        }

        void constrainGameLayerPosition(float x = -3.0f, float y = -6.0f) {
            __asm {
                movss xmm1, x
                movss xmm2, y
            }

            reinterpret_cast<void(__fastcall*)(EditorUI*)>(
                base + 0x8f920
            )(this);
        }
    
        void moveGameLayer(cocos2d::CCPoint const& pos) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCPoint)>(
                base + 0x79290
            )(this, pos);
        }
    
        void showUI(bool show) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, bool)>(
                base + 0x87180
            )(this, show);
        }

        void editObject(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x8ca50
            )(this, pSender);
        }

        void editObject2(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x8d1b0
            )(this, pSender);
        }

        void editGroup(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x8d720
            )(this, pSender);
        }

        void moveObjectCall(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x8db30
            )(this, pSender);
        }

        void moveObjectCall(EditCommand command) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, EditCommand)>(
                base + 0x8db50
            )(this, command);
        }

        void transformObjectCall(cocos2d::CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x8def0
            )(this, pSender);
        }

        void transformObjectCall(EditCommand command) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, EditCommand)>(
                base + 0x8df10
            )(this, command);
        }

        void onDeleteSelected(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x7bf50
            )(this, pSender);
        }

        void undoLastAction(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x87070
            )(this, pSender);
        }

        void redoLastAction(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x870f0
            )(this, pSender);
        }

        void onCopy(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x87fb0
            )(this, pSender);
        }

        void onPaste(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x880c0
            )(this, pSender);
        }

        void onDuplicate(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x87d20
            )(this, pSender);
        }

        void toggleEnableRotate(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x860d0
            )(this, pSender);
        }

        void toggleFreeMove(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x85eb0
            )(this, pSender);
        }

        void toggleSwipe(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x85dd0
            )(this, pSender);
        }

        void toggleSnap(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x85fa0
            )(this, pSender);
        }

        void onPlayback(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x87340
            )(this, pSender);
        }

        void onPlaytest(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x87600
            )(this, pSender);
        }

        void onStopPlaytest(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x876e0
            )(this, pSender);
        }

        void onGroupUp(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x8d780
            )(this, pSender);
        }

        void onGroupDown(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x8d7e0
            )(this, pSender);
        }

        void selectBuildTab(int tab) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, int)>(
                base + 0x88810
            )(this, tab);
        }

        void onPause(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x78020
            )(this, pSender);
        }
    
        void onSettings(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x77fe0
            )(this, pSender);
        }
    
        void activateRotationControl(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x8fe70
            )(this, pSender);
        }
    
        void activateScaleControl(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x889b0
            )(this, pSender);
        }

        // aka Build Helper
        void dynamicGroupUpdate(bool idk) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, bool)>(
                base + 0x8ad10
            )(this, idk);
        }

        void createRockOutline() {
            reinterpret_cast<void(__fastcall*)(EditorUI*)>(
                base + 0x89c10
            )(this);
        }

        void createRockEdges() {
            reinterpret_cast<void(__fastcall*)(EditorUI*)>(
                base + 0x88ec0
            )(this);
        }

        void createRockBase() {
            reinterpret_cast<void(__fastcall*)(EditorUI*)>(
                base + 0x8a2c0
            )(this);
        }
    
        void onCopyState(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x88490
            )(this, pSender);
        }
    
        void onPasteColor(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x88580
            )(this, pSender);
        }
    
        void onPasteState(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x884c0
            )(this, pSender);
        }
    
        void onGroupSticky(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x87a80
            )(this, pSender);
        }
    
        void onUngroupSticky(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x87ac0
            )(this, pSender);
        }
   
        void onGoToLayer(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x886b0
            )(this, pSender);
        }

        void onGoToBaseLayer(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x88790
            )(this, pSender);
        }
    
        void editColor(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCObject*)>(
                base + 0x8d3c0
            )(this, pSender);
        }
    
        void alignObjects(cocos2d::CCArray* objs, bool alignY) {
            reinterpret_cast<void(__thiscall*)(EditorUI*, cocos2d::CCArray*, bool)>(
                base + 0x8f320
            )(this, objs, alignY);
        }
    };

}

#endif
