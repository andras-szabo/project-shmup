#include "cfgState.h"
#include "globalVariables.h"
#include <fstream>

cfgState::cfgState(cEngine* p):
cState { p, "cfg" }
{
    std::vector<std::string> stmp;
        
    stmp.push_back(pEngine->mKeyMap[mK.Up]);
    stmp.push_back(pEngine->mKeyMap[mK.Right]);
    stmp.push_back(pEngine->mKeyMap[mK.Down]);
    stmp.push_back(pEngine->mKeyMap[mK.Left]);
    stmp.push_back(pEngine->mKeyMap[mK.Fire]);
    stmp.push_back(pEngine->mKeyMap[mK.Polarity]);
    stmp.push_back(pEngine->mKeyMap[mK.Rockets]);
        
    mMenu1 = cMenu { stmp, mFontHolder.get(FontID::smallfont), 24, sf::Vector2f(300,150) };
    mMenu1.setUpSoundQueue(&mSoundQueue);
    stmp.clear();

    std::string s;
    if ( mJ.UpDownAxis == sf::Joystick::Axis::Y ) s = "L Stick";
    if ( mJ.UpDownAxis == sf::Joystick::Axis::R ) s = "R Stick";
    if ( mJ.UpDownAxis == sf::Joystick::Axis::U ) s = "D pad";

    for (auto i = 0; i < 4; ++i ) stmp.push_back(s);

    stmp.push_back(pEngine->mJoyMap[mJ.Fire]);
    stmp.push_back(pEngine->mJoyMap[mJ.Polarity]);
    stmp.push_back(pEngine->mJoyMap[mJ.Rockets]);
    mMenu2 = cMenu { stmp, mFontHolder.get(FontID::smallfont), 24, sf::Vector2f(800, 150) };
    mMenu2.setUpSoundQueue(&mSoundQueue);
    
    stmp.clear();
    stmp.push_back("Done");
    mMenu3 = cMenu { stmp, mFontHolder.get(FontID::smallfont), 24, sf::Vector2f(550, 500) };
    mMenu3.setUpSoundQueue(&mSoundQueue);
    
    sf::Text tmp;
    tmp.setFont(mFontHolder.get(FontID::smallfont));
    tmp.setCharacterSize(26);
    tmp.setColor(sf::Color::White);
    tmp.setPosition(550,150);
    tmp.setString("Move up");
    mCaptions.push_back(tmp);
    
    tmp.setString("Move right");
    tmp.setPosition(550,186);
    mCaptions.push_back(tmp);
    
    tmp.setString("Move down");
    tmp.setPosition(550,224);
    mCaptions.push_back(tmp);
    
    tmp.setString("Move left");
    tmp.setPosition(550,260);
    mCaptions.push_back(tmp);
    
    tmp.setString("Shoot");
    tmp.setPosition(550,294);
    mCaptions.push_back(tmp);
    
    tmp.setString("Switch polarity");
    tmp.setPosition(550,330);
    mCaptions.push_back(tmp);
    
    tmp.setString("Homing lasers");
    tmp.setPosition(550,364);
    mCaptions.push_back(tmp);
    
    tmp.setString("Keyboard");
    tmp.setPosition(310,100);
    mCaptions.push_back(tmp);
    
    tmp.setString("Joystick/\ngamepad");
    tmp.setPosition(830,80);
    mCaptions.push_back(tmp);
    
    tmp.setString("Pause/\Options");
    tmp.setPosition(550,428);
    mCaptions.push_back(tmp);
    
    tmp.setString("ESC");
    tmp.setPosition(350,428);
    mCaptions.push_back(tmp);
    
    tmp.setString("Start button");
    tmp.setPosition(810,428);
    mCaptions.push_back(tmp);
    
    mActive = MenuID::kb;
    mMenu1.setActiveID(0);
    mMenu1.setDrawHighlight(true);
    mMenu2.setActiveID(0);
    mMenu2.setDrawHighlight(false);
    mMenu3.setActiveID(0);
    mMenu3.setDrawHighlight(false);
    
    mNewBinding = false;
    reset();
}

