#ifndef __shmup_project__sfxState__
#define __shmup_project__sfxState__

#include "engine.h"
#include "menus.h"
#include "selector.h"

class sfxState : public cState {
public:
    sfxState(cEngine*);
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    
private:
    enum class MenuID { left, cancel, defaults };
    enum class Mact { up, down, left, right, enter, nothing,
                      left_released, right_released };
    
    void            reset();
    void            processEvents();    // will update menu as needed
    void            menuChosen();
    void            render();
    void            select(SAID, std::string);
    
    cMenu           mMenu1;
    cMenu           mMenu2;
    cMenu           mMenu3;
    sf::Texture     mScreen;
    sf::Sprite      mSprite;
    sf::View        mView;
    MenuID          mActive;
    Mact            m;
    
    cSelector       mMusicSel;      // selector for music volume
    cSelector       mSFXSel;        // for sfx volume
    cSelector       mBrightSel;     // for bgnd brightness
    
    bool            mJoyMoved;
    bool            mSelected;
    sf::Time        mStillBlinking;
    SAID            mAction;
    std::string     mParam;
    sf::Clock       mClock;

    unsigned short int lID, rID;      // to know which selector was adjusted
};
#endif /* defined(__shmup_project__sfxState__) */
