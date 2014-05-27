#include "optionState.h"
#include "globalVariables.h"

cOptionState::cOptionState(cEngine* p):
cState { p, "option" }
{
    std::vector<std::string> stmp;
    stmp.push_back("Choose\ncontroller");
    stmp.push_back("Configure\nkeys\\buttons");
    stmp.push_back("Sound and\ngraphics");
    stmp.push_back("Done");
    mOpMenu = cMenu { stmp, mFontHolder.get(FontID::smallfont), 24, sf::Vector2f(50,300) };
    mOpMenu.setUpSoundQueue(&mSoundQueue);
    reset();
}

void cOptionState::init()
{
    // load background image (something simple to fade out the existing view
    
    mView.setSize(1200, 800);
    mView.setViewport(sf::FloatRect { 0, 0, 1, 1 });
    mView.setCenter(600, 400);
    mWindow.setView(mView);
    
    mSprite.setTexture(mTextureHolder.get(TexID::fadescreen));
    mSprite.setColor(sf::Color(200, 200, 200, 120));
    mSprite.setPosition(0,0);
}

void cOptionState::reset()
{
    mOpMenu.setActiveID(0);
    mOpMenu.setDrawHighlight(true);
    mSelected = false;
    mStillBlinking = sf::seconds(gMenuBlink);
}

void cOptionState::select(SAID action, std::string param)
{
    mAction = action;
    mParam = param;
    mSelected = true;
    mClock.restart();   // let the blinking begin!
}

void cOptionState::menuChosen()
{
    mSoundQueue.push(SfxID::menuselect);
    switch ( mOpMenu.getActiveID() )
    {
        case 0: { select(SAID::push, "kj"); break; }
        case 1: { select(SAID::push, "cfg"); break; }
        case 2: { select(SAID::push, "sfx"); break; }
        case 3: { select(SAID::pop, " "); break; }
    }
}

void cOptionState::processEvents()
{
    if ( mSelected )
    {
        if ( mStillBlinking > sf::Time::Zero )  // if still blinking, then let's blink!
        {
            mOpMenu.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2);
            mStillBlinking -= mClock.restart();
        }
        else // blinking time over, let's actually do stuff!
        {
            mOpMenu.setDrawHighlight(false);
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
                mOpMenu.move_down();
                mJoyMoved = true;
            }
            if ( jPos < -gJoyThreshold && !mJoyMoved)
            {
                mOpMenu.move_up();
                mJoyMoved = true;
            }
            if ( jPos > -gJoyThreshold && jPos < gJoyThreshold)
                mJoyMoved = false;
        }
        
        if ( event.type == sf::Event::JoystickButtonPressed )
        {
            sf::Uint8 button = event.joystickButton.button;
            if ( button == mJ.Fire ) menuChosen();
            if ( button == mJ.UpButton ) mOpMenu.move_up();
            if ( button == mJ.DownButton ) mOpMenu.move_down();
        }
        
        if (event.type == sf::Event::KeyPressed)
            switch (event.key.code)
        {
            case sf::Keyboard::Up:
            {
                mOpMenu.move_up();
                break;
            }
            case sf::Keyboard::Down:
            {
                mOpMenu.move_down();
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

void cOptionState::render()
{
    mWindow.setView(mView);
    mWindow.draw(mSprite);
    mOpMenu.render(mWindow, sf::RenderStates::Default);
}

void cOptionState::run()
{
    processEvents();
}

void cOptionState::cleanup()
{
    
}