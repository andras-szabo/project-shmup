#ifndef __shmup_project__entities__
#define __shmup_project__entities__

#include <SFML/Graphics.hpp>
#include <queue>
#include "commands.h"
#include "globalVariables.h"
#include "resourceHolders.h"
#include "explosions.h"
#include "scripts.h"
#include "entityInfo.h"

class cEntity : public sf::Drawable, public sf::Transformable {
public:
    cEntity(const cEntityHolder&,       // info about the entity
            EntityPolarity,             // polarity
            sf::Vector2f,               // position
            sf::Vector2f,               // velocity
            float,                      // starting rotation (which way it faces)
            CQtype&,                    // access to command queue
            const sf::Texture&,         // texture (no need to pass the whole texholder reference)
            const cScript&,             // script
            ShooterID = ShooterID::nobody);  // if projectile - who shot?
            
    virtual const   sf::FloatRect   getAABB() const;        // returns AABB
    virtual const   sf::Vector2f    getTurretPosition(sf::Vector2f rel = sf::Vector2f()) const;
    virtual         void            explode();
    virtual         void            switchPolarity(EntityPolarity, bool force = false);
    virtual         void            hit(int, EntityPolarity,
                                        sf::Vector2f kickVelocity = sf::Vector2f { 0.0, 0.0 },
                                        ShooterID shooter = ShooterID::nobody);
    virtual         void            heal(int);
    
                    sf::Vector2f    update(sf::Time dt);    // returns position
                    void            setVelocity(sf::Vector2f vel) { mVelocity = vel; }
                    sf::Vector2f    getVelocity() const { return mVelocity; }
                    void            turn(float x)
                                    {
                                        sf::Transform tmp;
                                        tmp.rotate(x);
                                        sf::Vector2f bvel { tmp.transformPoint( mVelocity ) };
                                        mVelocity = bvel;
                                    }
    
                    void            speedup(float x)
                                    {
                                        mVelocity.x *= x; mVelocity.y *= x;
                                    }
    
                    void            setRotate(float x) { mRotSpeed = x; }
                    void            setTracking(cEntity* e, float v = 270.0)
                                    {
                                        mTrackingTarget = e;
                                        mTrackingSpeed = v;
                                    }
    
                    void            adjustVelocity(const sf::Vector2f& v) { mVelocity += v; }
                    void            setScaleFactor(float x) { mScaleFactor = x; }
                    void            adjustScaleFactor(float x) { mScaleFactor += x; }
                    void            setIDDQD(bool b) { mVulnerable = !b; }
                    void            setSpawning(bool b)
                                    { mSpawning = b; mDrawAura = b;}
                    void            updateTarget(cEntity* old, cEntity* fresh)
                                    {
                                        if (mTrackingTarget == old) mTrackingTarget = fresh;
                                    }
                    void            setInvisible(bool b) { mInvisible = b; }

public:
    StatusID        mStatus;
    EntityType      mType;
    EntityPolarity  mPolarity;
    std::string     mID;
    bool            mHit;
    sf::Time        mHitStun;
    bool            mVulnerable;
    bool            mKick;
    bool            mBoss;
    int             mHP;
    int             mDMG;
    std::string     mShotScript;
    std::string     mSecondShotScript;
    ShooterID       mShooter;
    sf::Time        mSoundTimer;    // to measure "sound effect cool down"

private:
    virtual void    draw(sf::RenderTarget&, sf::RenderStates) const;
    void            track();        // To encapsulate the function calculating which way we should turn in case
                                    // we're tracking something
private:
    sf::Vector2f    mHalfSize;
    CQtype&         mCQ;
    const cScript&  mScript;
    cEntity*        mTrackingTarget;
    float           mTrackingSpeed;
    sf::Sprite      mSprite;
    sf::FloatRect   mAABB;
    sf::Time        mHelperTimer;
    sf::Vector2f    mVelocity;
    sf::Time        mTimer;         // Timer for script execution
    int             mInstrPtr;      // instruction pointer in scripts
    float           mRotSpeed;      // rotates this many degrees per second
    sf::Vector2f    mTurretPosition;
    std::unique_ptr<ExplosionObject>    mExplosion;
    float           mScaleFactor;
    std::queue<sf::Vector2f>            mKickback;  // Kickbacks are queued; it's possible
                                                    // to get kicked back by several bullets
                                                    // at the same time
    // (perhaps kickback is a wrong name; it should be something like "recoil", anyhow,
    // the idea is that when you're hit by bullets that do have this kickback, then
    // your ship's velocity will be affected, i.e. bullets push you back. some, anyway.
    
    bool            mSpawning;      // Only used for players. If true, collision checks will be
                                    // made, but this particular entity will be invulnerable.
                                    // This is a different kind of invulnerability to that
                                    // shown by mVulnerable, because if that one is false, then
                                    // collision checks are not even made.
    bool            mDrawAura;      // Should we actually draw it? (used for blinking it)
    sf::CircleShape mAura;          // spawning-invulnerability aura
    sf::Time        mSpawnTimeLeft; // spawning-induced invulnerability
    bool            mInvisible;     // yes, there are invisible entities too
    bool            mFinalBoss;     // is this Entity the final boss? probably no.
};

#endif /* defined(__shmup_project__entities__) */
