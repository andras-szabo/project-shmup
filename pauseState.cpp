#include "pauseState.h"
#include "globalVariables.h"

#include <iostream>

cPauseState::cPauseState(cEngine* p):
cState { p, "pause" }
{
    std::vector<std::string> stmp;
    stmp.push_back("Resume");
    stmp.push_back("Options");
    stmp.push_back("Quit");
    mPauseMenu = cMenu { stmp, mFontHolder.get(FontID::smallfont), 24, sf::Vector2f(550,400) };
    mPauseMenu.setUpSoundQueue(&mSoundQueue);
    reset();
}

void cPauseState::init()
{
    // load background image (something simple to fade out the existing view
    
    mView.setSize(1200, 800);
    mView.setViewport(sf::FloatRect { 0, 0, 1, 1 });
    mView.setCenter(600, 400);
    mWindow.setView(mView);
    
    mSprite.setTexture(mTextureHolder.get(TexID::fadescreen));
    mSprite.setColor(sf::Color(200, 200, 200, 120));
    mSprite.setPosition(0,0);
    
    mSoundQueue.push(SfxID::pausemenu);
    mMusicQueue.push(MusicCommand::pause);
}

void cPauseState::reset()
{
    mPauseMenu.setActiveID(0);
    mPauseMenu.setDrawHighlight(true);
    mSelected = false;
    mStillBlinking = sf::seconds(gMenuBlink);
}

void cPauseState::menuChosen()
{
    mSoundQueue.push(SfxID::menuselect);
    switch ( mPauseMenu.getActiveID() )
    {
        case 0: { select(SAID::pop, " "); break; }           // resume game
        case 1: { select(SAID::push, "option"); break; }     // go to options
        case 2: { select(SAID::return_to, "menu"); break; }  // back to main menu
    }
}

void cPauseState::select(SAID action, std::string param)
{
    mAction = action;
    mParam = param;
    mSelected = true;
    mClock.restart();   // let the blinking begin!
}

void cPauseState::processEvents()
{
    if ( mSelected )
    {
        if ( mStillBlinking > sf::Time::Zero )  // if still blinking, then let's blink!
        {
            mPauseMenu.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2);
            mStillBlinking -= mClock.restart();
        }
        else // blinking time over, let's actually do stuff!
        {
            switch (mAction) {
                case SAID::pop:
                {
                    popRequest();
                    break;
                }
                case SAID::push:
                {
                    pushRequest(mParam);
                    break;
                }
                case SAID::return_to:
                {
                    returnRequest(mParam);
                    break;
                }
            }
        }
        return;
    }
    
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        // Close window : exit
       if (event.type == sf::Event::Closed) {
           clearRequest();
           return;
        }

        if ( event.type == sf::Event::JoystickMoved && event.joystickMove.axis == mJ.UpDownAxis
            && event.joystickMove.joystickId == mJ.ID)
        {
            float jPos = event.joystickMove.position;
            if ( jPos > gJoyThreshold && !mJoyMoved)
            {
                mPauseMenu.move_down();
                mJoyMoved = true;
            }
            if ( jPos < -gJoyThreshold && !mJoyMoved)
            {
                mPauseMenu.move_up();
                mJoyMoved = true;
            }
            if ( jPos > -gJoyThreshold && jPos < gJoyThreshold)
                mJoyMoved = false;
        }
        
        if ( event.type == sf::Event::JoystickButtonPressed && event.joystickButton.joystickId == mJ.ID)
        {
            sf::Uint8 button = event.joystickButton.button;
            if ( button == mJ.Fire ) menuChosen();
            if ( button == mJ.UpButton ) mPauseMenu.move_up();
            if ( button == mJ.DownButton ) mPauseMenu.move_down();
        }

        if (event.type == sf::Event::KeyPressed)
            switch (event.key.code)
        {
            case sf::Keyboard::Up:
            {
                mPauseMenu.move_up();
                break;
            }
            case sf::Keyboard::Down:
            {
                mPauseMenu.move_down();
                break;
            }
            case sf::Keyboard::Return:
            {
                menuChosen();
                break;
            }
        }
    }
}

void cPauseState::render()
{
    mWindow.setView(mView);
    sf::RectangleShape bgShape;
    bgShape.setFillColor(sf::Color(0, 0, 0, 150));
    bgShape.setSize(sf::Vector2f(gViewSize.x+gSideBarSize.x*4, gViewSize.y));
    bgShape.setPosition(0,0);
    mWindow.setView(mView);
    mWindow.draw(bgShape);
    mPauseMenu.render(mWindow, sf::RenderStates::Default);
}

void cPauseState::run()
{
    processEvents();
}

void cPauseState::cleanup()
{
    mMusicQueue.push(MusicCommand::unpause);
}