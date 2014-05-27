// This is the main gamestate; i.e. the state we're in when
// we're actually playing - and the game's not paused.

#include "gameState.h"

cGameState::cGameState(cEngine* p):
cState { p, "game" },
mDifficulty { pEngine->mDifficulty },
mControl { pEngine->mControl },
mWorld { mCQ, mFontHolder, mTextureHolder, mWindow, mDifficulty, pEngine->getStrParam() },
mGamma { pEngine->mGamma },
mStageName { pEngine->getStrParam() }
{    
    mTimeSinceLastRender = sf::Time::Zero;
    mTimeSinceLastUpdate = sf::Time::Zero;
    mCurrentFPS = 0.0;
    mPastFPS = 0.0;
    mCurrentInput = sf::Vector2f { 0.0, 0.0 };
    mPastInput = sf::Vector2f { 0.0, 0.0 };
    mView = mWindow.getDefaultView();
    sf::Joystick::update();
    
    mFPStext = sf::Text("", mFontHolder.get(FontID::smallfont), 16);
    mFPStext.setColor(sf::Color::White);
    mFPStext.setPosition(10,10);
    
    mWorld.adjustBkGamma(mGamma);
    mWorld.setUpSoundQueue(&mSoundQueue);

    mMusicQueue.push(MusicCommand::playGame);
}

void cGameState::reset()
{
    mIsActive = true;
    if ( pEngine->mGamma != mGamma )
    {
        mWorld.adjustBkGamma(pEngine->mGamma - mGamma);
        mGamma = pEngine->mGamma;
    }
    mClock.restart();
    mFPSclock.restart();
}

void cGameState::init()
{
    // Mouse cursor might be visible in main menu,
    // not needed in actual game
    mWindow.setMouseCursorVisible(false);
    mWindow.clear();
    if ( mControl == ControlType::gamepad )
        sf::Joystick::update();
    reset();
}

void cGameState::putFPS()
{
    
    // mCurrentFPS is a counter for the number of frames published so far.
    // it becomes mPastFPS after a second has actually passed.
    
    mFPStext.setString("FPS: " + std::to_string(mPastFPS));
    mWindow.draw(mFPStext); // , mWorld.getViewTransform());
}

