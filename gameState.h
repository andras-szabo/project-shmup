#ifndef __shmup_project__gameState__
#define __shmup_project__gameState__

#include "engine.h"
#include "commands.h"
#include "gameWorld.h"
#include "resourceHolders.h"

class cGameState : public cState {
public:
    cGameState(cEngine*);
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    virtual void    reset();
    virtual void    render();
    
private:
    void            processEvents();
    void            update(sf::Time);
    void            putFPS();
    
    CQtype          mCQ;        // master Command Queue
    ControlType&    mControl;
    Difficulty&     mDifficulty;
    
    GameWorld       mWorld;
    
    sf::Vector2f    mPastInput;
    sf::Vector2f    mCurrentInput;
    sf::Time        mTimeSinceLastRender;
    sf::Time        mTimeSinceLastUpdate;
    
    // We utilize 2 Clocks. One in the actual game loop that
    // restarts with every update, and, independent of that,
    // there's another clock that we use to count rendered
    // frames every second, in order to measure FPS.
    
    sf::Clock       mFPSclock;
    sf::Clock       mClock;
    unsigned int    mCurrentFPS;
    unsigned int    mPastFPS;
    sf::View        mView;
    
    sf::Text        mFPStext;
    
    short int       mGamma;
    std::string     mStageName;

};

#endif /* defined(__shmup_project__gameState__) */
