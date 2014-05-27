#ifndef __shmup_project__introState__
#define __shmup_project__introState__

#include "engine.h"

class cIntroState : public cState {
public:
    cIntroState(cEngine*);
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    virtual void    reset();
    virtual void    render();
    
private:
    sf::Clock       mClock;
    sf::Time        mFadeInTimer;
    sf::Time        mProceedTimer;
    sf::Time        mTimePassed;
    sf::Texture     mInitScreen;
    sf::Sprite      mSprite;
    sf::View        mView;
};


#endif /* defined(__shmup_project__introState__) */