void cfgState::init()
{
    // load background image (something simple to fade out the existing view
    
    mView.setSize(1200, 800);
    mView.setViewport(sf::FloatRect { 0, 0, 1, 1 });
    mView.setCenter(600, 400);
    mWindow.setView(mView);
    
    mSprite.setTexture(mTextureHolder.get(TexID::fadescreen));
    mSprite.setColor(sf::Color(50, 50, 50, 180));
    mSprite.setPosition(0,0);

    mKeyChanged = false;
    mJoyChanged = false;
}

void cfgState::reset()
{
    
    mSelected = false;
    mStillBlinking = sf::seconds(gMenuBlink);
    switch (mActive) {
        case MenuID::kb: mMenu1.setDrawHighlight(true); break;
        case MenuID::joy: mMenu2.setDrawHighlight(true); break;
        case MenuID::done: mMenu3.setDrawHighlight(true); break;
    }
}

void cfgState::select(SAID action, std::string param)
{
    mAction = action;
    mParam = param;
    mSelected = true;
    mClock.restart();   // let the blinking begin!
}

void cfgState::menuChosen()
{
    // This refers only to the "done," 1-element menu
    mSoundQueue.push(SfxID::menuselect);
    
    // Here we should check first if there were any changes, but for now
    if ( mKeyChanged )
    {
        std::ofstream out { resourcePath() + "kcontrols.cfg" };
        out << "UP " << int(mK.Up) << "\n";
        out << "DOWN " << int(mK.Down) << "\n";
        out << "LEFT " << int(mK.Left) << "\n";
        out << "RIGHT " << int(mK.Right) << "\n";
        out << "FIRE " << int(mK.Fire) << "\n";
        out << "POLARITY " << int(mK.Polarity) << "\n";
        out << "ROCKETS " << int(mK.Rockets) << "\n";
        out.close();
    }
    
    if ( mJoyChanged )
    {
        std::ofstream out { resourcePath() + "jcontrols.cfg" };
        out << "JOY_ID " << int(mJ.ID) << "\n";
        out << "JOY_UPDOWN " << int(mJ.UpDownAxis) << "\n";
        out << "JOY_LEFTRIGHT " << int(mJ.LeftRightAxis) << "\n";
        out << "JUP " << int(mJ.UpButton) << "\n";
        out << "JDOWN " << int(mJ.DownButton) << "\n";
        out << "JLEFT " << int(mJ.LeftButton) << "\n";
        out << "JRIGHT " << int(mJ.RightButton) << "\n";
        out << "JFIRE " << int(mJ.Fire) << "\n";
        out << "JPOLARITY " << int(mJ.Polarity) << "\n";
        out << "JROCKETS " << int(mJ.Rockets) << "\n";
        out << "JSTART " << int(mJ.StartButton) << "\n";
        out.close();
    }
    
    select(SAID::pop, "");
}

