#include "engine.h"
#include "globalVariables.h"
#include "ResourcePath.hpp"
#include "introState.h"
#include "menuState.h"
#include "gameState.h"
#include "pauseState.h"
#include "diffState.h"
#include "messageState.h"
#include "optionState.h"
#include "kjState.h"
#include "cfgState.h"
#include "sfxState.h"
#include <fstream>


template <typename T>
void cEngine::launchState()
{
    std::unique_ptr<cState> p { new T { this } };
    p->init();
    mStateStack.push_back(std::move(p));
}

cEngine::cEngine()
{
    // Initialize renderwindow, view
    mWindow.create(sf::VideoMode::getDesktopMode(), "[ project shmup ] " + gVersion);
    mMenuView = mWindow.getDefaultView();
    mMenuView.setSize(1280, 800);                    // Logical menu view size: 1280 x 800
    mMenuView.setCenter(640, 400);
    mMenuView.setViewport(sf::FloatRect(0.0, 0.0, 1.0, 1.0));   // Let's use full screen.
    mWindow.setView(mMenuView);                                 // Let's use mMenuView as default.
    
    mWindow.setVerticalSyncEnabled(true);                       // vsync
    mWindow.setKeyRepeatEnabled(false);                         // keyrepeat off
    // mWindow.setMouseCursorVisible(false);                       // mouse cursor off
    
    // Load fonts
    mFontHolder.load(FontID::smallfont, "sansation.ttf");
    
    // Load generally used textures
    mTextureHolder.load(TexID::initscreen, "initscr2.png");
    mTextureHolder.load(TexID::menuscreen, "initscr2.png");
    mTextureHolder.load(TexID::fadescreen, "optscr2.png");
    mTextureHolder.load(TexID::GUItexture, "guitex.png");
    
    // Set difficulty
    mDifficulty = Difficulty::medium;
    
    // Set up key and joy maps
    mKeyMap[sf::Keyboard::A] = "A";
    mKeyMap[sf::Keyboard::B] = "B";
    mKeyMap[sf::Keyboard::C] = "C";
    mKeyMap[sf::Keyboard::D] = "D";
    mKeyMap[sf::Keyboard::E] = "E";
    mKeyMap[sf::Keyboard::F] = "F";
    mKeyMap[sf::Keyboard::G] = "G";
    mKeyMap[sf::Keyboard::H] = "H";
    mKeyMap[sf::Keyboard::I] = "I";
    mKeyMap[sf::Keyboard::J] = "J";
    mKeyMap[sf::Keyboard::K] = "K";
    mKeyMap[sf::Keyboard::L] = "L";
    mKeyMap[sf::Keyboard::M] = "M";
    mKeyMap[sf::Keyboard::N] = "N";
    mKeyMap[sf::Keyboard::O] = "O";
    mKeyMap[sf::Keyboard::P] = "P";
    mKeyMap[sf::Keyboard::Q] = "Q";
    mKeyMap[sf::Keyboard::R] = "R";
    mKeyMap[sf::Keyboard::S] = "S";
    mKeyMap[sf::Keyboard::T] = "T";
    mKeyMap[sf::Keyboard::U] = "U";
    mKeyMap[sf::Keyboard::V] = "V";
    mKeyMap[sf::Keyboard::W] = "W";
    mKeyMap[sf::Keyboard::X] = "X";
    mKeyMap[sf::Keyboard::Y] = "Y";
    mKeyMap[sf::Keyboard::Z] = "Z";
    mKeyMap[sf::Keyboard::Num0] = "0";
    mKeyMap[sf::Keyboard::Num1] = "1";
    mKeyMap[sf::Keyboard::Num2] = "2";
    mKeyMap[sf::Keyboard::Num3] = "3";
    mKeyMap[sf::Keyboard::Num4] = "4";
    mKeyMap[sf::Keyboard::Num5] = "5";
    mKeyMap[sf::Keyboard::Num6] = "6";
    mKeyMap[sf::Keyboard::Num7] = "7";
    mKeyMap[sf::Keyboard::Num8] = "8";
    mKeyMap[sf::Keyboard::Num9] = "9";
    mKeyMap[sf::Keyboard::Escape] = "Esc";
    mKeyMap[sf::Keyboard::LControl] = "L Ctrl";
    mKeyMap[sf::Keyboard::LShift] = "L Shift";
    mKeyMap[sf::Keyboard::LAlt] = "L Alt";
    mKeyMap[sf::Keyboard::LSystem] = "L System";
    mKeyMap[sf::Keyboard::RControl] = "R Ctrl";
    mKeyMap[sf::Keyboard::RShift] = "R Shift";
    mKeyMap[sf::Keyboard::RAlt] = "R Alt";
    mKeyMap[sf::Keyboard::RSystem] = "R System";
    mKeyMap[sf::Keyboard::Menu] = "Menu";
    mKeyMap[sf::Keyboard::LBracket] = "[";
    mKeyMap[sf::Keyboard::RBracket] = "]";
    mKeyMap[sf::Keyboard::SemiColon] = ";";
    mKeyMap[sf::Keyboard::Comma] = ",";
    mKeyMap[sf::Keyboard::Period] = ".";
    mKeyMap[sf::Keyboard::Quote] = "'";
    mKeyMap[sf::Keyboard::Slash] = "/";
    mKeyMap[sf::Keyboard::BackSlash] = "\\";
    mKeyMap[sf::Keyboard::Tilde] = "~";
    mKeyMap[sf::Keyboard::Equal] = "=";
    mKeyMap[sf::Keyboard::Dash] = "-";
    mKeyMap[sf::Keyboard::Space] = "Space";
    mKeyMap[sf::Keyboard::Return] = "Enter";
    mKeyMap[sf::Keyboard::BackSpace] = "Backspace";
    mKeyMap[sf::Keyboard::Tab] = "Tab";
    mKeyMap[sf::Keyboard::PageUp] = "Page up";
    mKeyMap[sf::Keyboard::PageDown] = "Page down";
    mKeyMap[sf::Keyboard::End] = "End";
    mKeyMap[sf::Keyboard::Home] = "Home";
    mKeyMap[sf::Keyboard::Insert] = "Insert";
    mKeyMap[sf::Keyboard::Delete] = "Delete";
    mKeyMap[sf::Keyboard::Add] = "+";
    mKeyMap[sf::Keyboard::Subtract] = "-";
    mKeyMap[sf::Keyboard::Multiply] = "*";
    mKeyMap[sf::Keyboard::Divide] = "/";
    mKeyMap[sf::Keyboard::Left] = "Left arrow";
    mKeyMap[sf::Keyboard::Right] = "Right arrow";
    mKeyMap[sf::Keyboard::Up] = "Up arrow";
    mKeyMap[sf::Keyboard::Down] = "Down arrow";
    mKeyMap[sf::Keyboard::Numpad0] = "Num 0";
    mKeyMap[sf::Keyboard::Numpad1] = "Num 1";
    mKeyMap[sf::Keyboard::Numpad2] = "Num 2";
    mKeyMap[sf::Keyboard::Numpad3] = "Num 3";
    mKeyMap[sf::Keyboard::Numpad4] = "Num 4";
    mKeyMap[sf::Keyboard::Numpad5] = "Num 5";
    mKeyMap[sf::Keyboard::Numpad6] = "Num 6";
    mKeyMap[sf::Keyboard::Numpad7] = "Num 7";
    mKeyMap[sf::Keyboard::Numpad8] = "Num 8";
    mKeyMap[sf::Keyboard::Numpad9] = "Num 9";
    mKeyMap[sf::Keyboard::F1] = "F1";
    mKeyMap[sf::Keyboard::F2] = "F2";
    mKeyMap[sf::Keyboard::F3] = "F3";
    mKeyMap[sf::Keyboard::F4] = "F4";
    mKeyMap[sf::Keyboard::F5] = "F5";
    mKeyMap[sf::Keyboard::F6] = "F6";
    mKeyMap[sf::Keyboard::F7] = "F7";
    mKeyMap[sf::Keyboard::F8] = "F8";
    mKeyMap[sf::Keyboard::F9] = "F9";
    mKeyMap[sf::Keyboard::F10] = "F10";
    mKeyMap[sf::Keyboard::F11] = "F11";
    mKeyMap[sf::Keyboard::F12] = "F12";
    mKeyMap[sf::Keyboard::Pause] = "Pause";
    
    mJoyMap[0] = "Select";
    mJoyMap[1] = "L3 (Button 1)";
    mJoyMap[2] = "R3 (Button 2)";
    mJoyMap[3] = "Start";
    mJoyMap[4] = "Up (Button 4)";
    mJoyMap[5] = "Right (Button 5)";
    mJoyMap[6] = "Down (Button 6)";
    mJoyMap[7] = "Left (Button 7)";
    mJoyMap[8] = "L2 (Button 8)";
    mJoyMap[9] = "R2 (Button 9)";
    mJoyMap[10]= "L1 (Button 10)";
    mJoyMap[11]= "R1 (Button 11)";
    mJoyMap[12]= "/\ (Button 12)";
    mJoyMap[13]= "O (Button 13)";
    mJoyMap[14]= "X (Button 14)";
    mJoyMap[15]= "[] (Button 15)";
    
    // OK now, armed with that knowledge, let's load
    // controls, if any.
    
    std::ifstream in { resourcePath() + "kcontrols.cfg" };
    if ( in.fail() )
    {
        in.close();
    
        // Assigning default key and joy bindings
        
        k.Up = sf::Keyboard::Key::Up;
        k.Down = sf::Keyboard::Key::Down;
        k.Left = sf::Keyboard::Key::Left;
        k.Right = sf::Keyboard::Key::Right;
        k.Fire = sf::Keyboard::Key::X;
        k.Polarity = sf::Keyboard::Key::Q;
        k.Rockets = sf::Keyboard::Key::Z;
        
        std::ofstream out { resourcePath() + "kcontrols.cfg" };
        out << "UP " << int(k.Up) << "\n";
        out << "DOWN " << int(k.Down) << "\n";
        out << "LEFT " << int(k.Left) << "\n";
        out << "RIGHT " << int(k.Right) << "\n";
        out << "FIRE " << int(k.Fire) << "\n";
        out << "POLARITY " << int(k.Polarity) << "\n";
        out << "ROCKETS " << int(k.Rockets) << "\n";
        
        out.close();
    }
    else    // kcontrols.cfg exists.
    {
        std::string stmp;
        int i;
        in >> stmp;
        if ( stmp != "UP" ) throw std::runtime_error("Controls.cfg error.");
        in >> i; k.Up = sf::Keyboard::Key(i);

        in >> stmp;
        if ( stmp != "DOWN" ) throw std::runtime_error("Controls.cfg error.");
        in >> i; k.Down = sf::Keyboard::Key(i);
        
        in >> stmp;
        if ( stmp != "LEFT" ) throw std::runtime_error("Controls.cfg error.");
        in >> i; k.Left = sf::Keyboard::Key(i);
        std::cout << "Left: " << i << "\n";
        
        in >> stmp;
        if ( stmp != "RIGHT" ) throw std::runtime_error("Controls.cfg error.");
        in >> i; k.Right = sf::Keyboard::Key(i);

        in >> stmp;
        if ( stmp != "FIRE" ) throw std::runtime_error("Controls.cfg error.");
        in >> i; k.Fire = sf::Keyboard::Key(i);

        in >> stmp;
        if ( stmp != "POLARITY" ) throw std::runtime_error("Controls.cfg error.");
        in >> i; k.Polarity = sf::Keyboard::Key(i);

        in >> stmp;
        if ( stmp != "ROCKETS" ) throw std::runtime_error("Controls.cfg error.");
        in >> i; k.Rockets = sf::Keyboard::Key(i);
        
        in.close();
    }

    in = std::ifstream(resourcePath() + "jcontrols.cfg");
    if ( in.fail() )
    {
        in.close();
        std::ofstream out { resourcePath() + "jcontrols.cfg" };
        j.ID = 0;
        j.UpDownAxis = sf::Joystick::Axis::Y;
        j.LeftRightAxis = sf::Joystick::Axis::X;
        j.Fire = 14;
        j.Polarity = 10;    // default: L1
        j.Rockets = 11;     // default: R1
        j.UpButton = 4;     // default: d-pad
        j.DownButton = 6;
        j.LeftButton = 7;
        j.RightButton = 5;
        j.StartButton = 3;  // default: start button
        
        out << "JOY_ID " << int(j.ID) << "\n";
        out << "JOY_UPDOWN " << int(j.UpDownAxis) << "\n";
        out << "JOY_LEFTRIGHT " << int(j.LeftRightAxis) << "\n";
        out << "JUP " << int(j.UpButton) << "\n";
        out << "JDOWN " << int(j.DownButton) << "\n";
        out << "JLEFT " << int(j.LeftButton) << "\n";
        out << "JRIGHT " << int(j.RightButton) << "\n";
        out << "JFIRE " << int(j.Fire) << "\n";
        out << "JPOLARITY " << int(j.Polarity) << "\n";
        out << "JROCKETS " << int(j.Rockets) << "\n";
        out << "JSTART " << int(j.StartButton) << "\n";
        
        out.close();
    }
    else    // jconfig exists
    {
        std::string stmp;

        int i;
        
        in >> stmp;
        if ( stmp != "JOY_ID" ) throw std::runtime_error("Controls.cfg error.1");
        in >> i; j.ID = i;
        
        in >> stmp;
        if ( stmp != "JOY_UPDOWN" ) throw std::runtime_error("Controls.cfg error.2");
        in >> i; j.UpDownAxis = sf::Joystick::Axis(i);
        
        in >> stmp;
        if ( stmp != "JOY_LEFTRIGHT" ) throw std::runtime_error("Controls.cfg error.3");
        in >> i; j.LeftRightAxis = sf::Joystick::Axis(i);
        
        in >> stmp;
        if ( stmp != "JUP" ) throw std::runtime_error("Controls.cfg error.4");
        in >> i; j.UpButton = i;
        
        in >> stmp;
        if ( stmp != "JDOWN" ) throw std::runtime_error("Controls.cfg error.5");
        in >> i; j.DownButton = i;
        
        in >> stmp;
        if ( stmp != "JLEFT" ) throw std::runtime_error("Controls.cfg error.6");
        in >> i; j.LeftButton = i;
        
        in >> stmp;
        if ( stmp != "JRIGHT" ) throw std::runtime_error("Controls.cfg error.7");
        in >> i; j.RightButton = i;
        
        in >> stmp;
        if ( stmp != "JFIRE" ) throw std::runtime_error("Controls.cfg error.8");
        in >> i; j.Fire = i;

        in >> stmp;
        if ( stmp != "JPOLARITY" ) throw std::runtime_error("Controls.cfg error.9");
        in >> i; j.Polarity = i;
        
        in >> stmp;
        if ( stmp != "JROCKETS" ) throw std::runtime_error("Controls.cfg error.10");
        in >> i; j.Rockets = i;
        
        in >> stmp;
        if ( stmp != "JSTART" ) throw std::runtime_error("Controls.cfg error.11");
        in >> i; j.StartButton = i;

        in.close();
    }
    
    // Default control: keyboard
    mControl = ControlType::keyboard;

    // Check control type; if joystick found, switch to that
    sf::Joystick::update();
    for ( auto i = 0; i < 8; ++i )
        if ( sf::Joystick::isConnected(i) )
        {
            j.ID = i;   // first available joy will be used
            mControl = ControlType::gamepad;
            // std::cout << "Joystick " << i << " found.\n";
            break;
        }
    
    
    // Gamma, volumes set up
    in = std::ifstream { resourcePath() + "settings.cfg" };
    if ( in.fail() )        // then we'll load defaults
    {
        in.close();
        std::ofstream out { resourcePath() + "settings.cfg" };
        
        mGamma = 0;
        mMusicVolume = 100;
        mSfxVolume = 100;
        
        out << "MUSIC_VOLUME " << mMusicVolume << "\n";
        out << "SFX_VOLUME " << mSfxVolume << "\n";
        out << "BRIGHTNESS " << mGamma << "\n";
        
        out.close();
    }
        else    // settings.cfg already exists and can be opened
    {
        std::string stmp;
        in >> stmp;
        if ( stmp != "MUSIC_VOLUME" ) throw std::runtime_error("Settings.cfg error.\n");
        in >> mMusicVolume;
        in >> stmp;
        if ( stmp != "SFX_VOLUME" ) throw std::runtime_error("Settings.cfg error.\n");
        in >> mSfxVolume;
        in >> stmp;
        if ( stmp != "BRIGHTNESS" ) throw std::runtime_error("Settings.cfg error.\n");
        in >> mGamma;
        
        mSoundPlayer.setVolume(mSfxVolume);
        mMusicPlayer.setVolume(mMusicVolume);
        
        in.close();
    }
    
    mVolumeChange = false;
    
    // Load hiscore, achievement lists
    in = std::ifstream { resourcePath() + "achievements.dat" };
    if ( in.fail() )    // then we'll set up defaults
    {
        in.close();
        mHiScore = 80000;
        mBossTrainingUnlocked = false;
        mACSurvivor = false;
        mACPacifist = false;
        mACMonochrome = false;
        writeAchievements();
    }
        else // achievements.dat exists and can be opened
    {
        auto err = [] { throw std::runtime_error("Achievements.dat error.\n"); };
        std::string stmp;
        in >> stmp;
        if ( stmp != "HISCORE" ) err();
        in >> mHiScore;

        in >> stmp;
        if ( stmp != "BOSSTRAINING" ) err();
        int itmp;
        in >> itmp; mBossTrainingUnlocked = itmp;
        
        in >> stmp;
        if ( stmp != "PACIFIST" ) err();
        in >> itmp; mACPacifist = itmp;
        
        in >> stmp;
        if ( stmp != "MONOCHROME" ) err();
        in >> itmp; mACMonochrome = itmp;
        
        in >> stmp;
        if ( stmp != "SURVIVOR" ) err();
        in >> itmp; mACSurvivor = itmp;
        
        in.close();
    }
    
    mRewrite = false;
}