void cGameState::processEvents()            // process user's events.
{
    if (!mIsActive) return;
    sf::Event event;
    while ( mWindow.pollEvent(event) )
    {
        if ( event.type == sf::Event::Closed )  // if window closed -> let's clear the state stack.
        {                                       // that will terminate the program.
            clearRequest();
            return;
        }
        if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        {
            mIsActive = false;          // launch pause menu when Escape is pressed.
            pushRequest("pause");
            return;
        }
            
        if ( mControl == ControlType::keyboard && event.type == sf::Event::KeyPressed )
        {
            if (mWorld.mPlayerActive)
            {
                if ( event.key.code == mK.Polarity ) mWorld.playerSwitch();
                if ( event.key.code == mK.Rockets ) mWorld.playerRockets();
            }
        }
        
        if ( mControl == ControlType::gamepad && event.type == sf::Event::JoystickButtonPressed )
        {
            if (event.joystickButton.joystickId == mJ.ID)
            {
                if ( event.joystickButton.button == mJ.StartButton )
                {
                    mIsActive = false;
                    pushRequest("pause");
                    return;
                }
                if ( mWorld.mPlayerActive )
                {
                    if ( event.joystickButton.button == mJ.Polarity ) mWorld.playerSwitch();
                    if ( event.joystickButton.button == mJ.Rockets ) mWorld.playerRockets();
                }
            }
        }
    }
    
    if ( mControl == ControlType::keyboard )
    {
        if ( sf::Keyboard::isKeyPressed(mK.Fire) ) mWorld.playerShoot();
        bool dirbuttons[4] { false, false, false, false };
        
        if (sf::Keyboard::isKeyPressed(mK.Left))
            dirbuttons[0] = true;
        if (sf::Keyboard::isKeyPressed(mK.Right))
            dirbuttons[2] = true;
        if (sf::Keyboard::isKeyPressed(mK.Up))
            dirbuttons[1] = true;
        if (sf::Keyboard::isKeyPressed(mK.Down))
            dirbuttons[3] = true;
        
        mWorld.playerMove(dirbuttons);
    }

    if (mControl == ControlType::gamepad && mWorld.mPlayerActive)
    {
        if (sf::Joystick::isButtonPressed(mJ.ID, mJ.Fire)) mWorld.playerShoot();
        
        // OK first check if analog stick is moving
        mCurrentInput.x = sf::Joystick::getAxisPosition(mJ.ID, mJ.LeftRightAxis);
        mCurrentInput.y = sf::Joystick::getAxisPosition(mJ.ID, mJ.UpDownAxis);
        
        if ((mCurrentInput.x < gJoyThreshold) && (mCurrentInput.x > -gJoyThreshold))
            mCurrentInput.x = 0.0;
        if ((mCurrentInput.y < gJoyThreshold) && (mCurrentInput.y > -gJoyThreshold))
            mCurrentInput.y = 0.0;
        
        mWorld.playerAccelerate(mCurrentInput.x - mPastInput.x, mCurrentInput.y - mPastInput.y);
        mPastInput = mCurrentInput;
        
        // Then check d-pad, but only if there's no input on the analog stick!
        if ( mCurrentInput.x == 0.0 && mCurrentInput.y == 0.0)
        {
            bool dirbuttons[4] { false, false, false, false };
        
            if (sf::Joystick::isButtonPressed(mJ.ID, mJ.LeftButton))
                dirbuttons[0] = true;
            if (sf::Joystick::isButtonPressed(mJ.ID, mJ.RightButton))
                dirbuttons[2] = true;
            if (sf::Joystick::isButtonPressed(mJ.ID, mJ.UpButton))
                dirbuttons[1] = true;
            if (sf::Joystick::isButtonPressed(mJ.ID, mJ.DownButton))
                dirbuttons[3] = true;
        
            mWorld.playerMove(dirbuttons);
        }
    }
}

void cGameState::render()
{
    // This runs regardless of the state being active or not.
    // But if it's passive, we put an opaque layer on top.
    
    mWorld.render();

    ++mCurrentFPS;
    mTimeSinceLastRender += mFPSclock.restart();
    if ( mTimeSinceLastRender >= sf::seconds(1.0f) )
    {
        mPastFPS = mCurrentFPS;
        mCurrentFPS = 0;
        mTimeSinceLastRender -= sf::seconds(1.0f);
    }
    
    putFPS();

}

void cGameState::update(sf::Time dt)
{
    mWorld.update(dt);
}

