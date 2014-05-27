#include "menuState.h"
#include "ResourcePath.hpp"

// Diagnostics only:
#include <iostream>

// So this is the state of main menu

cMenuState::cMenuState(cEngine* p):
cState { p, "menu" },
mSelected { false },
mStillBlinking { sf::seconds(gMenuBlink) }
{
    std::vector<std::string> stmp;
    stmp.push_back("New game");
    stmp.push_back("Options");
    stmp.push_back("How to play");
    stmp.push_back("Boss training");
    stmp.push_back("Achievements");
    stmp.push_back("Credits");
    stmp.push_back("Quit");
    mMainMenu = cMenu { stmp, mFontHolder.get(FontID::smallfont), 36, sf::Vector2f(510,320) };
    mMainMenu.setUpSoundQueue(&mSoundQueue);
}

void cMenuState::init()
{
    mWindow.clear();

    mWindow.setMouseCursorVisible(false);
    
    mView.setSize(1200, 800);
    mView.setViewport(sf::FloatRect { 0, 0, 1, 1 });
    mView.setCenter(600, 400);
    mWindow.setView(mView);

    // load background image
    mSprite.setTexture(mTextureHolder.get(TexID::menuscreen));
    mSprite.setColor(sf::Color(200, 200, 200, 120));
    
    mSoundQueue.push(SfxID::pausemenu);
    mMusicQueue.push(MusicCommand::playMenu);
}

void cMenuState::reset()
{
    mIsActive = true;
    mMainMenu.setActiveID(0);
    mSelected = false;
    mMainMenu.setDrawHighlight(true);
    mStillBlinking = sf::seconds(gMenuBlink);
    
    if ( pEngine->mRewrite == true )
        pEngine->writeAchievements();
}

void cMenuState::select(SAID action, std::string param)
{
    mAction = action;
    mParam = param;
    mSelected = true;
    mClock.restart();   // let the blinking begin!
}

void cMenuState::menuChosen()
{
    mSoundQueue.push(SfxID::menuselect);
    switch ( mMainMenu.getActiveID() )
    {
        case 0: {
            pEngine->setStrParam("stage01"); // set pEngine's "string register" to stage 1
            select(SAID::push, "diff");
            break;
        }
        case 1: {
            select(SAID::push, "options");
            break;
        }
        case 2: { // tutorial
            pEngine->setStrParam("tutorial");
            pEngine->mDifficulty = Difficulty::easy;
            select(SAID::push, "game");
            break;
        };
        case 3: {
            if ( pEngine->mBossTrainingUnlocked )
            {
                pEngine->setStrParam("testmap");
                select(SAID::push, "diff");
            }
            else
            {
                pEngine->setStrParam("You have to unlock\n this mode by meeting\n the boss first.\n");
                select(SAID::push, "message");
            }
            break;
        }
        case 4: {
            // Basic int to str conversion
            auto i2s = [](unsigned int x)
            { std::string scr; do { char ctmp = (x % 10) + 48; scr = ctmp + scr; x /= 10; } while ( x > 0);
                return scr; };
            
            std::string stmp;
            stmp = "High score: " + i2s(pEngine->mHiScore) + "\n\n";
            stmp += "Achievements:\n\nSurvivor - ";
            stmp += pEngine->mACSurvivor ? "completed!\n" : "to be completed\n";
            stmp += "Pacifist - ";
            stmp += pEngine->mACPacifist ? "completed!\n" : "to be completed\n";
            stmp += "Monochrome - ";
            stmp += pEngine->mACMonochrome ? "completed!\n" : "to be completed\n";
            pEngine->setStrParam(stmp);
            select(SAID::push, "message");
            break;
        }
        case 5 : {
            std::string stmp { "Code: Andras Szabo\n" };
            stmp += "   (andras.szabo.fin@gmail.com)\n";
            stmp += "\nMusic: Fabien Saubrement\n";
            stmp += "   (soundcloud.com/factuel)\n";
            stmp += "\nGraphics:\n    What looks good: Domonkos Kovacs\n";
            stmp += "           (kov.dom@gmail.com)\n";
            stmp += "    What doesn't: a.sz.\n";
            stmp += "\nThis game uses the SFML library\n";
            stmp += "    (c) Laurent Gomila, Marco Antognini\n";
            stmp += "    http://www.sfml-dev.org";
            stmp += "\n\n                       2013 - 2014.";
            pEngine->setStrParam(stmp);
            select(SAID::push, "message");
            break;
        }
        case 6: {
            select(SAID::clear, "");
            break;
        }
    }
}

void cMenuState::processEvents()
{
    if ( mSelected )
    {
        if ( mStillBlinking > sf::Time::Zero )  // if still blinking, then let's blink!
        {
            mMainMenu.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2);
            mStillBlinking -= mClock.restart();
        }
        else // blinking time over, let's actually do stuff!
        {
            switch (mAction) {
                case SAID::clear:
                {
                    clearRequest();
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
        }
        
        if ( event.type == sf::Event::JoystickMoved && event.joystickMove.axis == mJ.UpDownAxis
             && event.joystickMove.joystickId == mJ.ID)
        {
            float jPos = event.joystickMove.position;
            if ( jPos > gJoyThreshold && !mJoyMoved)
            {
                mMainMenu.move_down();
                mJoyMoved = true;
            }
            if ( jPos < -gJoyThreshold && !mJoyMoved)
            {
                mMainMenu.move_up();
                mJoyMoved = true;
            }
            if ( jPos > -gJoyThreshold && jPos < gJoyThreshold)
                mJoyMoved = false;
        }
        
        if ( event.type == sf::Event::JoystickButtonPressed && event.joystickButton.joystickId == mJ.ID)
        {
            if ( event.joystickButton.button == mJ.Fire ) menuChosen();
            if ( event.joystickButton.button == mJ.UpButton )
            {
                mMainMenu.move_up();
            }
            if ( event.joystickButton.button == mJ.DownButton )
            {
                mMainMenu.move_down();

            }
        }
        
        if (event.type == sf::Event::KeyPressed)
            switch (event.key.code)
        {
            case sf::Keyboard::Up:
            {
                mMainMenu.move_up();
                break;
            }
            case sf::Keyboard::Down:
            {
                mMainMenu.move_down();
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

void cMenuState::render()
{
    if ( !mIsActive ) return;
    mWindow.setView(mView);
    mWindow.draw(mSprite);
    mMainMenu.render(mWindow, sf::RenderStates::Default);
}


void cMenuState::run()
{
    processEvents();
}

void cMenuState::cleanup()
{
    mWindow.close();
}