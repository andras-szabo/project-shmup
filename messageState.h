#ifndef __shmup_project__messageState__
#define __shmup_project__messageState__

#include "engine.h"
#include "menus.h"

// A "message state:" pauses the gameplay and slides in a textbox
// with some message. Used e.g. in the tutorial.

class cMessageState : public cState {
public:
    cMessageState(cEngine*);
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    
private:
    void            reset();
    void            processEvents();    // will update menu as needed
    void            render();
    void            update(sf::Time);

    sf::RectangleShape  mBox;
    sf::Text            mText;
    sf::View            mView;
    sf::Vector2f        mCenterPos;
    
    sf::Clock           mClock;
    sf::Time            mTimeSinceLastUpdate;
    
    TextStatus          mStatus;
};


#endif /* defined(__shmup_project__messageState__) */
