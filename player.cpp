#include "player.h"
#include "globalVariables.h"

cPlayer::cPlayer(const std::string& n, sf::Font& f, int hp):
mHP { hp },
mScore { 0 },
mLivesLeft { 2 },
mStartingHP { hp }
{
    mNameText = sf::Text { n, f, 24 };
    mNameText.setColor(sf::Color(200, 200, 200, 250));
    mNameText.setPosition( 10, 10 );
    
    mScoreText = sf::Text { "0", f, 22 };
    mScoreText.setColor(sf::Color(200,200, 200, 250));
    mScoreText.setPosition( 10, 40 );
    updateScorePos();
    
    mLivesText = sf::Text { "Lives left: ", f, 22 };
    mLivesText.setColor(sf::Color(200, 200, 200, 250));
    mLivesText.setPosition( 10, 70 );
    
    mHealthText = sf::Text { "HP: ", f, 22 };
    mHealthText.setColor(sf::Color(200, 200, 200, 250));
    mHealthText.setPosition( 10, 138 );
    
    mHealthBar = sf::RectangleShape { sf::Vector2f(gSideBarSize.x - 30, 10) };
    mHealthBar.setOutlineThickness(0.0);
    mHealthBar.setFillColor(sf::Color(0,200,0,250));
    mHealthBar.setPosition( 10, 168 );
    
    mLifeMarker = sf::RectangleShape { sf::Vector2f(20, 20) };
    mLifeMarker.setOutlineColor(sf::Color(200, 200, 200, 250));
    mLifeMarker.setOutlineThickness(1.0);
    mLifeMarker.setFillColor(sf::Color(0,0,200,200));
    
    mSuperBarFrame = sf::RectangleShape { sf::Vector2f(gSideBarSize.x - 78, 402) };
    mSuperBarFrame.setOutlineColor(sf::Color(200, 200, 200, 250));
    mSuperBarFrame.setOutlineThickness(2.0);
    mSuperBarFrame.setFillColor(sf::Color(0,0,0,0));
    mSuperBarFrame.setPosition(38, 196);
    
    mSuperBar = sf::RectangleShape { sf::Vector2f(gSideBarSize.x - 86, 0) };
    mSuperBar.setOutlineColor(sf::Color::Black);
    mSuperBar.setOutlineThickness(0.0);
    mSuperBar.setFillColor(sf::Color(200,0,0,250));
    updateSuperBarPos();
    
    mFrame = sf::RectangleShape { sf::Vector2f(gSideBarSize.x-4, gSideBarSize.y-4) };
    mFrame.setOutlineColor(sf::Color::Blue);
    mFrame.setOutlineThickness(2.0);
    mFrame.setFillColor(sf::Color(0,0,0,0));
    mFrame.setPosition(2,2);
}

void cPlayer::updateSuperBarPos()
{
    float height = mSuperMeter ? (float(mSuperMeter / 100.0)) * 390 : 0.0;
    mSuperBar.setPosition(42, 590 - height);
    mSuperBar.setSize( sf::Vector2f(gSideBarSize.x - 86, height) );
}

void cPlayer::updateScorePos()
{
    // Basic int-to-string routine; no point in bothering about
    // negative or floating point or whatever numbers
    std::string scr;
    auto i = mScore;

    do
    {
        char ctmp = (i % 10) + 48;
        scr = ctmp + scr;
        i /= 10;
    } while ( i > 0 );
    
    mScoreText.setString(scr);
    float length = mScoreText.getLocalBounds().width;
    mScoreText.setPosition( gSideBarSize.x - 30 - length , mScoreText.getPosition().y);
}

void cPlayer::render(sf::RenderTarget& t, sf::RenderStates s)
{
    if ( mNameText.getString() != "Player02" )
    {
        t.draw(mNameText, sf::RenderStates::Default);
        updateScorePos();
        t.draw(mScoreText, s);
        t.draw(mLivesText, s);
        for ( auto i = 0; i < mLivesLeft; ++i)
        {
            mLifeMarker.setPosition( gSideBarSize.x - 55 - i * 25, 110);
            t.draw(mLifeMarker, s);
        }
        t.draw(mHealthText, s);
        mHealthBar.setSize( sf::Vector2f((mHP / 1.0 / mStartingHP) * (gSideBarSize.x - 30) / 1.0, 10) );
        if ( mHealthBar.getSize().x > 0)
            t.draw(mHealthBar, s);
        t.draw(mSuperBarFrame, s);
        updateSuperBarPos();
        t.draw(mSuperBar, s);
    }
    // t.draw(mFrame, s);
}