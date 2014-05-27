// The "difficulty select" state

#ifndef __shmup_project__diffState__
#define __shmup_project__diffState__

#include "engine.h"
#include "menus.h"

class cDiffState : public cState {
public:
    cDiffState(cEngine*);
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    
private:
    void            reset();
    void            processEvents();    // will update menu as needed
    void            menuChosen();
    void            render();
    void            select(SAID, std::string);
    
    cMenu           mDiffMenu;
    sf::Texture     mDiffScreen;
    sf::Sprite      mSprite;
    sf::View        mView;

    bool            mJoyMoved;
    bool            mSelected;
    sf::Time        mStillBlinking;
    SAID            mAction;
    std::string     mParam;
    sf::Clock       mClock;
};


#endif /* defined(__shmup_project__diffState__) */
