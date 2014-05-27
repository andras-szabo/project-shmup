#include "entities.h"
#include <cmath>

cEntity::cEntity(const cEntityHolder& e,
                 EntityPolarity epol,
                 sf::Vector2f pos,
                 sf::Vector2f vel,
                 float rot,
                 CQtype& cq,
                 const sf::Texture& tex,
                 const cScript& scr,
                 ShooterID shooter):
mID { e.mID },
mType { e.mType },
mDMG { e.mDMG },
mHP { e.mHP },
mKick { e.mKick },
mAABB { e.mAABBRect },
mBoss { e.mBoss },
mFinalBoss { e.mFinalBoss },
mShotScript { e.mShotType },
mSecondShotScript { e.mSecondShotType },
mHit { false },
mCQ { cq },
mPolarity { epol },
mVelocity { vel },
mScript { scr },
mStatus { StatusID::onscreen },
mTrackingTarget { nullptr },
mTrackingSpeed { 0 },
mVulnerable { true },
mScaleFactor { 1.0 },
mTimer { sf::Time::Zero },
mHelperTimer { sf::Time::Zero },
mInstrPtr { 0 },
mRotSpeed { 0 },
mShooter { shooter },
mSpawning { false },
mDrawAura { false },
mSpawnTimeLeft { gSpawnTime },
mSoundTimer { sf::seconds(0) },
mInvisible { false }
{
    mSprite.setTexture(tex);
    mSprite.setTextureRect(e.mTexRect);
    switch (mPolarity) {
        case (EntityPolarity::light):
            mSprite.setColor(gCLight);
            break;
        case (EntityPolarity::neutral):
            mSprite.setColor(gCNeutral);
            break;
        case (EntityPolarity::dark):
            mSprite.setColor(gCDark);
            break;
        default: break;
    }
    sf::FloatRect tmp { mSprite.getLocalBounds() };
    mHalfSize = sf::Vector2f ( tmp.width / 2.f, tmp.height / 2.f);
    mSprite.setOrigin(tmp.left + mHalfSize.x, tmp.top + mHalfSize.y);
    setPosition(pos.x, pos.y);
    setRotation(rot);
    
    mAura.setRadius(std::max(mHalfSize.x + 16, mHalfSize.y + 16));
    mAura.setOutlineColor(sf::Color(200, 200, 200));
    mAura.setOutlineThickness(4.0);
    mAura.setFillColor(sf::Color(120, 120, 120, 180));
    mAura.setPosition(-mAura.getRadius(), -mAura.getRadius());
    
    if ( mType == EntityType::player ) setSpawning(true);

}

void cEntity::draw(sf::RenderTarget& t, sf::RenderStates s) const
{
    if (( mType == EntityType::playerbullet ) || ( mType == EntityType::enemybullet ))
    {
        s.transform *= getTransform(); // add own translation and rotation
        t.draw(mSprite, s);
        return;
    }
    
    if (( mType == EntityType::ship ) || ( mType == EntityType::player ) ||
        ( mType == EntityType::wall ))
    {
        sf::RenderStates tmp = s;
        s.transform *= getTransform();
        if (mScaleFactor != 1.0)
            s.transform.scale(mScaleFactor, mScaleFactor);

        if (mStatus != StatusID::exploding)
        {
            if ( mInvisible == false || mHit == true ) // even invisible objects
            {                                          // are drawn when hit!
                t.draw(mSprite, s);
                if (mSpawning && mDrawAura)
                    t.draw(mAura, s);
            }
            return;
        }

        // If we're here, then we're actually exploding.
        // The explosions of invisible objects will actually be visible;
        // that's the point: we can conjure up large explosions by
        // making a lot of invisible enemies blow up at the same time.

        if (!mExplosion->mDrawingFinished && mInvisible == false )
                                            // 1st part of explosion: sprite still visible
            t.draw(mSprite, s);             // underneath

        if (mExplosion->mVisible)
            t.draw(*mExplosion, tmp);             // draw explosion on top of it
    }
}

