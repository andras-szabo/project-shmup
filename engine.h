#ifndef __shmup_project__engine__
#define __shmup_project__engine__

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include "resourceHolders.h"
#include "globalVariables.h"
#include "basicState.h"
#include "keyBindings.h"
#include <iostream>
#include "joyBindings.h"
#include "soundPlayer.h"
#include "musicPlayer.h"

class cState;   // fwd. decl.

class cEngine {
public:
    cEngine();
    ~cEngine() { };
    
public:
    void        run();
    void        request(SAID, std::string = " ");   // handles game state requests;
                                                    // SAID: Stack Action ID, see globalVariables.h
    void        writeAchievements();                // save achievements, hi score

    void            setStrParam(const std::string& s) { mStrParam = s; }
    void            setEvtParam(sf::Event e) { mEvtParam = e; }
    sf::Event       getEvtParam() const { return mEvtParam; }
    std::string     getStrParam() const { return mStrParam; }
    
    // Here come all the global variables that various states
    // can access through their pEngine-> reference.
public:
    sf::RenderWindow                    mWindow;
    sf::View                            mMenuView;
    ResourceHolder<sf::Font, FontID>    mFontHolder;
    ResourceHolder<sf::Texture, TexID>  mTextureHolder;
    Difficulty                          mDifficulty;
    ControlType                         mControl;
    cBindings                           k;
    jBindings                           j;
    std::queue<SfxID>                   mSoundQueue;
    std::queue<MusicCommand>            mMusicQueue;
    std::map<sf::Keyboard::Key, std::string>    mKeyMap;        // Let each char key correspond to a
                                                                // string; and also button presses
    std::map<unsigned int, std::string>         mJoyMap;        // on the joystick / gamepad
    
    short int                           mGamma;
    unsigned short int                  mMusicVolume;
    unsigned short int                  mSfxVolume;
    bool                                mVolumeChange;

    unsigned long int                   mHiScore;
    bool                                mBossTrainingUnlocked;
    bool                                mACPacifist;            // Pacifist achievement attained?
    bool                                mACMonochrome;          // Monochrome achievement attained?
    bool                                mACSurvivor;            // Survivor achievement attained?
    bool                                mRewrite;               // Do we need a re-saving of achievement data?
    
private:
    template <typename T>               // Creates an appropriate State and pushes it onto the
    void        launchState();          // state stack

    void        executeStackChanges();  // The dispatcher function that executes pop, push &c requests.
    void        pushState(const std::string&);
    void        popState();
    void        swapState(const std::string&);      // pop statestack, and push on new one
    void        clearStateStack();                  // pop all of state stack (=> quit)
    void        returnToState(const std::string&);  // pops as many states as needed in order to
                                                    // reach the designated state.

    cSoundPlayer                        mSoundPlayer;   // The sound player will be ruled by
                                                        // the Engine. If anyone wants to play a
                                                        // sound, it has to add it to the sound play
                                                        // queue (mSoundQueue);
    SfxID                               tSFX;           // temporary variable
    std::map<SfxID, int>                mPlayedAlready; // Which sounds did we already play this frame?
    
    cMusicPlayer                        mMusicPlayer;   // Music player will also be handled by
                                                        // the Engine.
    
    
private:
    std::vector<std::unique_ptr<cState>>    mStateStack;

    std::string                             mStrParam;
    sf::Event                               mEvtParam;
                    // OK this is a bit of a hack. These members are
                    // used to pass "parameters" to/from newly created
                    // states. E.g. a stage name could come in here,
                    // or a piece of text to be displayed.
    SAID                                    mStackAction;   // This is where we store the requested
    std::string                             mStackParam;    // stack actions.
};

#endif /* defined(__shmup_project__engine__) */
