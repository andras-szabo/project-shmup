#ifndef __shmup_project__menuState__
#define __shmup_project__menuState__

#include "engine.h"
#include "menus.h"

class cMenuState : public cState {
public:
    cMenuState(cEngine*);
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    virtual void    reset();
    virtual void    render();
    
private:
    void            processEvents();    // will update menu as needed
    void            menuChosen();       // a menu item was chosen; it's put in a separate
                                        // member so as not to repeat code when chosen by
                                        // keyboard or mouse or joy
    void            select(SAID, std::string);  // let's activate a (hilighted) menu point
    
    cMenu           mMainMenu;
    sf::Texture     mMenuScreen;
    sf::Sprite      mSprite;
    sf::View        mView;
    
    bool            mJoyMoved;

    bool            mSelected;
    sf::Time        mStillBlinking;
    sf::Clock       mClock;             // for measuring blinking times
    SAID            mAction;
    std::string     mParam;
};

#endif /* defined(__shmup_project__menuState__) */
