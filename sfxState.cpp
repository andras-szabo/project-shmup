// State to mess with sound effects and the like

#include "sfxState.h"
#include "globalVariables.h"
#include "ResourcePath.hpp"
#include <fstream>

sfxState::sfxState(cEngine* p):
cState { p, "sfx" }
{
    std::vector<std::string> stmp;
    stmp.push_back("Music volume");
    stmp.push_back("Sound volume");
    stmp.push_back("Background\nbrightness");
    stmp.push_back("Apply");
    mMenu1 = cMenu { stmp, mFontHolder.get(FontID::smallfont), 24, sf::Vector2f(240,250) };
    mMenu1.setUpSoundQueue(&mSoundQueue);
    
    stmp.clear();
    stmp.push_back("Cancel");
    mMenu2 = cMenu { stmp, mFontHolder.get(FontID::smallfont), 24, sf::Vector2f(560,520) };
    mMenu2.setUpSoundQueue(&mSoundQueue);
    stmp.clear();
    stmp.push_back("Reset defaults");
    mMenu3 = cMenu { stmp, mFontHolder.get(FontID::smallfont), 24, sf::Vector2f(760, 520) };
    mMenu3.setUpSoundQueue(&mSoundQueue);
    reset();
    
    // Now initialize selectors

    // basic int-to-string
    auto i2s = [](unsigned int x, std::string scr)
    { do { char ctmp = (x % 10) + 48; scr = ctmp + scr; x /= 10; } while ( x > 0);
        return scr; };
    
    // Fill up stmp with numbers from 0 to 100
    stmp.clear();
    for (auto i = 0; i < 110; i += 10)
    {
        std::string s { "" };
        s = i2s(i,s);
        stmp.push_back(s);
    }
  
    // All selectors are the same here, except for their position.
    mMusicSel = cSelector { stmp, sf::Vector2f(530,250) } ;
    mMusicSel.setArrowTexture(mTextureHolder.get(TexID::GUItexture), gArrowRect);
    mMusicSel.setFont(mFontHolder.get(FontID::smallfont), 24);
    mMusicSel.setTextColor(sf::Color::White);
    mMusicSel.setBkColor(sf::Color::Black);

    mMusicSel.setActiveItem(pEngine->mMusicVolume / 10);
    
    mSFXSel = cSelector { stmp, sf::Vector2f(530, 350) };
    mSFXSel.setArrowTexture(mTextureHolder.get(TexID::GUItexture), gArrowRect);
    mSFXSel.setFont(mFontHolder.get(FontID::smallfont), 24);
    mSFXSel.setTextColor(sf::Color::White);
    mSFXSel.setBkColor(sf::Color::Black);
    
    mSFXSel.setActiveItem(pEngine->mSfxVolume / 10);
    
    mBrightSel = cSelector { stmp, sf::Vector2f(530, 450) };
    mBrightSel.setArrowTexture(mTextureHolder.get(TexID::GUItexture), gArrowRect);
    mBrightSel.setFont(mFontHolder.get(FontID::smallfont), 24);
    mBrightSel.setTextColor(sf::Color::White);
    mBrightSel.setBkColor(sf::Color::Black);

    mBrightSel.setActiveItem(pEngine->mGamma / 10);
    
    reset();
}

void sfxState::init()
{
    // load background image (something simple to fade out the existing view
    
    mView.setSize(1200, 800);
    mView.setViewport(sf::FloatRect { 0, 0, 1, 1 });
    mView.setCenter(600, 400);
    mWindow.setView(mView);
    
    mSprite.setTexture(mTextureHolder.get(TexID::fadescreen));
    mSprite.setColor(sf::Color(50, 50, 50, 180));
    mSprite.setPosition(0,0);
    
}

void sfxState::reset()
{
    mMenu1.setActiveID(0);
    mMenu1.setDrawHighlight(true);
    mSelected = false;
    mStillBlinking = sf::seconds(gMenuBlink);
    mMenu2.setActiveID(0);
    mMenu2.setDrawHighlight(false);
    mMenu3.setActiveID(0);
    mMenu3.setDrawHighlight(false);
    mActive = MenuID::left;
    lID = 99;
    rID = 99;
    mJoyMoved = false;
}