void cEngine::run()
{
    launchState<cIntroState>();
    
    // Main loop
    while ( !mStateStack.empty() )
    {
        // OK so this is what happens here. There's only ever one active state,
        // that is on top of the state stack; that needs to run() all the time.
        // But when it comes to drawing, it's important to draw the current stack
        // of States BELOW the active one, so that we can see e.g. the paused
        // game field underneath the pause menu.
        // Then, when all drawings are done, we display the thing.
        // O.c. if we don't need a state drawn when not active (e.g. we don't need
        // to see the main menu in the background of the actual game),
        // then we don't have to; states keep track their own mIsActive member.
        
        mWindow.clear();
        
        for ( const auto& i : mStateStack )
            i->render();
        
        mWindow.display();
        (*mStateStack.rbegin())->run();
        
        // POSSIBLE REFACTORING:
        // Update sound buffer -- perhaps removal doesn't need to run every update,
        // but like every other update would suffice, or even fewer
        
        // First let's see if volume changed.
        if ( mVolumeChange )
        {
            mSoundPlayer.setVolume(mSfxVolume);
            mMusicPlayer.setVolume(mMusicVolume);
            mVolumeChange = false;
        }
        
        // Then play the sounds. We only ever allow one sound effect to be played
        // at most twice every frame, so as to avoid multiple sounds strengthening one another
        // and creating a super loud sound.
        
        for ( auto& i : mPlayedAlready )    // reset the "played already" table
            i.second = 0;
        
        while ( !mSoundQueue.empty() )
        {
            tSFX = mSoundQueue.front();
            if ( mPlayedAlready[tSFX]++ < gMaxSfxCopy )
                mSoundPlayer.play(tSFX);
            mSoundQueue.pop();
        }
        mSoundPlayer.removeStoppedSounds();

        // OK now we handle music requests
        while ( !mMusicQueue.empty() )
        {
            MusicCommand c = mMusicQueue.front();
            mMusicQueue.pop();
            switch (c) {
                case MusicCommand::playMenu:
                {
                    mMusicPlayer.play(MusicID::menutheme);
                    break;
                }
                case MusicCommand::playGame:
                {
                    mMusicPlayer.play(MusicID::gametheme);
                    break;
                }
                case MusicCommand::pause:
                {
                    mMusicPlayer.setPaused(true);
                    break;
                }
                case MusicCommand::unpause:
                {
                    mMusicPlayer.setPaused(false);
                    break;
                }
            }
        }
        
        // And here: let's call the engine's 'let's apply stack changes' method,
        // so we make sure to run() everything first, THEN change the state (if
        // needed). Thereby we can avoid a situation where a state requests a
        // pop, is popped, and THEN it tries to refer to some own member which
        // has since then been gone.
        
        executeStackChanges();
        
    }
    // No more active states: clean up time.
    // if ((mWindow != nullptr) && (mWindow.isOpen()) mWindow.close();
}