sf::Vector2f cEntity::update(sf::Time dt)
{
    // Exploding entities are not updated further. Once the explosion
    // update returns false (it's over), the entity is marked destroyed
    // so that it can be erased by cSceneContainer.
    if ( mStatus == StatusID::exploding )
    {
        if ( !(mExplosion->update(dt)) )
        {
            if ( mType != EntityType::player )
            {
                mStatus = StatusID::destroyed;
            }
            else  // whoops, player dead! but player's status is not immediately set to Destroyed,
                  // because we still need the pointer afterward: the playerdeath command will
                  // have something to do with it. AFTER the new player is created, we can get rid
                  // of the old one.
            {
                Command c;
                c.type = CommandType::playerdeath;
                c.p = this;
                mCQ.push(c);    // push "playerdeath" command on the command queue
            }
            return getPosition();
        }
        return getPosition();
    }
    
    // Now-
    // Why "switch polarity" when coming out of hitstun?
    // Because this will force the entity to regain its
    // original colour. So it's not "switching" polarity,
    // really, just making sure that the entity's colour
    // will once again match its polarity (this equasion
    // is upset when the Entity lights up when it's hit).
    
    if ( mHitStun > sf::Time::Zero )
    {
        mHitStun -= dt;
        if (mHitStun <= sf::Time::Zero)
        {
            mHit = false;
            switchPolarity(mPolarity, true);
        }
    }
    
    // OK, now, script execution
    
    if ( mScript.mSteps > 0 )
    {
        bool onrepeat = false;
        if ( mHelperTimer > sf::Time::Zero )
        {
            mHelperTimer -= dt;
            mTimer += dt;
            onrepeat = true;
        }
        else
        {
            mHelperTimer = sf::Time::Zero;
            mTimer += dt;
        }
        
        // advance timer; and if enough time has passed, execute the next
        // order, as pointed to by mInstrPtr.
        
        // HelperTimer is a timer used for repeating an action. As long as it's > 0,
        // we pretend that the timer just reached the trigger for the action that
        // mInstrPtr points to. But we don't actually move mInstrPtr forward!
        
        // Entities could in theory forward all their commands to the master command queue,
        // but it wouldn't make sense to go through that if the command only affects the
        // Entity itself. Therefore, move and rotate and setpol commands are handled here,
        // while fire commands are forwarded to the command queue. Spawn commands
        // in entity scripts are for now ignored.
        
        if ( mTimer >= mScript.mOrder[mInstrPtr].mTrigger )
        {
            Command c { mScript.mOrder[mInstrPtr].mCommand };
            switch ( c.type ) {
                case (CommandType::fire):
                {
                    if (mStatus != StatusID::onscreen) break;
                    c.p = this;
                    mCQ.push(c);
                    break;
                }
                case (CommandType::fire_secondary):
                {
                    c.p = this;
                    mCQ.push(c);
                    break;
                }
                case (CommandType::move):    // move commands are not forwarded to the main command
                {                            // queue - rather, they are resolved here.
                    setVelocity(c.vel);
                    break;
                }
                case (CommandType::accelerate):
                {
                    adjustVelocity(c.vel);
                    break;
                }
                case (CommandType::forward): // "forward" works w/ relative directions
                {
                    sf::Transform tmp { };
                    tmp.rotate(getRotation());
                    sf::Vector2f bvel { tmp.transformPoint(0.0, -c.rot) };
                    setVelocity(bvel);
                    break;
                }
                case (CommandType::turn):
                {
                    turn(c.rot);
                    break;
                }
                case (CommandType::speed_up):
                {
                    speedup(c.rot);
                    break;
                }
                case (CommandType::rotate):
                {
                    setRotate(c.rot);
                    break;
                }
                case (CommandType::setpolarity):
                {
                    if ( mType == EntityType::player )
                    {
                        c.p = this;
                        mCQ.push(c);
                    } else
                    switchPolarity(c.polarity);
                    break;
                }
                case (CommandType::shoot):
                {
                    // ignore for now - it should not be in entity scripts anyway.
                    // entities "Fire", but don't actually shoot - that will be done
                    // for them by the gameWorld object. that's because shooting
                    // (actually spawning the bullets) depends on info that
                    // gameworld has access to, but entities don't.
                    break;
                }
                case (CommandType::repeat):
                {
                    mHelperTimer = c.timer;
                    break;
                }
                case (CommandType::scale):
                {
                    setScaleFactor(c.rot);
                    break;
                }
                case (CommandType::scale_up):
                {
                    adjustScaleFactor(c.rot);
                    break;
                }
                case (CommandType::iddqd):
                {
                    setIDDQD(c.boolean);
                    break;
                }
                case (CommandType::track):
                {
                    // This needs to be pushed to the command queue, because here
                    // we have no idea of who to track (pointers to player1 or 2 or to none,
                    // if tracking is to stop). So it can't be handled locally.
                    c.p = this;
                    mCQ.push(c);
                    break;
                }
                case (CommandType::brown):
                {
                    // responsible for the Brownian movement of entities (bullets,
                    // mostly, that wiggle a bit around horizontally)
                    sf::Vector2f vtmp { static_cast<float>(rand() % gBrown - (gBrown / 2)), 0 };
                    adjustVelocity(vtmp);
                    break;
                }
                case (CommandType::rocketrequest):
                {
                    c.p = this;         // Give me a rocket, please.
                    mCQ.push(c);
                    break;
                }
                case (CommandType::selfdestruct):
                {
                    // No point in forwarding to the main command queue.
                    hit(mHP, EntityPolarity::neutral);
                    break;
                }
                default: break;
            }
            
            // If not on repeat, let's look at the next instruction;
            // if no instructions left, we'll start again from the
            // first (zeroth) one. Hence the use of %.
            
            if ( !onrepeat )
                ++mInstrPtr %= mScript.mSteps;
            
            mTimer = sf::Time::Zero;
        }
    }
    
    // Update velocity, tracking, rotation speed and kickbacks;
    // i.e. we'll empty the kickback queue and IMMEDIATELY ADJUST
    // the player's position. So this is not realistic: kickbacks
    // don't actually affect your velocity; they simply, literally,
    // push your ship into a new position that corresponds to the
    // kick's velocity. Not realistic, but it seems to work well,
    // and quite simple.
    
    if ( mVelocity.x || mVelocity.y ) move(mVelocity * dt.asSeconds());
    while ( !mKickback.empty() )
    {
        sf::Vector2f kick = mKickback.front();
        mKickback.pop();
        move(kick * dt.asSeconds());
    }
    
    if ( mTrackingTarget != nullptr ) track();
    if ( mRotSpeed ) rotate(mRotSpeed * dt.asSeconds());
    
    // Update "spawning sickness," or rather, spawning-induced
    // invulnerability
    if (mSpawning)
    {
        mSpawnTimeLeft -= dt;
        if ( mSpawnTimeLeft <= sf::Time::Zero )
            setSpawning(false);
        else
        {
                mDrawAura = (mSpawnTimeLeft.asMilliseconds() / 200) % 2;
        
            // we toggle the "mDrawAura" boolean every 200 milliseconds.
            // wow. such blink. so aura.
        }
    }
    
    // Update "sound cool down", i.e. it's not nice to play so many sounds in
    // such rapid succession, so the sound cool down has to expire before any
    // given entity can give out a new sound effect.
    
    if ( mSoundTimer > sf::Time::Zero ) mSoundTimer -= dt;
    
    // Entities return their position after their update.
    
    return getPosition();
}