void sfxState::select(SAID action, std::string param)
{
    mAction = action;
    mParam = param;
    mSelected = true;
    mClock.restart();   // let the blinking begin!
}

void sfxState::menuChosen()
{
    mSoundQueue.push(SfxID::menuselect);
    if ( mActive == MenuID::left )      // then we need to apply changes
    {
        auto newGamma = mBrightSel.getStateID() * 10;
        auto newMVol = mMusicSel.getStateID() * 10;
        auto newSVol = mSFXSel.getStateID() * 10;
        
        // Only change things if things are - well - changed
        
        if ( newGamma != pEngine->mGamma ||
             newMVol != pEngine->mMusicVolume ||
             newSVol != pEngine->mSfxVolume)
        {
            pEngine->mGamma = mBrightSel.getStateID() * 10;
            pEngine->mMusicVolume = mMusicSel.getStateID() * 10;
            pEngine->mSfxVolume = mSFXSel.getStateID() * 10;
        
            std::cout << pEngine->mSfxVolume << "\n";
            
            // Commit changes to file
            std::ofstream out { resourcePath() + "settings.cfg" };
            out << "MUSIC_VOLUME " << pEngine->mMusicVolume << "\n";
            out << "SFX_VOLUME " << pEngine->mSfxVolume << "\n";
            out << "BRIGHTNESS " << pEngine->mGamma << "\n";
            out.close();
        
            pEngine->mVolumeChange = true;
        }
    }
    select(SAID::pop, "");
}

