#ifndef __shmup_project__pauseState__
#define __shmup_project__pauseState__

#include "engine.h"
#include "menus.h"

class cPauseState : public cState {
public:
    cPauseState(cEngine*);
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    
private:
    void            reset();
    void            processEvents();    // will update menu as needed
    void            render();
    void            menuChosen();
    void            select(SAID, std::string);
    
    cMenu           mPauseMenu;
    sf::Texture     mPauseScreen;
    sf::Sprite      mSprite;
    sf::View        mView;
    bool            mJoyMoved;
    bool            mSelected;
    sf::Time        mStillBlinking;
    sf::Clock       mClock;
    SAID            mAction;
    std::string     mParam;
};

#endif /* defined(__shmup_project__pauseState__) */