void cGameState::run()
{
    /* Main loop with fixed timestep updates.
     For explanation, start with the end:
     We render the scene (at the end of the loop),
     then, if we're still running, we process
     input events. If rendering and input event
     processing took longer than we have time for
     each frame, that means that the next rendered
     frame should represent the situation after
     not just one update (that would mean we're
     slowing down - the lower the framerate, the
     slower the simulation).
     So in the while loop in the middle we update
     the scene as many times as necessary,
     and only render the final state after these
     updates. */

    // Though as you can see this is not actually
    // in a loop. That's because the ENGINE is
    // looping, calling in turn the run() method
    // of the game state that's currently on the
    // top of the game state stack.
    
    processEvents();
    
    // to maintain desired frame rate:
    mTimeSinceLastUpdate += mClock.restart();
    while ( mTimeSinceLastUpdate > gTimePerFrame )
    {
        processEvents();
        update(gTimePerFrame);
        mTimeSinceLastUpdate -= gTimePerFrame;
    }
    
    // To decouple it from desired frame rate:
    // mTimeSinceLastUpdate = mClock.restart();
    // update(mTimeSinceLastUpdate);
    
    // Let's check now if mGameWorld has any messages for us.
    // If there's a message, let's push it on the screen, unless
    // it's a command message (e.g. _STAGEOVER_), in which case
    // handle that command.
    
    if ( !mWorld.msgQueue.empty() )
    {
        std::string currentMSG = mWorld.msgQueue.front();
        if ( currentMSG == "_STAGEOVER_" )
        {
            mWorld.msgQueue.pop();
            returnRequest("menu");
            return;
        }
            else if ( currentMSG == "_BOSS_MET_" )
            {
                // This message lets us know that the player actually
                // met a boss, so the "boss training" game mode is
                // now unlocked.
                
                mWorld.msgQueue.pop();
                pEngine->mBossTrainingUnlocked = true;
                pEngine->mRewrite = true;
            }
            else if ( currentMSG == "_BONUS_" )
            {
                // We reached the bonus stage!
                
                mWorld.msgQueue.pop();
                mWorld.mBonusStage = true;
            }
            else if ( currentMSG == "_GAMEOVER_" || currentMSG == "_STAGECLEAR_")
            {
                mWorld.msgQueue.pop();
                mWorld.msgQueue.push("_STAGEOVER_");
                std::string endmsg;
                if ( currentMSG == "_GAMEOVER_" )
                    { endmsg = "Game over!\n\nHow unfortunate.\n\nBullets fired: "; }
                else { endmsg = "Stage cleared!\n\nWell done!\n\nBullets fired: "; }

                // Basic int to str conversion
                auto i2s = [](unsigned int x)
                { std::string scr; do { char ctmp = (x % 10) + 48; scr = ctmp + scr; x /= 10; } while ( x > 0);
                    return scr; };

                std::string stmp;
                stmp = i2s(mWorld.mBulletsFired);
                endmsg += stmp;
                endmsg += "\nEnemies shot: ";
                
                stmp = i2s(mWorld.mEnemiesKilled);
                endmsg += stmp;
                endmsg += "\nPolarity switches: ";
                
                stmp = i2s(mWorld.mPolChgCount);
                endmsg += stmp;

                endmsg += "\n\nScore: ";
                
                unsigned int score = mWorld.getScore();
                
                stmp = i2s(score);
                endmsg += stmp;
                                
                if ( score > pEngine->mHiScore )
                {
                    endmsg += "\n\nNew high score!";
                    pEngine->mHiScore = score;
                    pEngine->mRewrite = true;
                }
                
                if ( mWorld.mEnemiesKilled == 0 && currentMSG == "_STAGECLEAR_"
                     && mStageName == "stage01" )
                {
                    endmsg += "\n\n'Pacifist' achievement completed!";
                    pEngine->mACPacifist = true;
                    pEngine->mRewrite = true;
                }
                
                if ( mWorld.mPolChgCount == 0 && currentMSG == "_STAGECLEAR_"
                    && mStageName == "stage01" )
                {
                    endmsg += "\n\n'Monochrome' achievement completed!";
                    pEngine->mACMonochrome = true;
                    pEngine->mRewrite = true;
                }
                
                if ( mWorld.mLifeLost == false && currentMSG == "_STAGECLEAR_"
                    && mStageName == "stage01" )
                {
                    endmsg += "\n\n'Survivor' achievement completed!";
                    pEngine->mACSurvivor = true;
                    pEngine->mRewrite = true;
                }
                endmsg += "\n";
                
                pEngine->setStrParam(endmsg);
                pushRequest("message");
            }
            else
        {
            pEngine->setStrParam(mWorld.msgQueue.front());
            mWorld.msgQueue.pop();
            pushRequest("message");
        }
    }
    
    // Render() will be called by state stack manager, i.e. the engine (engine.h).
}

void cGameState::cleanup()
{
    mMusicQueue.push(MusicCommand::playMenu);   // When returning to the menu, start its theme.
}                                               // Doing so will automatically stop the current
                                                // theme.