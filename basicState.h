// This is the base and parent of all state classes.
// It's set up by initializing a pointer to the Engine,
// references to the Engine's basic resources (gamewindow,
// fontholder, textureholder, sound and music queues),
// and to variables that should exist only in one copy
// (for the engine), but which should be accessible to
// all states - namely, variables describing control
// settings (keyboard or joystick; particular control keys)

#ifndef __shmup_project__basicState__
#define __shmup_project__basicState__

#include "engine.h"
#include "keyBindings.h"
#include "joyBindings.h"

class cEngine;

class cState {
public:
    cState(cEngine*, const std::string&);
    virtual void    init() = 0;
    virtual void    run() = 0;
    virtual void    cleanup() = 0;
    virtual void    reset() = 0;        // launched when popping back from another state
    virtual void    render() = 0;

    void            deactivate() { mIsActive = false; }
    
protected:
    void        pushRequest(std::string);
    void        popRequest();
    void        swapRequest(std::string);
    void        returnRequest(std::string);
    void        clearRequest();
    
protected:
    cEngine*                            pEngine;
    sf::RenderWindow&                   mWindow;            // References to the engine's resources
    ResourceHolder<sf::Font, FontID>&   mFontHolder;        // (renderwindow, fonts, textures); the idea
    ResourceHolder<sf::Texture, TexID>& mTextureHolder;     // being that some resources are needed globally
    bool                                mIsActive;          // (but States can also have their own respective
                                                            // resource holders)
    cBindings&                          mK;
    jBindings&                          mJ;
    ControlType&                        mControl;
    std::queue<SfxID>&                  mSoundQueue;
    std::queue<MusicCommand>&           mMusicQueue;
    
public:
    std::string                         mName;
};


#endif /* defined(__shmup_project__basicState__) */
