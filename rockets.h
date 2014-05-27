#ifndef __shmup_project__rockets__
#define __shmup_project__rockets__

#include <SFML/Graphics.hpp>
#include "globalVariables.h"
#include "entities.h"

const size_t    MAXBEAMS = 12;
const sf::Time  Beam_LifeTime = sf::seconds(0.9);
const sf::Time  Beam_DrawTime = sf::seconds(0.25);

// Invocation:
// HomingBeam* p { new HomingBeam(starting_position, colour, number_of_beams);
//
// Targets: to be fed into HomingBeam in its update() member Function:
//
// update(dt, pointer-to-array-of-Vector2f)
//
// where the "target array" is just an array with targets' coordinates.
// These target coordinates may obviously change; the point is that the
// lazer beam "homes" in on the target. Yay.

class HomingBeam : public sf::Drawable, public sf::Transformable {
public:
    explicit                    HomingBeam(const sf::Vector2f&, const sf::Color&, size_t, int);
    bool                        update(sf::Time, const sf::Vector2f[]);
    
public:
    bool                        mVisible;
    bool                        mDrawingFinished;
    
    
private:
    void                        clear();
    virtual void                draw(sf::RenderTarget&, sf::RenderStates) const;
    
private:
    static const size_t         mTriangles = 600;
    int                         mThickness; // = 24
    static constexpr int        mSteps = mTriangles;
    static constexpr size_t     mSize = mTriangles + 2;
    
    float                       mUnitTime; // time for one triangle
    size_t                      mNumBeams;
    
    sf::Vertex                  mP[MAXBEAMS][mSize];
    sf::Time                    mT[MAXBEAMS][mSize];
    sf::Vector2f                mVelocity[MAXBEAMS][2];
    
    sf::Time                    mTimer;
    size_t                      mDrawPointer;
    
    sf::Color                   mColor;
};

// Rocket is a wrapper class around 2 HomingBeams.
class cRocket {
public:
    cRocket(sf::Vector2f, int, EntityPolarity, cEntity**, ShooterID);
    
    ~cRocket()
    {
        delete mBackgroundBeam;
        delete mForegroundBeam;
    }
    
    bool            mBeamActive;
    
public:
    void            update(sf::Time);
    void            render(sf::RenderTarget&, sf::RenderStates);
    
private:
    const int       mBackgroundBeamWidth = 30;
    const int       mForegroundBeamWidth = 18;
    
    int             mTargetCount;
    cEntity*        mTargets[gMaxTargetCount];
    sf::Vector2f    mTargetCoords[gMaxTargetCount];
    
    HomingBeam*     mBackgroundBeam;    // For cooler effect, 1 rocket =
    HomingBeam*     mForegroundBeam;    // 2 beams on top of each other
    sf::Color       mBackgroundBeamColor;
    sf::Color       mForegroundBeamColor;
    bool            mBeamHit;
    int             mBeamDmg;
    EntityPolarity  mBeamPolarity;
    ShooterID       mShooter;
    
    sf::CircleShape mRing0;              // to create the "shockwave effect"
    sf::CircleShape mRing1;              // to create the "shockwave effect"
};

#endif /* defined(__shmup_project__rockets__) */