void cEngine::writeAchievements()
{
    std::ofstream out { resourcePath() + "achievements.dat" };
    out << "HISCORE " << mHiScore << "\n";
    out << "BOSSTRAINING " << int(mBossTrainingUnlocked) << "\n";
    out << "PACIFIST " << int(mACPacifist) << "\n";
    out << "MONOCHROME " << int(mACMonochrome) << "\n";
    out << "SURVIVOR " << int(mACSurvivor) << "\n";
    out.close();

    mRewrite = false;
}

// What the states call:
void cEngine::request(SAID action, std::string s)
{
    // We only consider the first request in every update.
    if ( mStackAction == SAID::nothing )
    {
        mStackAction = action;
        mStackParam = s;
    }
}


// The dispatcher.
void cEngine::executeStackChanges()
{
    // Parameters expected in mStackAction and mStackParam.
    switch (mStackAction) {
        case SAID::pop:
        {
            popState();
            break;
        }
        case SAID::push:
        {
            pushState(mStackParam);
            break;
        }
        case SAID::swap:
        {
            swapState(mStackParam);
            break;
        }
        case SAID::return_to:
        {
            returnToState(mStackParam);
            break;
        }
        case SAID::clear:
        {
            clearStateStack();
            break;
        }
        default: break;
    }
    
    // Finally, reset stack action requests:
    mStackAction = SAID::nothing;
}