void sfxState::processEvents()
{
    if ( mSelected )
    {
        if ( mStillBlinking > sf::Time::Zero )  // if still blinking, then let's blink!
        {
            switch (mActive){
                case MenuID::left: mMenu1.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2); break;
                case MenuID::cancel: mMenu2.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2); break;
                case MenuID::defaults: mMenu3.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2); break;
            }
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
    }
        
    
    sf::Event event;
    m = Mact::nothing;
    while (mWindow.pollEvent(event))
    {
        // Close window : exit
        if (event.type == sf::Event::Closed) {
            clearRequest();
            return;
        }
        
        if ( event.type == sf::Event::JoystickMoved )
        {
            float jPos = event.joystickMove.position;
            if ( event.joystickMove.axis == mJ.UpDownAxis)
            {
                if ( jPos > gJoyThreshold && !mJoyMoved)
                {
                    m = Mact::down;
                    mJoyMoved = true;
                }
                if ( jPos < -gJoyThreshold && !mJoyMoved)
                {
                    m = Mact::up;
                    mJoyMoved = true;
                }
                
            }
            else // horizontal movement
            {
                if ( jPos > gJoyThreshold && !mJoyMoved )
                {
                    m = Mact::right;
                    mJoyMoved = true;
                }
                if ( jPos < -gJoyThreshold && !mJoyMoved )
                {
                    m = Mact::left;
                    mJoyMoved = true;
                }
            }
            
            if ( jPos > -gJoyThreshold && jPos < gJoyThreshold)
            {
                m = Mact::nothing;
                mJoyMoved = false;
            }
        }
        
        if ( event.type == sf::Event::JoystickButtonPressed )
        {
            sf::Uint8 button = event.joystickButton.button;
            if ( button == mJ.Fire ) m = Mact::enter;
            if ( button == mJ.UpButton ) m = Mact::up;
            if ( button == mJ.DownButton ) m = Mact::down;
            if ( button == mJ.LeftButton ) m = Mact::left;
            if ( button == mJ.RightButton ) m = Mact::right;
        }
        
        if ( event.type == sf::Event::JoystickButtonReleased )
        {
            sf::Uint8 button = event.joystickButton.button;
            if ( button == mJ.LeftButton ) m = Mact::left_released;
            if ( button == mJ.RightButton ) m = Mact::right_released;
        }
        
        // Refactor: so that we're able to move here also with
        // user-defined keys (WASD, for instance; throw out the
        // case and bring in ifs)
        
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Up:
                {
                    m = Mact::up;
                    break;
                }
                case sf::Keyboard::Down:
                {
                    m = Mact::down;
                    break;
                }
                case sf::Keyboard::Left:
                {
                    m = Mact::left;
                    break;
                }
                case sf::Keyboard::Right:
                {
                    m = Mact::right;
                    break;
                }
                case sf::Keyboard::Return:
                {
                    m = Mact::enter;
                    break;
                }
            }
            if (event.key.code == mK.Left )
                m = Mact::left;
            else if ( event.key.code == mK.Right )
                m = Mact::right;
            else if ( event.key.code == mK.Up )
                m = Mact::up;
            else if ( event.key.code == mK.Down )
                m = Mact::down;
            else if ( event.key.code == mK.Fire )
                m = Mact::enter;
        }
        
        
        if ( event.type == sf::Event::KeyReleased )
        {
            if ( event.key.code == sf::Keyboard::Left || event.key.code == mK.Left )
                m = Mact::left_released;
            if ( event.key.code == sf::Keyboard::Right || event.key.code == mK.Right )
                m = Mact::right_released;
        }
    }
    
    switch (m){
        case Mact::up:
        {
            if ( mActive == MenuID::left )
            {
                mMenu1.move_up();
                break;
            }
            if ( mActive == MenuID::cancel )
            {
                mMenu2.setDrawHighlight(false);
                mActive = MenuID::left;
                mMenu1.setDrawHighlight(true);
                mMenu1.setActiveID(mMenu1.getSize()-2); // one-before-last element
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            if ( mActive == MenuID::defaults )
            {
                mMenu3.setDrawHighlight(false);
                mActive = MenuID::left;
                mMenu1.setDrawHighlight(true);
                mMenu1.setActiveID(mMenu1.getSize()-2);
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            break;
        }
            
        case Mact::down:
        {
            if ( mActive == MenuID::left )
            {
                mMenu1.move_down();
                break;
            }
            if ( mActive == MenuID::cancel )
            {
                mMenu2.setDrawHighlight(false);
                mActive = MenuID::left;
                mMenu1.setDrawHighlight(true);
                mMenu1.setActiveID(0);
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            if ( mActive == MenuID::defaults )
            {
                mMenu3.setDrawHighlight(false);
                mActive = MenuID::left;
                mMenu1.setDrawHighlight(true);
                mMenu1.setActiveID(0);
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            break;
        }
            
        case Mact::right:
        {
            if ( mActive == MenuID::left )
                {
                    if ( mMenu1.getActiveID() == mMenu1.getSize()-1 )
                    {
                        rID = 99;
                        mMenu1.setDrawHighlight(false);
                        mActive = MenuID::cancel;
                        mMenu2.setDrawHighlight(true);
                        mMenu2.setActiveID(0);
                        mSoundQueue.push(SfxID::menumove);
                        break;
                    }
                    if ( rID == 99 && !mJoyMoved )                // not waiting for an RRelease
                    {
                        mSoundQueue.push(SfxID::selectorsound);
                        rID = mMenu1.getActiveID();
                        if ( rID == 0 )
                        mMusicSel.toggleArrow(1);
                        else if ( rID == 1 )
                        mSFXSel.toggleArrow(1);
                        else if ( rID == 2 )
                        mBrightSel.toggleArrow(1);
                        break;
                    }
                    break;
                }
            if ( mActive == MenuID::cancel )
            {
                rID = 99;
                mMenu2.setDrawHighlight(false);
                mMenu3.setDrawHighlight(true);
                mMenu3.setActiveID(0);
                mActive = MenuID::defaults;
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            if ( mActive == MenuID::defaults )
            {
                rID = 99;
                mMenu3.setDrawHighlight(false);
                mMenu1.setDrawHighlight(true);
                mMenu1.setActiveID(mMenu1.getSize()-1);
                mActive = MenuID::left;
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            break;
        }
            
        case Mact::left:
        {
            if ( mActive == MenuID::left)
            {
                if ( mMenu1.getActiveID() == mMenu1.getSize()-1 )
                {
                    lID = 99;
                    mMenu1.setDrawHighlight(false);
                    mActive = MenuID::defaults;
                    mMenu3.setDrawHighlight(true);
                    mMenu3.setActiveID(0);
                    mSoundQueue.push(SfxID::menumove);
                    break;
                }
                if ( lID == 99 && !mJoyMoved )                    // not waiting for an LRelease yet
                {
                    mSoundQueue.push(SfxID::selectorsound);
                    lID = mMenu1.getActiveID();
                    if ( lID == 0 )
                        mMusicSel.toggleArrow(0);
                    else if ( lID == 1 )
                        mSFXSel.toggleArrow(0);
                    else if ( lID == 2 )
                        mBrightSel.toggleArrow(0);
                    break;
                }
                break;
            }
            if ( mActive == MenuID::cancel )
            {
                lID = 99;
                mMenu2.setDrawHighlight(false);
                mMenu1.setDrawHighlight(true);
                mMenu1.setActiveID(mMenu1.getSize()-1);
                mActive = MenuID::left;
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            if ( mActive == MenuID::defaults )
            {
                lID = 99;
                mMenu3.setDrawHighlight(false);
                mMenu2.setDrawHighlight(true);
                mMenu2.setActiveID(0);
                mActive = MenuID::cancel;
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            break;
        }
            
        case Mact::right_released:
        {
            if ( mActive == MenuID::left )
            {
                if (rID != mMenu1.getActiveID() )   // this means that the right key was released after we moved away from that line
                {
                    switch (rID) {
                        case 0: mMusicSel.toggleArrow(1); break;
                        case 1: mSFXSel.toggleArrow(1); break;
                        case 2: mBrightSel.toggleArrow(1); break;
                    }
                    rID = 99;
                    break;
                }
                if ( mMenu1.getActiveID() == 0 )
                {
                    mMusicSel.toggleArrow(1);
                    mMusicSel.stepRight();
                    rID = 99;
                }
                else if ( mMenu1.getActiveID() == 1 )
                {
                    mSFXSel.toggleArrow(1);
                    mSFXSel.stepRight();
                    rID = 99;
                }
                else if ( mMenu1.getActiveID() == 2 )
                {
                    mBrightSel.toggleArrow(1);
                    mBrightSel.stepRight();
                    rID = 99;
                }
            }
            else
            {
                switch ( rID ) {
                    case 0: mMusicSel.toggleArrow(1); break;
                    case 1: mSFXSel.toggleArrow(1); break;
                    case 2: mBrightSel.toggleArrow(1); break;
                }
                rID = 99;
            }
            break;
        }
        
        case Mact::left_released:
        {
            if ( mActive == MenuID::left )
            {
                if (lID != mMenu1.getActiveID() )
                {
                    switch (lID) {
                        case 0: mMusicSel.toggleArrow(0); break;
                        case 1: mSFXSel.toggleArrow(0); break;
                        case 2: mBrightSel.toggleArrow(0); break;
                    }
                    lID = 99;
                    break;
                }
                if ( mMenu1.getActiveID() == 0 )
                {
                    mMusicSel.toggleArrow(0);
                    mMusicSel.stepLeft();
                    lID = 99;
                }
                else if ( mMenu1.getActiveID() == 1 )
                {
                    mSFXSel.toggleArrow(0);
                    mSFXSel.stepLeft();
                    lID = 99;
                }
                else if ( mMenu1.getActiveID() == 2 )
                {
                    mBrightSel.toggleArrow(0);
                    mBrightSel.stepLeft();
                    lID = 99;
                }
            }
            else
            {
                switch ( lID ) {
                    case 0: mMusicSel.toggleArrow(0); break;
                    case 1: mSFXSel.toggleArrow(0); break;
                    case 2: mBrightSel.toggleArrow(0); break;
                }
                lID = 99;
            }
            break;
        }
            
        case Mact::enter:
        {
            if ( (mActive == MenuID::left && mMenu1.getActiveID() == mMenu1.getSize()-1) || mActive == MenuID::cancel )
                menuChosen();
            else if (mActive == MenuID::defaults)
            {
                mSoundQueue.push(SfxID::menuselect);
                mMusicSel.setActiveItem(gDefMusicVolume / 10);
                mSFXSel.setActiveItem(gDefSFXVolume / 10);
                mBrightSel.setActiveItem(gDefGamma / 10);
            }
            break;
        }
    }
}

void sfxState::render()
{
    mWindow.setView(mView);
    mWindow.draw(mSprite);
    mMenu1.render(mWindow, sf::RenderStates::Default);
    mMenu2.render(mWindow, sf::RenderStates::Default);
    mMenu3.render(mWindow, sf::RenderStates::Default);
    mMusicSel.render(mWindow);
    mSFXSel.render(mWindow);
    mBrightSel.render(mWindow);
}

void sfxState::run()
{
    processEvents();
}

void sfxState::cleanup()
{
    
}