void cfgState::processEvents()
{
    if ( mSelected )
    {
        if ( mStillBlinking > sf::Time::Zero )  // if still blinking, then let's blink!
        {
            switch (mActive){
                case MenuID::done: mMenu3.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2); break;
                case MenuID::joy: mMenu2.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2); break;
                case MenuID::kb: mMenu1.setDrawHighlight((mStillBlinking.asMilliseconds()/100) % 2); break;
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
                case SAID::push:
                {
                    pushRequest(mParam);
                    break;
                }
            }
        }
        return;
    }
    
    if ( mNewBinding )  // we have to deal with a new binding!
    {
        sf::Event e = pEngine->getEvtParam();   // Engine has the relevant event for us
        if (mActive == MenuID::kb)  // it is a new KEY binding, not button
        {
            if ( e.type == sf::Event::KeyPressed )
            {
                    switch (mMenu1.getActiveID()) {
                    case 0 : mK.Up = e.key.code; break;
                    case 1 : mK.Right = e.key.code; break;
                    case 2 : mK.Down = e.key.code; break;
                    case 3 : mK.Left = e.key.code; break;
                    case 4 : mK.Fire = e.key.code; break;
                    case 5 : mK.Polarity = e.key.code; break;
                    case 6 : mK.Rockets = e.key.code; break;
                }
            }
            std::string s;
            switch (mMenu1.getActiveID()) {
                case 0 : s = pEngine->mKeyMap[mK.Up]; break;
                case 1 : s = pEngine->mKeyMap[mK.Right]; break;
                case 2 : s = pEngine->mKeyMap[mK.Down]; break;
                case 3 : s = pEngine->mKeyMap[mK.Left]; break;
                case 4 : s = pEngine->mKeyMap[mK.Fire]; break;
                case 5 : s = pEngine->mKeyMap[mK.Polarity]; break;
                case 6 : s = pEngine->mKeyMap[mK.Rockets]; break;
            }
            mMenu1.setActiveText(s);
            mKeyChanged = true;
        }
        
        if (mActive == MenuID::joy) // it is a new button binding
        {
            std::string s;
            
            if ( mMenu2.getActiveID() < 4 ) // movement!
            {
                if ( mJ.UpDownAxis == sf::Joystick::Axis::Y ) s = "L Stick";
                if ( mJ.UpDownAxis == sf::Joystick::Axis::R ) s = "R Stick";
                if ( mJ.UpDownAxis == sf::Joystick::Axis::U ) s = "D pad";
                mMenu2.setActiveText(s);
                
                if ( e.type == sf::Event::JoystickMoved && abs(e.joystickMove.position)>gJoyThreshold)
                {
                    if ( e.joystickMove.axis == sf::Joystick::Axis::X || e.joystickMove.axis == sf::Joystick::Axis::Y ) // left stick
                    {
                        mJ.UpDownAxis = sf::Joystick::Axis::Y;
                        mJ.LeftRightAxis = sf::Joystick::Axis::X;
                        for (auto i = 3; i >= 0; --i)
                        {
                            mMenu2.setActiveID(i);
                            mMenu2.setActiveText("L Stick");
                        }
                    }
                    if ( e.joystickMove.axis == sf::Joystick::Axis::Z || e.joystickMove.axis == sf::Joystick::Axis::R ) // right stick
                    {
                        mJ.UpDownAxis = sf::Joystick::Axis::R;
                        mJ.LeftRightAxis = sf::Joystick::Axis::Z;
                        for (auto i = 3; i >= 0; --i)
                        {
                            mMenu2.setActiveID(i);
                            mMenu2.setActiveText("R Stick");
                        }
                    }
                }
            
                if ( e.type == sf::Event::JoystickButtonPressed && e.joystickButton.button > 3 && e.joystickButton.button < 8) // d-pad!
                {
                    mJ.UpDownAxis = sf::Joystick::Axis::U;
                    mJ.LeftRightAxis = sf::Joystick::Axis::V;
                    mJ.UpButton = 4;
                    mJ.RightButton = 5;
                    mJ.DownButton = 6;
                    mJ.LeftButton = 7;
                    for ( auto i = 3; i >= 0; --i )
                    {
                        mMenu2.setActiveID(i);
                        mMenu2.setActiveText("D pad");
                    }
                }
            }
        
            if ( mMenu2.getActiveID() > 3) // non-movement
            {
                if ( e.type == sf::Event::JoystickButtonPressed )
                {
                    switch (mMenu2.getActiveID()) {
                        case 4 : mJ.Fire = e.joystickButton.button; break;
                        case 5 : mJ.Polarity = e.joystickButton.button; break;
                        case 6 : mJ.Rockets = e.joystickButton.button; break;
                    }
                }
                std::string s;
                switch (mMenu2.getActiveID()) {
                    case 4 : s = pEngine->mJoyMap[mJ.Fire]; break;
                    case 5 : s = pEngine->mJoyMap[mJ.Polarity]; break;
                    case 6 : s = pEngine->mJoyMap[mJ.Rockets]; break;
                }
                mMenu2.setActiveText(s);
            }
            
            mJoyChanged = true;
        }

        mNewBinding = false;
        return;
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
                mJoyMoved = false;
                m = Mact::nothing;
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
        
        if (event.type == sf::Event::KeyPressed)
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
    }

    switch (m){
        case Mact::up:
        {
            if ( mActive == MenuID::kb )
            {
                if ( mMenu1.getActiveID() == 0 )
                {
                    mMenu1.setDrawHighlight(false);
                    mActive = MenuID::done;
                    mMenu3.setDrawHighlight(true);
                    mSoundQueue.push(SfxID::menumove);
                } else mMenu1.move_up();
                break;
            }
            if ( mActive == MenuID::joy )
            {
                if ( mMenu2.getActiveID() == 0 )
                {
                    mMenu2.setDrawHighlight(false);
                    mActive = MenuID::done;
                    mMenu3.setDrawHighlight(true);
                    mSoundQueue.push(SfxID::menumove);
                } else mMenu2.move_up();
                break;
            }
            if ( mActive == MenuID::done )
            {
                mMenu3.setDrawHighlight(false);
                mMenu1.setDrawHighlight(true);
                mMenu1.setActiveID(6);
                mActive = MenuID::kb;
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            break;
        }

        case Mact::down:
        {
            if ( mActive == MenuID::kb )
            {
                if ( mMenu1.getActiveID() == 6 )
                {
                    mMenu1.setDrawHighlight(false);
                    mActive = MenuID::done;
                    mMenu3.setDrawHighlight(true);
                    mSoundQueue.push(SfxID::menumove);
                } else mMenu1.move_down();
                break;
            }
            if ( mActive == MenuID::joy )
            {
                if ( mMenu2.getActiveID() == 6 )
                {
                    mMenu2.setDrawHighlight(false);
                    mActive = MenuID::done;
                    mMenu3.setDrawHighlight(true);
                    mSoundQueue.push(SfxID::menumove);
                } else mMenu2.move_down();
                break;
            }
            if ( mActive == MenuID::done )
            {
                mMenu3.setDrawHighlight(false);
                mMenu1.setDrawHighlight(true);
                mMenu1.setActiveID(0);
                mActive = MenuID::kb;
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            break;
        }

        case Mact::right:
        {
            if ( mActive == MenuID::kb )
            {
                mMenu1.setDrawHighlight(false);
                mActive = MenuID::joy;
                mMenu2.setDrawHighlight(true);
                mMenu2.setActiveID(mMenu1.getActiveID());
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            if ( mActive == MenuID::done )
            {
                mMenu3.setDrawHighlight(false);
                mMenu2.setDrawHighlight(true);
                mMenu2.setActiveID(6);
                mActive = MenuID::joy;
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            break;
        }
    
        case Mact::left:
        {
            if ( mActive == MenuID::joy )
            {
                mMenu2.setDrawHighlight(false);
                mActive = MenuID::kb;
                mMenu1.setDrawHighlight(true);
                mMenu1.setActiveID(mMenu2.getActiveID());
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            if ( mActive == MenuID::done )
            {
                mMenu3.setDrawHighlight(false);
                mMenu1.setDrawHighlight(true);
                mMenu1.setActiveID(6);
                mActive = MenuID::kb;
                mSoundQueue.push(SfxID::menumove);
                break;
            }
            break;
        }
    
        case Mact::enter:
        {
            if ( mActive == MenuID::done ) { menuChosen(); break; }
            else
            {
                if ( mActive == MenuID::kb )
                {
                    mSoundQueue.push(SfxID::menuselect);
                    mMenu1.setActiveText(" ");
                    pEngine->setStrParam("Press a key!\n");
                }
                else
                {
                    mSoundQueue.push(SfxID::menuselect);
                    mMenu2.setActiveText(" ");
                    if (mMenu2.getActiveID() < 4)   // this is about movement!
                        pEngine->setStrParam("Press a directional\nbutton, or move an\nanalog stick.\n");
                    else pEngine->setStrParam("Press a button!\n");
                }
                mNewBinding = true;             // let's wait for what kinda event we get back.
                select(SAID::push, "message");
            }
        }
    }
}

void cfgState::render()
{
    mWindow.setView(mView);
    mWindow.draw(mSprite);
    mMenu1.render(mWindow, sf::RenderStates::Default);
    mMenu2.render(mWindow, sf::RenderStates::Default);
    mMenu3.render(mWindow, sf::RenderStates::Default);
    for(auto& i : mCaptions)
        mWindow.draw(i);
}

void cfgState::run()
{
    processEvents();
}

void cfgState::cleanup()
{
    
}