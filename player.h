#ifndef shmup_project_player_h
#define shmup_project_player_h

// info about the player's craft

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

class cPlayer {
public:
    cPlayer(const std::string&, sf::Font&, int);   // name and hp
    void            render(sf::RenderTarget&, sf::RenderStates);
    void            setHP(int x) { mHP = x; }
    void            setSHP(int x) { mStartingHP = x; }

public:
    float           mTopSpeed;
    sf::Time        mCooldown;
    sf::Time        mTimer;
    int             mSuperMeter;
    unsigned int    mScore;
    int             mHP;
    int             mStartingHP;
    unsigned int    mLivesLeft;
    
private:
    void            updateScorePos();
    void            updateSuperBarPos();
    
private:
    sf::Text                mNameText;
    sf::Text                mScoreText;
    sf::Text                mLivesText;
    sf::Text                mHealthText;
    sf::RectangleShape      mSuperBarFrame;
    sf::RectangleShape      mSuperBar;
    sf::RectangleShape      mFrame;
   
    // sf::Sprite              mSprite;    // sprites representing player's lives
    sf::RectangleShape      mLifeMarker;    // for now, let's use this.
    sf::RectangleShape      mHealthBar;
};

#endif
