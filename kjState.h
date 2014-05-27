#ifndef __shmup_project__kjState__
#define __shmup_project__kjState__

#include "engine.h"
#include "menus.h"

class ckjState : public cState {
public:
    ckjState(cEngine*);
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    
private:
    void            reset();
    void            processEvents();    // will update menu as needed
    void            menuChosen();
    void            render();
    void            select(SAID, std::string);
    
    cMenu           mMenu;
    sf::Texture     mScreen;
    sf::Sprite      mSprite;
    sf::View        mView;
    
    bool            mJoyMoved;
    bool            mSelected;
    sf::Time        mStillBlinking;
    SAID            mAction;
    std::string     mParam;
    sf::Clock       mClock;
};

#endif /* defined(__shmup_project__kjState__) */