void cEntity::switchPolarity(EntityPolarity e, bool force)
{
    if ((e != mPolarity) || (force == true))
    {
        if (e != EntityPolarity::alternate )
        {
            mPolarity = e;
            switch (mPolarity) {
                case (EntityPolarity::light):
                    mSprite.setColor(gCLight);
                    break;
                case (EntityPolarity::neutral):
                    mSprite.setColor(gCNeutral);
                    break;
                case (EntityPolarity::dark):
                    mSprite.setColor(gCDark);
                    break;
                default: break;
            }
        } else // e == EntityPolarity::alternate
        {
            switch (mPolarity) {
                case (EntityPolarity::light):
                {
                    mPolarity = EntityPolarity::dark;
                    mSprite.setColor(gCDark);
                    break;
                }
                case (EntityPolarity::dark):
                {
                    mPolarity = EntityPolarity::light;
                    mSprite.setColor(gCLight);
                    break;
                }
                default: break;
            }
        }
    }
}

void cEntity::hit(int dmg, EntityPolarity epol, sf::Vector2f kickVel, ShooterID shooter)
{
    if (mStatus == StatusID::exploding || mVulnerable == false) return;
    if (( epol == mPolarity ) && ( mType == EntityType::player )) heal(dmg);
    else
    {
        if ( mSpawning ) return;
        // We deal double damage to folks of other polarity
        if (( epol != mPolarity ) && ( mType != EntityType::player)) dmg *= 2;
        mHP -= dmg;
        if (mHP <= 0)
        {
            explode();
            if ( mType == EntityType::ship && shooter != ShooterID::nobody )
            {
                Command c;
                c.type = CommandType::increase_score;
                c.ScriptID = shooter == ShooterID::player1 ? "player01" : "player02";
                c.chargeAmount = mDMG;
                mCQ.push(c);
            }
            
            // If this entity was the final boss, we should let the world know
            // about that, and pass the current position so we know where we
            // can spawn the fireworks.
            if ( mFinalBoss )
            {
                Command c;
                c.type = CommandType::bossdeath;
                c.vel = getPosition();
                mCQ.push(c);
            }
        }
        else
        {
            mHit = true;
            mHitStun = sf::seconds(gHitStun);
            mSprite.setColor(sf::Color(255,255,255));   // light up on hit
        }
    }
    
    if ( (mType == EntityType::player) &&
        ((kickVel.x != 0.0) || (kickVel.y != 0.0)) )
        mKickback.push(kickVel);
}