// Creates and pushes a new state onto the game state
// stack.
void cEngine::pushState(const std::string& s)
{

    if (!mStateStack.empty()) (*mStateStack.rbegin())->deactivate();

    if      ( s == "menu" )     launchState<cMenuState>();  // main menu
    else if ( s == "diff" )     launchState<cDiffState>();  // difficulty selection screen
    else if ( s == "kj" )       launchState<ckjState>();      // control type selection: (k)eyboard or (j)oystick
    else if ( s == "cfg" )      launchState<cfgState>();     // key-and-button-binding
    else if ( s == "options" || s == "option" ) launchState<cOptionState>();    // options' main menu
    else if ( s == "game" )     launchState<cGameState>();
    else if ( s == "pause" )    launchState<cPauseState>();
    else if ( s == "message" )  launchState<cMessageState>();
    else if ( s == "sfx" )      launchState<sfxState>();     // sound effects and graphics config
}

// Pops gamestate from the stack
void cEngine::popState()
{
    if (mStateStack.empty()) return;
    (*mStateStack.rbegin())->cleanup();
    mStateStack.pop_back();
    if ( !mStateStack.empty() )
        (*mStateStack.rbegin())->reset();
}

// Pops the top gamestate, and pushes on the stack
// a new one instead of the one that was just popped.
void cEngine::swapState(const std::string& s)
{
    popState();
    if (!mStateStack.empty()) (*mStateStack.rbegin())->deactivate();
    pushState(s);
}

// Pops states until it reaches one that we're looking for,
// as identified by the passed string parameter.
// If can't find desired state, it will terminate program!
void cEngine::returnToState(const std::string& s)
{
    while (!mStateStack.empty() && ((*mStateStack.rbegin())->mName != s))
        popState();
}

// Clears the game stack. This essentially terminates the program.
void cEngine::clearStateStack()
{
    while ( !mStateStack.empty() )
        popState();
}