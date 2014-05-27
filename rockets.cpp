#include "rockets.h"
#include "globalVariables.h"

HomingBeam::HomingBeam(const sf::Vector2f& pos, const sf::Color& col, size_t num, int thi):
mNumBeams { num },
mColor { col },
mThickness { thi }
{
    clear();
    
    // Setting up starting line of the strips,
    // and starting velocities so that beams are a bit spread
    // out like a fan.
    
    for(auto i = 0; i < mNumBeams; ++i)
    {
        mP[i][0].position = pos;
        mP[i][0].position.x -= mThickness / 6;
        mP[i][1].position = pos;
        mP[i][1].position.x += mThickness / 6;
        
        mP[i][0].color = mColor;
        mP[i][1].color = mColor;
        
        if ( i % 2 )
        {
            mVelocity[i][0].x = -80 + i * 16;
            mVelocity[i][1].x = -80 + i * 16;
        } else
        {
            mVelocity[i][0].x = 80 - i * 16;
            mVelocity[i][1].x = 80 - i * 16;
        }
        mVelocity[i][0].y = i < 5 ? -20 + i * 8 : 40 - ((i-5)*8);
        mVelocity[i][1].y = i < 5 ? -20 + i * 8 : 40 - ((i-5)*8);
    }
}

bool HomingBeam::update(sf::Time dt,
                        const sf::Vector2f target[])
{
    //if (!mDrawingFinished)            // see comments below: this might be commented out or in
    //{
    mVisible = true;
    mTimer += dt;
    while ((mTimer >= sf::seconds(mUnitTime * mDrawPointer) && (!mDrawingFinished)))
    {
        for (auto i = 0; i < mNumBeams; ++i)
        {
            // Left line
            sf::Vector2f targeta { target[i] - mP[i][mDrawPointer].position };
            if (mDrawPointer < mSize * 0.2)
            {   targeta.x -= mThickness/4;
                targeta.y -= mThickness/4; }
            if (mDrawPointer < mSize * 0.95)
            {   targeta.x -= mThickness / 4;
                targeta.y -= mThickness / 4; }
            
            targeta.x /= (mSteps - mDrawPointer);
            targeta.y /= (mSteps - mDrawPointer);
            mVelocity[i][0] += targeta;
            mP[i][mDrawPointer + 2].position = mP[i][mDrawPointer].position + mVelocity[i][0];
            mVelocity[i][0].x /= 2;
            mVelocity[i][0].y /= 2;
            mP[i][mDrawPointer+2].color = mColor;
            
            // Right line
            sf::Vector2f targetb { target[i] - mP[i][mDrawPointer+1].position};
            
            // To make the beam pointy:
            if (mDrawPointer < mSize * 0.2)
            {   targetb.x += mThickness/4;
                targetb.y += mThickness/4; }
            if (mDrawPointer < mSize * 0.95)
            {   targetb.x += mThickness / 4;
                targetb.y += mThickness / 4; }
            targetb.x /= (mSteps - mDrawPointer);
            targetb.y /= (mSteps - mDrawPointer);
            mVelocity[i][1] += targetb;
            mP[i][mDrawPointer + 3].position = mP[i][mDrawPointer+1].position + mVelocity[i][1];
            mVelocity[i][1].x /= 2;
            mVelocity[i][1].y /= 2;
            mP[i][mDrawPointer+3].color = mColor;
        }
        mDrawPointer += 2;    // Move on to next triangle in strip
        if (mDrawPointer >= mTriangles) mDrawingFinished = true;
    }
    //  }
    //  else // -> yes, drawing is finished! -> if this bit is commented out, then drawing
    //  {                                       doesn't need to finish before dissolving.
    mVisible = false;
    for(auto i = 0; i < mNumBeams; ++i)
        for(auto j = 0; j < mSize; ++j)
        {
            mT[i][j] -= dt;
            if (mT[i][j] > sf::Time::Zero)
            {
                float ratio = mT[i][j].asSeconds() / Beam_LifeTime.asSeconds();
                mP[i][j].color.a = static_cast<sf::Uint8>(300 * std::max(ratio, 0.f));
                mVisible = true;
            } else
            {
                mP[i][j].color = sf::Color::Transparent;
            }
            
        }
    // }
    return mVisible;
}

void HomingBeam::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = nullptr;
    for (auto i = 0; i < mNumBeams; ++i)
        target.draw(&mP[i][0], mDrawPointer, sf::TrianglesStrip, states);
}

void HomingBeam::clear()
{
    if (mNumBeams > MAXBEAMS) mNumBeams = MAXBEAMS;
    for (auto i = 0; i < mNumBeams; ++i)
        for(auto j = 0; j < mSize; ++j)
        {
            mP[i][j].color = sf::Color::Transparent;
            mT[i][j] = Beam_LifeTime - sf::milliseconds((mSize-j));
        }
    
    mDrawPointer = 0;
    mTimer = sf::Time::Zero;
    mVisible = true;
    mDrawingFinished = false;
    mUnitTime = Beam_DrawTime.asSeconds() / mTriangles;
}

