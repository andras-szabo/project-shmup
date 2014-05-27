#ifndef __shmup_project__optionState__
#define __shmup_project__optionState__

#include "engine.h"
#include "menus.h"

class cOptionState : public cState {
public:
    cOptionState(cEngine*);
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    
private:
    void            reset();
    void            processEvents();    // will update menu as needed
    void            menuChosen();
    void            render();
    void            select(SAID, std::string);
    
    cMenu           mOpMenu;
    sf::Texture     mOpScreen;
    sf::Sprite      mSprite;
    sf::View        mView;
    
    bool            mJoyMoved;
    bool            mSelected;
    sf::Time        mStillBlinking;
    SAID            mAction;
    std::string     mParam;
    sf::Clock       mClock;
};


#endif /* defined(__shmup_project__optionState__) */