void cEntity::heal(int amount)
{
    // Hmm question though - how do we know WHICH player to charge?
    // So far, we don't. Good thing we're set for 1 player only.
    // heal() will ONLY be called on EntityType::player, that's for sure.
    
    Command c;
    c.type = CommandType::charge_meter;
    c.chargeAmount = amount / gChargeRate;
    mCQ.push(c);
}

void cEntity::explode()
{
    if ((mType == EntityType::enemybullet) || (mType == EntityType::playerbullet))
    {
        mStatus = StatusID::destroyed;
        return;
    }
    
    mStatus = StatusID::exploding;
    mVelocity = sf::Vector2f(0,0);
    mRotSpeed = 0;
    int size = ((mHalfSize.x + mHalfSize.y) / 2) * 1.5;
    sf::Color tmpc = mPolarity == EntityPolarity::light ? sf::Color::Yellow : sf::Color::Red;
    std::unique_ptr<ExplosionObject> mEXP { new ExplosionObject { getPosition(), tmpc, size }};
    mExplosion = std::move(mEXP);
    
    // Explosions spawn small bullets - number depending on the size of
    // the exploding ship. The bullets will share the object's polarity
    // and perform Brownian movement.
    // Whether the exploding bullets ACTUALLY get spawned is a decision
    // that the gameWorld will make, on the basis of the difficulty level
    // (easy: no explosion bullets, normal: only explosions bullets that share the
    // polarity of the player, hard: always spawn all explosion bullets).
    
    Command c;
    c.type = CommandType::explode_spawn;
    c.ep.polarity = mPolarity;
    c.ep.pos = getPosition();
    c.ep.count = (mHalfSize.x + mHalfSize.y) / 35;
    mCQ.push(c);
}

const sf::FloatRect cEntity::getAABB() const
{
    // For now, let's make it simple. AABB's shall be chosen wisely, so as to cover a central area
    // of entities that roughly stays the same no matter how the entity rotates.
    sf::Vector2f pos { getPosition() };
    return sf::FloatRect { pos.x + mAABB.left * mScaleFactor, pos.y + mAABB.top * mScaleFactor,
        mAABB.width * mScaleFactor, mAABB.height * mScaleFactor };
}

const sf::Vector2f cEntity::getTurretPosition(sf::Vector2f rel) const
{
    return getTransform().transformPoint(mTurretPosition+rel);
}

void cEntity::track()    // This basically only sets RotSpeed + or -, depending on how we should turn.
{
    // I'm not interested in mVelocity, but in rotation! So I take the convenient straight up
    // unit vector and rotate it with the actual rotation.
    sf::Transform tmp;
    tmp.rotate(getRotation());
    sf::Vector2f rotated_unit { tmp.transformPoint(sf::Vector2f(0, -1)) };
    
    sf::Vector2f normal { rotated_unit.y, -rotated_unit.x };
    sf::Vector2f target { mTrackingTarget->getPosition() };
    
    sf::Vector2f direct { target - getPosition() }; // direct: as in DIRECTion
    
    // Now what we're interested in is the angle between "direct" and "rotated_unit",
    // as well as "direct" and "normal."
    
    auto dot = [](sf::Vector2f a, sf::Vector2f b) { return (a.x * b.x) + (a.y * b.y); };
    auto length = [](const sf::Vector2f& a) { return sqrt( (a.x * a.x) + (a.y * a.y) ); };
    
    float dlen = length(direct);
    
    float odeg_a = acos(dot(direct, rotated_unit) / dlen * length(rotated_unit));
    float odeg_b = acos(dot(direct, normal) / dlen * length(normal));
    
    if (odeg_a > 3.141592 / 180 * 3)
    {
        if ((odeg_a < 3.141592 / 2) && (odeg_b > 3.141592 / 2))
            mRotSpeed = mTrackingSpeed;
        else mRotSpeed = -mTrackingSpeed;
    }
    else mRotSpeed = 0; // if turned toward the tracking object to within 3 degrees, stop turning
}