cRocket::cRocket(sf::Vector2f pos,
               int count,
               EntityPolarity pol,
               cEntity** e,
               ShooterID shooter):
mTargetCount { count },
mBeamPolarity { pol },
mBeamActive { true },
mBeamHit { false },
mBeamDmg { gRocketDMG },
mShooter { shooter }
{
    for (auto i = 0; i < mTargetCount; ++i)
        mTargets[i] = e[i];
        
        if ( pol == EntityPolarity::light)
        {
            mBackgroundBeamColor = sf::Color::White;
            mForegroundBeamColor = sf::Color::Cyan;
        } else
        {
            mBackgroundBeamColor = sf::Color::White;
            mForegroundBeamColor = sf::Color::Red;
        }
    
    // oops. naked "new". might lead to trouble.
    
    mBackgroundBeam = new HomingBeam(pos, mBackgroundBeamColor, mTargetCount, mBackgroundBeamWidth);
    mForegroundBeam = new HomingBeam(pos, mForegroundBeamColor, mTargetCount, mForegroundBeamWidth);
    
    // Let's set up the shockwave rings.
    // For now radii are hard coded; this seems to work so sod it.
    
    // A shockwave is an outer ring corresponding to the beam's foreground color,
    // only more transparent; and an inner ring which is always white, but
    // also more transparent.
    
    mRing0.setRadius(25.0);
    mForegroundBeamColor.a = 60;
    mRing0.setOutlineColor(mForegroundBeamColor);
    mRing0.setFillColor(sf::Color(0,0,0,0));
    mRing0.setOutlineThickness(15.0);
    mRing0.setPosition(pos.x - 25.0, pos.y - 25.0);
    
    mRing1.setRadius(5.0);
    mRing1.setOutlineColor(sf::Color(150, 150, 150, 110));
    mRing1.setFillColor(sf::Color(0,0,0,0));
    mRing1.setOutlineThickness(15.0);
    mRing1.setPosition(pos.x - 5.0, pos.y - 5.0);
}

void cRocket::update(sf::Time dt)
{
    // If a target was removed, then set its pointer
    // to nullptr, to prevent it pointing somewhere else.
    for (auto i = 0; i < mTargetCount; ++i)
        if ( mTargets[i] != nullptr)
        {
            if (mTargets[i]->mStatus == StatusID::destroyed)
            {
                mTargets[i] = nullptr;
            }
            else
            {
                mTargetCoords[i] = mTargets[i]->getPosition();
            }
        }
    
    mBackgroundBeam->update(dt, mTargetCoords);
    mForegroundBeam->update(dt, mTargetCoords);
    
    // Now we check if things were hit. This will call
    // targets' hit() methods, which can set their states
    // to "destroyed." But that should be ok. Suppose that
    // two players shoot rockets at the same target.
    // When the first rocket hits it, it's Status is set to
    // destroyed. In the second rocket's update, BEFORE
    // the Entity is actually removed from the scene,
    // the second rocket's Target pointer is set to
    // nullptr. So this should be fine.
    
    // We hit things if the beam's drawing is finished AND
    // we haven't hit anything before.
    
    if ((mBackgroundBeam->mDrawingFinished == true) &&
        (mBeamHit == false))
    {
        for (int i = 0; i < mTargetCount; ++i)
            if (mTargets[i] != nullptr)
                mTargets[i]->hit(mBeamDmg, mBeamPolarity, sf::Vector2f(0,0), mShooter);
        mBeamHit = true;
    }
    
    // Finally, if the beam is no longer visible, then
    // we can call it a day too: mark this Rocket object
    // for deletion, and delete the pointers to beams.
    
    if (!(mBackgroundBeam->mVisible))
        mBeamActive = false;
    
    // Update shockwave rings
    
    sf::Vector2f pos = mRing1.getPosition();
    float radius = mRing1.getRadius();
    float dradius = 1000 * dt.asSeconds();
    radius += dradius;
    mRing1.setRadius(radius);
    mRing1.setPosition(pos.x - dradius, pos.y - dradius);
    
    pos = mRing0.getPosition();
    radius = mRing0.getRadius();
    dradius = 1800 * dt.asSeconds();
    radius += dradius;
    mRing0.setRadius(radius);
    mRing0.setPosition(pos.x - dradius, pos.y - dradius);
}

void cRocket::render(sf::RenderTarget& target,
                    sf::RenderStates states)
{
    target.draw(*mBackgroundBeam, states);
    target.draw(*mForegroundBeam, states);
    target.draw(mRing0, states);
    target.draw(mRing1, states);
}