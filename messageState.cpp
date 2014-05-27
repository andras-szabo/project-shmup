#include "messageState.h"
#include "globalVariables.h"

// The message to be displayed is stored in
// pEngine->mStrParam; accessible through
// pEngine->getStrParam();

cMessageState::cMessageState(cEngine* p):
cState { p, "message" },
mStatus { TextStatus::entering }
{
    // Set up textbox, center it on screen.
    mText.setString(pEngine->getStrParam());
    mText.setFont(mFontHolder.get(FontID::smallfont));
    mText.setCharacterSize(24);
    mText.setColor(sf::Color::White);
    sf::FloatRect f { mText.getLocalBounds() };
    
    mCenterPos = { static_cast<float>(600 - f.width / 2.0), static_cast<float>(400 - f.height / 2.0) };
    
    sf::Vector2f tp { mCenterPos.x - gTextVel.x, mCenterPos.y - gTextVel.y };
    if ( tp.x > 1200 ) tp.x = 1200;
    if ( tp.x < 0 ) tp.x = -f.width;
    if ( tp.y > 800 ) tp.y = 800;
    if ( tp.y < 0 ) tp.y = -f.height;
        
    mText.setPosition(tp);
    
    mBox.setFillColor(sf::Color(0,80,0,120));
    mBox.setOutlineColor(sf::Color::White);
    mBox.setOutlineThickness(2.0);
    mBox.setPosition(tp.x - 20, tp.y - 20);    // textbox boundary
    mBox.setSize(sf::Vector2f(f.width + 40, f.height + 40));
    
    mCenterPos.x -= 20;
    mCenterPos.y -= 20;
    
    mClock.restart();
}

void cMessageState::init()
{
    mView.setSize(1200, 800);
    mView.setViewport(sf::FloatRect { 0, 0, 1, 1 });
    mView.setCenter(600, 400);
    mWindow.setView(mView);
    
    mSoundQueue.push(SfxID::message);

}

void cMessageState::reset()
{

}

void cMessageState::processEvents()
{
    if (!mIsActive) return;
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        // Close window : exit
        if (event.type == sf::Event::Closed) {
            clearRequest();
            break;
        }
        
        if ((event.type == sf::Event::KeyPressed ||
             (event.type == sf::Event::JoystickButtonPressed && event.joystickButton.joystickId == mJ.ID) ||
             (event.type == sf::Event::JoystickMoved && event.joystickMove.joystickId == mJ.ID &&
              abs(event.joystickMove.position) > gJoyThreshold &&
              mText.getString() == "Press a directional\nbutton, or move an\nanalog stick.\n"))
            && mStatus == TextStatus::waiting)
        {
            mSoundQueue.push(SfxID::roger);
            mStatus = TextStatus::leaving;
            pEngine->setEvtParam(event);    // we return the event through pEngine; it will come in handy.
            break;
        }
    }
}

void cMessageState::render()
{
    mWindow.setView(mView);
    mWindow.draw(mBox);
    mWindow.draw(mText);
}

void cMessageState::update(sf::Time dt)
{
    if (!mIsActive) return;                 // Let's not allow it to call several poprequests.
    switch (mStatus) {
        case TextStatus::entering:
        {
            mText.move(gTextVel * dt.asSeconds());
            mBox.move(gTextVel * dt.asSeconds());
            
            // Check if we've reached the center of the stage already
            // if so, we're entering "waiting" state (that's not a gamestate though!)
            
            sf::Vector2f tp { mBox.getPosition() };
            if ( tp.x <= mCenterPos.x + 20 && tp.x >= mCenterPos.x - 20 &&
                 tp.y <= mCenterPos.y + 20 && tp.y >= mCenterPos.y - 20 )
                mStatus = TextStatus::waiting;
            break;
        }
        case TextStatus::leaving:
        {
            mText.move(-gTextVel * dt.asSeconds());
            mBox.move(-gTextVel * dt.asSeconds());
            
            // Check if we're fully out of bounds!
            // If so, pop the (game)state
            
            sf::Vector2f tp { mBox.getPosition() };
            sf::Vector2f sp { mBox.getSize() };
            if ( tp.x >= 1200 || tp.x+sp.x <= 0 || tp.y >= 800 || tp.y+sp.y <= 0 )
            {
                mIsActive = false;
                popRequest();
            }
            break;
        }
    }
}

void cMessageState::run()
{
    processEvents();
    mTimeSinceLastUpdate += mClock.restart();
    while ( mTimeSinceLastUpdate > gTimePerFrame )
    {
        processEvents();
        update(gTimePerFrame);
        mTimeSinceLastUpdate -= gTimePerFrame;
    }
}

void cMessageState::cleanup()
{
    
}