/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCDIRECTOR_H__
#define __CCDIRECTOR_H__

#include "platform/CCPlatformMacros.h"
#include "cocoa/CCObject.h"
#include "ccTypes.h"
#include "cocoa/CCGeometry.h"
#include "cocoa/CCArray.h"
#include "CCGL.h"
#include "kazmath/mat4.h"
#include "label_nodes/CCLabelAtlas.h"
#include "ccTypeInfo.h"


NS_CC_BEGIN

/**
 * @addtogroup base_nodes
 * @{
 */

 /** @typedef ccDirectorProjection
  Possible OpenGL projections used by director
  */
    typedef enum {
    /// sets a 2D projection (orthogonal projection)
    kCCDirectorProjection2D,

    /// sets a 3D projection with a fovy=60, znear=0.5f and zfar=1500.
    kCCDirectorProjection3D,

    /// it calls "updateProjection" on the projection delegate.
    kCCDirectorProjectionCustom,

    /// Default projection is 3D projection
    kCCDirectorProjectionDefault = kCCDirectorProjection3D,
} ccDirectorProjection;

    typedef enum {
        LOW=1,
        MEDIUM,
        HIGH
    }TextureQuality;

    typedef enum{
        FadeTransition,
        MoveInTransition
    }PopTransition;

/* Forward declarations. */

//Robtop Modification : New Classes

class CCLabelAtlas;
class CCScene;
class CCEGLView;
class CCDirectorDelegate;
class CCNode;
class CCScheduler;
class CCActionManager;
class CCTouchDispatcher;
class CCKeypadDispatcher;
class CCKeyboardDispatcher;
class CCMouseDispatcher;
class CCSceneDelegate;
class CCAccelerometer;


