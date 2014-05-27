// The "difficulty select" state

#include "diffState.h"
#include "globalVariables.h"

cDiffState::cDiffState(cEngine* p):
cState { p, "diff" }
{
    std::vector<std::string> stmp;
    stmp.push_back("Easy");
    stmp.push_back("Medium");
    stmp.push_back("Hard");
    mDiffMenu = cMenu { stmp, mFontHolder.get(FontID::smallfont), 24, sf::Vector2f(550,400) };
    mDiffMenu.setUpSoundQueue(&mSoundQueue);
    reset();
}

void cDiffState::init()
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

void cDiffState::reset()
{
    mDiffMenu.setActiveID(0);
    mDiffMenu.setDrawHighlight(true);
    mSelected = false;
    mStillBlinking = sf::seconds(gMenuBlink);
}

void cDiffState::select(SAID action, std::string param)
{
    mAction = action;
    mParam = param;
    mSelected = true;
    mClock.restart();   // let the blinking begin!
}

void cDiffState::menuChosen()
{
    switch ( mDiffMenu.getActiveID() )
    {
        case 0: { pEngine->mDifficulty = Difficulty::easy; break; }
        case 1: { pEngine->mDifficulty = Difficulty::medium; break; }
        case 2: { pEngine->mDifficulty = Difficulty::hard; break; }
    }
    mSoundQueue.push(SfxID::menuselect);
    select(SAID::swap, "game");
}

void cDiffState::processEvents()
{
    if ( mSelected )
    {
        if ( mStillBlinking > sf::Time::Zero )  // if still blinking, then let's blink!
        {
            mDiffMenu.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2);
            mStillBlinking -= mClock.restart();
        }
        else // blinking time over, let's actually do stuff!
        {
            switch (mAction) {
                case SAID::swap:
                {
                    swapRequest(mParam);
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
                mDiffMenu.move_down();
                mJoyMoved = true;
            }
            if ( jPos < -gJoyThreshold && !mJoyMoved)
            {
                mDiffMenu.move_up();
                mJoyMoved = true;
            }
            if ( jPos > -gJoyThreshold && jPos < gJoyThreshold)
                mJoyMoved = false;
        }
        
        if ( event.type == sf::Event::JoystickButtonPressed )
        {
            sf::Uint8 button = event.joystickButton.button;
            if ( button == mJ.Fire ) menuChosen();
            if ( button == mJ.UpButton ) mDiffMenu.move_up();
            if ( button == mJ.DownButton ) mDiffMenu.move_down();
        }
        
        if (event.type == sf::Event::KeyPressed)
            switch (event.key.code)
        {
            case sf::Keyboard::Up:
            {
                mDiffMenu.move_up();
                break;
            }
            case sf::Keyboard::Down:
            {
                mDiffMenu.move_down();
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

void cDiffState::render()
{
    mWindow.setView(mView);
    mDiffMenu.render(mWindow, sf::RenderStates::Default);
}

void cDiffState::run()
{
    processEvents();
}

void cDiffState::cleanup()
{
    
}