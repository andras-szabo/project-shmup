#include "introState.h"
#include "ResourcePath.hpp"

cIntroState::cIntroState(cEngine* p):
cState { p, "intro" },
mFadeInTimer { sf::seconds(1.5) },
mTimePassed { sf::seconds(0.0) },
mProceedTimer { sf::seconds(5.0) }
{
    /*
     In case we want to add something to cIntroState's constructor... ?
     */
}

void cIntroState::init()
{
    //if (!mInitScreen.loadFromFile(resourcePath() + "initscr.png"))
    //    throw std::runtime_error("Couldn't load init screen!");
    mSprite.setTexture(mTextureHolder.get(TexID::initscreen));
    mSprite.setColor(sf::Color(255, 255, 255, 0));
    mView.setSize(1200, 800);
    mView.setViewport(sf::FloatRect { 0, 0, 1, 1 });
    mView.setCenter(600, 400);
    mWindow.setView(mView);
}

void cIntroState::render()
{
        mWindow.draw(mSprite);
}

void cIntroState::run()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if ( event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed
            || event.type == sf::Event::JoystickButtonPressed)
        {
            swapRequest("menu");    // we quit into the menu state
            return;
        }
    }
    mTimePassed += mClock.restart();
    
    
    if ( mTimePassed >= mProceedTimer )
    {
        swapRequest("menu");        // we quit into the menu state
        return;
    }
    
    // If no need to proceed yet, do the fade-in (if needed)
    float fadeInRatio;
    if (mTimePassed < mFadeInTimer )
        fadeInRatio = mTimePassed.asSeconds() / mFadeInTimer.asSeconds() * 255;
    else fadeInRatio = 255.0;
    mSprite.setColor(sf::Color(255, 255, 255, fadeInRatio));
}

void cIntroState::cleanup()
{

}

void cIntroState::reset()
{
    
}