// KJ, as in, Keyboard or Joystick?

#include "kjState.h"
#include "globalVariables.h"

ckjState::ckjState(cEngine* p):
cState { p, "kj" }
{
    std::vector<std::string> stmp;
    stmp.push_back("Keyboard");
    stmp.push_back("Joystick / gamepad");
    mMenu = cMenu { stmp, mFontHolder.get(FontID::smallfont), 24, sf::Vector2f(500,350) };
    mMenu.setUpSoundQueue(&mSoundQueue);
    reset();
}

void ckjState::init()
{
    // load background image (something simple to fade out the existing view
    
    mView.setSize(1200, 800);
    mView.setViewport(sf::FloatRect { 0, 0, 1, 1 });
    mView.setCenter(600, 400);
    mWindow.setView(mView);
    
    mSprite.setTexture(mTextureHolder.get(TexID::fadescreen));
    mSprite.setColor(sf::Color(100, 100, 100, 120));
    mSprite.setPosition(0,0);
}

void ckjState::reset()
{
    // Setting up initial state on the basis of current control type.
    // Could be expanded w/ support for several joysticks.
    if ( mControl == ControlType::keyboard )
        mMenu.setActiveID(0);
    else
        mMenu.setActiveID(1);
    
    mMenu.setDrawHighlight(true);
    mSelected = false;
    mStillBlinking = sf::seconds(gMenuBlink);
}

void ckjState::select(SAID action, std::string param)
{
    mAction = action;
    mParam = param;
    mSelected = true;
    mClock.restart();   // let the blinking begin!
}

void ckjState::menuChosen()
{
    switch ( mMenu.getActiveID() )
    {
        case 0: { pEngine->mControl = ControlType::keyboard; break; }
        case 1: { pEngine->mControl = ControlType::gamepad; break; }
    }
    mSoundQueue.push(SfxID::menuselect);
    select(SAID::pop, "");
}

void ckjState::processEvents()
{
    if ( mSelected )
    {
        if ( mStillBlinking > sf::Time::Zero )  // if still blinking, then let's blink!
        {
            mMenu.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2);
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
        
        if ( event.type == sf::Event::JoystickMoved && event.joystickMove.axis == mJ.UpDownAxis)
        {
            float jPos = event.joystickMove.position;
            if ( jPos > gJoyThreshold && !mJoyMoved)
            {
                mMenu.move_down();
                mJoyMoved = true;
            }
            if ( jPos < -gJoyThreshold && !mJoyMoved)
            {
                mMenu.move_up();
                mJoyMoved = true;
            }
            if ( jPos > -gJoyThreshold && jPos < gJoyThreshold)
                mJoyMoved = false;
        }
        
        if ( event.type == sf::Event::JoystickButtonPressed )
        {
            sf::Uint8 button = event.joystickButton.button;
            if ( button == mJ.Fire ) menuChosen();
            if ( button == mJ.UpButton ) mMenu.move_up();
            if ( button == mJ.DownButton ) mMenu.move_down();
        }
        
        if (event.type == sf::Event::KeyPressed)
            switch (event.key.code)
        {
            case sf::Keyboard::Up:
            {
                mMenu.move_up();
                break;
            }
            case sf::Keyboard::Down:
            {
                mMenu.move_down();
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

void ckjState::render()
{
    mWindow.setView(mView);
    mWindow.draw(mSprite);
    mMenu.render(mWindow, sf::RenderStates::Default);
}

void ckjState::run()
{
    processEvents();
}

void ckjState::cleanup()
{
    
}