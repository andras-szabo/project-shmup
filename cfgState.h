#ifndef __shmup_project__cfgState__
#define __shmup_project__cfgState__

#include "engine.h"
#include "menus.h"

// So this is the game state for keyboard
// and sound and whatnot config. A lot of
// the goings-on here deal only with menus,
// not very interesting stuff tbh.

class cfgState : public cState {
public:
    cfgState(cEngine*);
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    
private:
   
    enum class MenuID { kb, joy, done };
    enum class Mact { up, down, left, right, enter, nothing };
                    // "Menu action" for short, thank you, yes, thank you
    
    void            reset();
    void            processEvents();    // will update menu as needed
    void            menuChosen();
    void            render();
    void            select(SAID, std::string);
    
    cMenu                   mMenu1;
    cMenu                   mMenu2;
    cMenu                   mMenu3;
    sf::Texture             mScreen;
    sf::Sprite              mSprite;
    sf::View                mView;
    
    std::vector<sf::Text>   mCaptions;
    
    bool                    mJoyMoved;
    bool                    mSelected;
    sf::Time                mStillBlinking;
    SAID                    mAction;
    std::string             mParam;
    sf::Clock               mClock;
    
    MenuID                  mActive;
    Mact                    m;
    
    bool                    mNewBinding;
    bool                    mKeyChanged;
    bool                    mJoyChanged;
    
    // mNewBinding starts out (in the constructor) as false.
    // When a new key/button mapping is done, it's done through
    // a cMessageState state, which returns a keyboard or
    // joy event through pEngine->mEvtParam. mNewBinding is
    // set to "true" just before calling that cMessageState,
    // signalling that there's some new binding to be done.
};

#endif /* defined(__shmup_project__cfgState__) */