/**
@brief Class that creates and handle the main Window and manages how
and when to execute the Scenes.

 The CCDirector is also responsible for:
  - initializing the OpenGL context
  - setting the OpenGL pixel format (default on is RGB565)
  - setting the OpenGL buffer depth (default one is 0-bit)
  - setting the projection (default one is 3D)
  - setting the orientation (default one is Portrait)

 Since the CCDirector is a singleton, the standard way to use it is by calling:
  _ CCDirector::sharedDirector()->methodName();

 The CCDirector also sets the default OpenGL context:
  - GL_TEXTURE_2D is enabled
  - GL_VERTEX_ARRAY is enabled
  - GL_COLOR_ARRAY is enabled
  - GL_TEXTURE_COORD_ARRAY is enabled
*/
class CC_DLL CCDirector : public CCObject, public TypeInfo
{
public:
    /**
     *  @js ctor
     */
    CCDirector(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~CCDirector(void);
    virtual bool init(void);
    /**
     * @js NA
     * @lua NA
     */
    virtual long getClassTypeInfo() {
        static const long id = cocos2d::getHashCodeByString(typeid(cocos2d::CCDirector).name());
        return id;
    }

    // attribute

    /** Get current running Scene. Director can only run one Scene at the time */
    inline CCScene* getRunningScene(void) { return m_pRunningScene; }

    /** Get the FPS value */
    inline double getAnimationInterval(void) { return m_dAnimationInterval; }
    /** Set the FPS value. */
    virtual void setAnimationInterval(double dValue) = 0;

    /** Whether or not to display the FPS on the bottom-left corner */
    inline bool isDisplayStats(void) { return m_bDisplayStats; }
    /** Display the FPS on the bottom-left corner */
    inline void setDisplayStats(bool bDisplayStats) { m_bDisplayStats = bDisplayStats; }

    /** seconds per frame */
    inline float getSecondsPerFrame() { return m_fSecondsPerFrame; }

    /** Get the CCEGLView, where everything is rendered
     * @js NA
     */
    inline CCEGLView* getOpenGLView(void) { return m_pobOpenGLView; }
    void setOpenGLView(CCEGLView* pobOpenGLView);

    inline bool isNextDeltaTimeZero(void) { return m_bNextDeltaTimeZero; }
    void setNextDeltaTimeZero(bool bNextDeltaTimeZero);

    /** Whether or not the Director is paused */
    inline bool isPaused(void) { return m_bPaused; }

    /** How many frames were called since the director started */
    inline unsigned int getTotalFrames(void) { return m_uTotalFrames; }

    /** Sets an OpenGL projection
     @since v0.8.2
     @js NA
     */
    inline ccDirectorProjection getProjection(void) { return m_eProjection; }
    void setProjection(ccDirectorProjection kProjection);
    /** reshape projection matrix when canvas has been change"*/
    void reshapeProjection(const CCSize& newWindowSize);

    /** Sets the glViewport*/
    void setViewport();

    /** How many frames were called since the director started */


    /** Whether or not the replaced scene will receive the cleanup message.
     If the new scene is pushed, then the old scene won't receive the "cleanup" message.
     If the new scene replaces the old one, the it will receive the "cleanup" message.
     @since v0.99.0
     */
    inline bool isSendCleanupToScene(void) { return m_bSendCleanupToScene; }

    /** This object will be visited after the main scene is visited.
     This object MUST implement the "visit" selector.
     Useful to hook a notification object, like CCNotifications (http://github.com/manucorporat/CCNotifications)
     @since v0.99.5
     */
    CCNode* getNotificationNode();
    void setNotificationNode(CCNode* node);

    /** CCDirector delegate. It shall implemente the CCDirectorDelegate protocol
     @since v0.99.5
     */
    CCDirectorDelegate* getDelegate() const;
    void setDelegate(CCDirectorDelegate* pDelegate);

    // window size

    /** returns the size of the OpenGL view in points.
    */
    CCSize getWinSize(void);

    /** returns the size of the OpenGL view in pixels.
    */
    CCSize getWinSizeInPixels(void);

    /** returns visible size of the OpenGL view in points.
     *  the value is equal to getWinSize if don't invoke
     *  CCEGLView::setDesignResolutionSize()
     */
    CCSize getVisibleSize();

    /** returns visible origin of the OpenGL view in points.
     */
    CCPoint getVisibleOrigin();

    /** converts a UIKit coordinate to an OpenGL coordinate
     Useful to convert (multi) touch coordinates to the current layout (portrait or landscape)
     */
    CCPoint convertToGL(const CCPoint& obPoint);

    /** converts an OpenGL coordinate to a UIKit coordinate
     Useful to convert node points to window points for calls such as glScissor
     */
    CCPoint convertToUI(const CCPoint& obPoint);

    /// XXX: missing description 
    float getZEye(void);

    // Scene Management

    /** Enters the Director's main loop with the given Scene.
     * Call it to run only your FIRST scene.
     * Don't call it if there is already a running scene.
     *
     * It will call pushScene: and then it will call startAnimation
     */
    void runWithScene(CCScene* pScene);

    /** Suspends the execution of the running scene, pushing it on the stack of suspended scenes.
     * The new scene will be executed.
     * Try to avoid big stacks of pushed scenes to reduce memory allocation.
     * ONLY call it if there is a running scene.
     */

     //Robtop Modification: changed return type to BOOL
    bool pushScene(CCScene* pScene);

    /** Pops out a scene from the queue.
     * This scene will replace the running one.
     * The running scene will be deleted. If there are no more scenes in the stack the execution is terminated.
     * ONLY call it if there is a running scene.
     */
    void popScene(void);

    /** Pops out all scenes from the queue until the root scene in the queue.
     * This scene will replace the running one.
     * Internally it will call `popToSceneStackLevel(1)`
     */
    void popToRootScene(void);

    /** Pops out all scenes from the queue until it reaches `level`.
     If level is 0, it will end the director.
     If level is 1, it will pop all scenes until it reaches to root scene.
     If level is <= than the current stack level, it won't do anything.
     */
    void popToSceneStackLevel(int level);

    /** Replaces the running scene with a new one. The running scene is terminated.
     * ONLY call it if there is a running scene.
     */
     //Robtop Modification: changed return type to BOOL
    bool replaceScene(CCScene* pScene);

    /** Ends the execution, releases the running scene.
     It doesn't remove the OpenGL view from its parent. You have to do it manually.
     */
    void end(void);

    /** Pauses the running scene.
     The running scene will be _drawed_ but all scheduled timers will be paused
     While paused, the draw rate will be 4 FPS to reduce CPU consumption
     */
    void pause(void);

    /** Resumes the paused scene
     The scheduled timers will be activated again.
     The "delta time" will be 0 (as if the game wasn't paused)
     */
    void resume(void);

    /** Stops the animation. Nothing will be drawn. The main loop won't be triggered anymore.
     If you don't want to pause your animation call [pause] instead.
     */
    virtual void stopAnimation(void) = 0;

    /** The main loop is triggered again.
     Call this function only if [stopAnimation] was called earlier
     @warning Don't call this function to start the main loop. To run the main loop call runWithScene
     */
    virtual void startAnimation(void) = 0;

    /** Draw the scene.
    This method is called every frame. Don't call it manually.
    */
    void drawScene(void);

    // Memory Helper

    /** Removes cached all cocos2d cached data.
     It will purge the CCTextureCache, CCSpriteFrameCache, CCLabelBMFont cache
     @since v0.99.3
     */
    void purgeCachedData(void);

    /** sets the default values based on the CCConfiguration info */
    void setDefaultValues(void);

    // OpenGL Helper

    /** sets the OpenGL default values */
    void setGLDefaultValues(void);

    /** enables/disables OpenGL alpha blending */
    void setAlphaBlending(bool bOn);

    /** enables/disables OpenGL depth test */
    void setDepthTest(bool bOn);

    virtual void mainLoop(void) = 0;

    /** The size in pixels of the surface. It could be different than the screen size.
    High-res devices might have a higher surface size than the screen size.
    Only available when compiled using SDK >= 4.0.
    @since v0.99.4
    */
    void setContentScaleFactor(float scaleFactor);
    float getContentScaleFactor(void);
private:
    float m_fScreenScaleFactor;    //0x24
    float m_fScreenScaleFactorMax; //0x28
    float m_fScreenScaleFactorW;   //0x2C
    float m_fScreenScaleFactorH;   //0x30
    float m_fScreenTop;            //0x34
    float m_fScreenBottom;         //0x38
    float m_fScreenLeft;           //0x3C
    float m_fScreenRight;          //0x40
    uint32_t m_uSceneReference;    //0x44

public:
    /** CCScheduler associated with this director
     @since v2.0
     */
    CC_PROPERTY(CCScheduler*, m_pScheduler, Scheduler);  //0x48

    /** CCActionManager associated with this director
     @since v2.0
     */
    CC_PROPERTY(CCActionManager*, m_pActionManager, ActionManager); //0x4C

    /** CCTouchDispatcher associated with this director
     @since v2.0
     */
    CC_PROPERTY(CCTouchDispatcher*, m_pTouchDispatcher, TouchDispatcher); //0x50

    /** CCKeypadDispatcher associated with this director
     @since v2.0
     */
    CC_PROPERTY(CCKeypadDispatcher*, m_pKeypadDispatcher, KeypadDispatcher); //0x54

    //Robtop Modification: add getter/setter macros for CCKeyboardDispatcher
    CC_PROPERTY(CCKeyboardDispatcher*, m_pKeyboardDispatcher, KeyboardDispatcher); //0x58

    //Robtop Modification: add getter/setter macros for CCMouseDispatcher
    CC_PROPERTY(CCMouseDispatcher*, m_pMouseDispatcher, MouseDispatcher); //0x5C


    /** CCAccelerometer associated with this director
     @since v2.0
     @js NA
     @lua NA
     */
    CC_PROPERTY(CCAccelerometer*, m_pAccelerometer, Accelerometer); //0x60
protected:
    float m_fDeltaTime; //0x64
public:
    /* delta time since last tick to main loop */
    virtual float getDeltaTime(void);
    void setDeltaTime(float);
    
    //CC_PROPERTY(float, m_fDeltaTime, DeltaTime); //0x64

    CC_PROPERTY_NOVIRTUAL(float, m_fActualDeltaTime, ActualDeltaTime); //0x68;

    //Hello there
    ROB_CC_SYNTHESIZE(bool, m_bIsTransitioning, IsTransitioning) //0x6C;
    

public:
    /** returns a shared instance of the director
     *  @js getInstance
     */
    static CCDirector* sharedDirector(void);

protected:
    
    void purgeDirector();
    bool m_bPurgeDirecotorInNextLoop; // this flag will be set to true in end() //0x70

    void setNextScene(void);

    void showStats();
    void createStatsLabel();
    void calculateMPF();
    void getFPSImageData(unsigned char** datapointer, unsigned int* length);

    /** calculates delta time since last time it was called */
    void calculateDeltaTime();
protected:
    uint32_t m_uPadding00[2]; //0x74
    /* The CCEGLView, where everything is rendered */
    CCEGLView* m_pobOpenGLView; //0x78
    
    uint32_t m_uPadding01; //0x7C
    
    double m_dAnimationInterval; //0x80
    double m_dOldAnimationInterval; //0x88

    /* landscape mode ? */
    bool m_bLandscape; //0x89, struct alignment takes care of the rest
    //uint32_t m_uPadding02; //0x8D

    bool m_bDisplayStats; //0x91
    float m_fAccumDt;     //0x94, struct alignment
    float m_fFrameRate;   //0x98

    CCLabelAtlas* m_pFPSLabel;   //0x9C
    CCLabelAtlas* m_pSPFLabel;   //0xA0
    CCLabelAtlas* m_pDrawsLabel; //0xA4

    /** Whether or not the Director is paused */
    bool m_bPaused; //0xA8

    /* How many frames were called since the director started */
    unsigned int m_uTotalFrames; //0xAC
    unsigned int m_uFrames;      //0xB0
    float m_fSecondsPerFrame;    //0xB4

    /* The running scene */
    CCScene* m_pRunningScene;   //0xB8

    /* will be the next 'runningScene' in the next frame
     nextScene is a weak reference. */
    CCScene* m_pNextScene;      //0xBC

    /* If YES, then "old" scene will receive the cleanup message */
    bool    m_bSendCleanupToScene; //0xC0

    /* scheduled scenes */
    CCArray* m_pobScenesStack; //0xC4

    /* last time the main loop was updated */
    struct cc_timeval* m_pLastUpdate; //0xC8

    /* whether or not the next delta time will be zero */
    bool m_bNextDeltaTimeZero; //0xCC

    /* projection used */
    ccDirectorProjection m_eProjection; //0xD0

    /* window size in points */
    CCSize    m_obWinSizeInPoints; //0xD4

    /* content scale factor */
    float    m_fContentScaleFactor; //0xDC

    /* store the fps string */
    char* m_pszFPS; //0xE0

    /* This object will be visited after the scene. Useful to hook a notification node */
    CCNode* m_pNotificationNode; //0xE4

    /* Projection protocol delegate */
    CCDirectorDelegate* m_pProjectionDelegate; //0xE8

    //I'm opting to replace this with a cocos macro
    //CCSceneDelegate* m_pSceneDelegate; //0xEC
    CC_PROPERTY_CONST(CCSceneDelegate*, m_pSceneDelegate, SceneDelegate);
protected:    
    CCSize m_sUnknown04; //0xF0  //These are referenced in CCDirector::updateScreenScale, no idea what they do
    CCSize m_sUnknown05; //0xF8
    
    TextureQuality m_eTextureQuality; //0x100
    bool m_bWillSwitchToScene;
    

    // CCEGLViewProtocol will recreate stats labels to fit visible rect
    friend class CCEGLViewProtocol;
public:
    //Robtop  Functions
    bool getDontCallWillSwitch(void) const;
    //bool getForceSmoothFix(void) const;
    //bool getIsTransitioning(void) const; 
    //CCKeyboardDispatcher* getKeyboardDispatcher(void); //Already taken care of
    TextureQuality getLoadedTextureQuality(void) const;
    //getMouseDispatcher(void); //Already taken care of
    CCScene* getNextScene(void);
    //getSceneDelegate(void); //Already taken care of
    CCScene* getSceneReference(void) const;
    float getScreenBottom(void);
    float getScreenLeft(void);
    float getScreenRight(void);
    float getScreenScaleFactor(void);
    float getScreenScaleFactorH(void);
    float getScreenScaleFactorMax(void);
    float getScreenScaleFactorW(void);
    float getScreenTop(void);
    bool getSmoothFix(void) const;
    bool getSmoothFixCheck(void) const;
    int getSmoothFixCounter(void) const;
    int levelForSceneInStack(CCScene*);
    bool popSceneWithTransition(float, PopTransition);
    int sceneCount(void);
    //void setActualDeltaTime(float); //Already taken care of
    //void setDeltaTime(float);       //Already taken care of
    void setForceSmoothFix(bool);
    //setKeyboardDispatcher(CCKeyboardDispatcher*)	//Already taken care of
    //setMouseDispatcher(CCMouseDispatcher*)	    //Already taken care of
    //setSceneDelegate(CCSceneDelegate*)	        //Already taken care of
    void setSceneReference(CCScene*);
    void setSmoothFix(bool);
    void setSmoothFixCheck(bool);
    void setupScreenScale(CCSize, CCSize, TextureQuality);
    void updateContentScale(TextureQuality);
    void updateScreenScale(CCSize);
    void willSwitchToScene(CCScene*);
};

/**
 @brief DisplayLinkDirector is a Director that synchronizes timers with the refresh rate of the display.

 Features and Limitations:
  - Scheduled timers & drawing are synchronizes with the refresh rate of the display
  - Only supports animation intervals of 1/60 1/30 & 1/15

 @since v0.8.2
 @js NA
 @lua NA
 */
class CCDisplayLinkDirector : public CCDirector
{
public:
    CCDisplayLinkDirector(void)
        : m_bInvalid(false)
    {}

    virtual void mainLoop(void);
    virtual void setAnimationInterval(double dValue);
    virtual void startAnimation(void);
    virtual void stopAnimation();

protected:
    bool m_bInvalid;
};

// end of base_node group
/// @}

NS_CC_END

#endif // __CCDIRECTOR_H__
