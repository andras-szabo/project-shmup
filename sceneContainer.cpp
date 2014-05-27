#include "sceneContainer.h"
#include "globalVariables.h"

// Default constructor: initializes the grid with
// nullptrs and 0s.
cGridContainer::cGridContainer()
{
    for ( auto i = 0; i < nGridX; ++i )
        for ( auto j = 0; j < nGridY; ++j )
        {
            for ( auto k = 0; k < nMaxEntities; ++k )
                cell[i][j].items[k] = nullptr;
            cell[i][j].nEntities = 0;
        }
}

cGridContainer::~cGridContainer()
{
    
}

void cGridContainer::append(int x, int y, cEntity* e)
{
    // we avoid adding entities to cells after the
    // limit was reached. but first we check if we're trying
    // to append to a valid cell
    if ( (x >= 0) && (x < nGridX) && (y >= 0) && (y < nGridY) )
        if ( cell[x][y].nEntities < nMaxEntities )
            cell[x][y].items[cell[x][y].nEntities++] = e;
    
}

void cGridContainer::reset()
{
    for ( auto i = 0; i < nGridX; ++i )
        for ( auto j = 0; j < nGridY; ++j )
            cell[i][j].nEntities = 0;
}

cSceneContainer::cSceneContainer(CQtype& c, sf::RenderWindow& w):
mCQ { c },
mGrid { new cGridContainer() },
mGridSize { gViewSize.x / mGrid->nGridX, gViewSize.y / mGrid->nGridY },
mScreen { sf::FloatRect { -20.0f, -20.0f, static_cast<float>(gViewSize.x + 20.0), static_cast<float>(gViewSize.y + 20.0) } },
mBuffer { sf::FloatRect { -mBufferSize, -mBufferSize,
                            gViewSize.x + mBufferSize,
                            gViewSize.y + mBufferSize } },
mSlowdown { false } // start w/o slowdown
{

}

// For now, all entities are pushed onto Level0;
// other levels - perhaps when parallax effects will be
// implemented.

void cSceneContainer::addEntity(std::unique_ptr<cEntity> p)
{
    mLevel0.push_back(std::move(p));
}

// Checks if a given Entity is on screen, not on screen but in the buffer
// region, or off the buffer region. In this latter case, it can (should)
// be destroyed.
StatusID cSceneContainer::onScreen(const sf::Vector2f& pos)
{
    if ( (pos.x >= mScreen.left) && (pos.x <= mScreen.width) &&
         (pos.y >= mScreen.top) && (pos.y <= mScreen.height)) return StatusID::onscreen;
    
    if ( (pos.x >= mBuffer.left) && (pos.x <= mBuffer.width) &&
         (pos.y >= mBuffer.top) && (pos.y <= mBuffer.height)) return StatusID::onbuffer;
    
    return StatusID::destroyed;
}

// OK, so the following method is where the magic is. It checks LOCAL
// coordinates, and expects such as input. It returns the number of managed
// _non-player_ _ships_
unsigned int cSceneContainer::update(sf::Time dt)
{
    mGrid->reset();             // reset screen grid
    mLightEnemyCount = 0;
    mDarkEnemyCount = 0;
    mLightBossCount = 0;
    mDarkBossCount = 0;
    
    // For testing:
    bool vulnerability = true;
    
    // First: update each managed entity
    for ( auto& i : mLevel0 )
    {
        sf::Vector2f pos;
        if ( mSlowdown )
        {
            if ( i->mType == EntityType::player )
                pos = i->update(dt);
            else
                pos = i->update(dt / gSlowFactor);
        }
        else pos = i->update(dt);
        
        // Let's not let players leave the screen
        if ( i->mType == EntityType::player )
        {
            if ( pos.x < mScreen.left + 15 ) pos.x = mScreen.left + 15;
            else if ( pos.x > mScreen.width - 15 ) pos.x = mScreen.width - 15;
            
            if ( pos.y < mScreen.top + 15 ) pos.y = mScreen.top + 15;
            else if ( pos.y > mScreen.height - 15 ) pos.y = mScreen.height - 15;
            
            i->setPosition(pos.x, pos.y);
        }
        
        // On the basis of the updated position, let's update the entity's status.
        if ( (i->mStatus != StatusID::destroyed) && (i->mStatus != StatusID::exploding) )
            i->mStatus = onScreen(pos);
        
        // Now add entities to the screen grid, based on the position of their AABB's 4
        // corner points; and update enemy lists. An enemy could be registered to multiple
        // cells in the grid, based on the position of its AABB.
        if (( i->mStatus == StatusID::onscreen ) && i->mVulnerable )
        {
            cEntity* e = i.get();
            sf::FloatRect   box { e->getAABB() };
            sf::Vector2i    topleft { static_cast<int>(box.left / mGridSize.x), static_cast<int>(box.top / mGridSize.y) };
            sf::Vector2i    topright { static_cast<int>((box.left + box.width) / mGridSize.x), static_cast<int>(box.top / mGridSize.y) };
            sf::Vector2i    botleft { static_cast<int>(box.left / mGridSize.x), static_cast<int>((box.top + box.height) / mGridSize.y) };
            sf::Vector2i    botright { static_cast<int>((box.left + box.width) / mGridSize.x),
                                       static_cast<int>((box.top + box.height) / mGridSize.y) };
            
            mGrid->append(topleft.x, topleft.y, e);
            if ( topright != topleft ) mGrid->append(topright.x, topright.y, e);
            if ( (botleft != topleft) && (botleft != topright)) mGrid->append(botleft.x, botleft.y, e);
            if ( (botright != topleft) && (botright != topright) &&
                 (botright != botleft)) mGrid->append(botright.x, botright.y, e);
            
            // Now update type-separated lists of vulnerable entities on screen
            
            if ( i->mType == EntityType::ship )
            {
                if ( !i->mBoss )
                {
                    if ((i->mPolarity == EntityPolarity::light) && (mLightEnemyCount < gMaxEnemyCount-1))
                        mLightEnemies[mLightEnemyCount++] = e;
                    else
                        if ((i->mPolarity == EntityPolarity::dark) && (mDarkEnemyCount < gMaxEnemyCount-1))
                            mDarkEnemies[mDarkEnemyCount++] = e;
                } else // we're dealing with a boss here!
                {
                    if ((i->mPolarity == EntityPolarity::light) && (mLightBossCount < gMaxEnemyCount-1))
                        mLightBosses[mLightBossCount++] = e;
                    else
                        if ((i->mPolarity == EntityPolarity::dark) && (mDarkBossCount < gMaxEnemyCount-1))
                            mDarkBosses[mDarkBossCount++] = e;
                }
            }
            
        }
    }
    
    // OK. Now let's do the collision checks.
    if ( vulnerability ) collisionCheck();

    // Now update list of rockets (homing beams); remove
    // those that are destroyed, so that rendering won't have
    // to check that.
    if ( mSlowdown ) dt /= gSlowFactor;
    for ( auto& i : mRockets ) i->update(dt);
    for ( auto it = mRockets.begin(); it != mRockets.end(); ++it )
        if ((*it)->mBeamActive == false) it = mRockets.erase(it);
    
    
    // Finally: let's go through the updated list of entities and remove those
    // that are destroyed. Then return the number of not-destroyed, non-exploding
    // ships/walls on scene.
    int entityCount { 0 };
    
    mLevel0.remove_if( [] (const std::unique_ptr<cEntity>& p)
                      { return p->mStatus == StatusID::destroyed; } );
    
    for (auto& i : mLevel0)
        if ((i->mType == EntityType::ship || i->mType == EntityType::wall)
            && i->mStatus != StatusID::exploding )
                ++entityCount;
    
    return entityCount;
}

// collisionCheck returns the number of checks made.
int cSceneContainer::collisionCheck()
{
    int checkCount { 0 };
    for ( auto i = 0; i < mGrid->nGridX; ++i )
        for ( auto j = 0; j < mGrid->nGridY; ++j )
            for ( auto p = 0; p < mGrid->cell[i][j].nEntities; ++p )
            {
                cEntity* a { mGrid->cell[i][j].items[p] };               // for readability.
                if ((a->mType == EntityType::player) ||
                    (a->mType == EntityType::ship) ||
                    (a->mType == EntityType::wall))
                {
                    for ( auto q = 0; q < mGrid->cell[i][j].nEntities; ++q )
                        if ( q != p )                                   // we don't check entities with themselves
                        {
                            cEntity* b { mGrid->cell[i][j].items[q] };   // for readability. we can now check 'a' vs 'b'
                            if
                (
    (a->mType == EntityType::player && (b->mType == EntityType::enemybullet || b->mType == EntityType::ship)) ||
    (a->mType == EntityType::ship && (b->mType == EntityType::playerbullet || b->mType == EntityType::wall)) ||
    (a->mType == EntityType::wall && (b->mType == EntityType::playerbullet || b->mType == EntityType::player
                                || b->mType == EntityType::enemybullet))
                 )
                            {
                                ++checkCount;
                                if ( a->getAABB().intersects(b->getAABB()) )
                                {
                                    if (a->mType == EntityType::player)
                                    {
                                        if (b->mType == EntityType::enemybullet)        // Player vs Enemybullet
                                        {
                                            sf::Vector2f kickVelocity { b->getVelocity() };
                                            if ( !b->mKick ) kickVelocity = { 0.0, 0.0 };
                                            else
                                            {
                                                kickVelocity.x /= 2;
                                                kickVelocity.y /= 2;
                                            }
                                            a->hit(b->mDMG, b->mPolarity, kickVelocity);
                                            b->hit(b->mHP, b->mPolarity);      // Bullets destroyed on hit
                                        }
                                        else                                            // Player vs Ship
                                        {
                                            a->hit(b->mDMG, EntityPolarity::neutral);
                                            b->hit(a->mDMG, EntityPolarity::neutral);
                                        }
                                    } else
                                        if (a->mType == EntityType::ship)
                                        {
                                            if (b->mType == EntityType::playerbullet)   // Ship vs PlayerBullet
                                            {
                                                // if ship is hit, we indicate who it was shot
                                                // by; because of default arguments, need to pass
                                                // an empty vector too (for kickback)
                                                a->hit(b->mDMG, b->mPolarity, sf::Vector2f(0,0), b->mShooter);
                                                b->hit(b->mHP, b->mPolarity);
                                            }
                                            else                                        // Ship vs Wall
                                                a->hit(a->mHP, a->mPolarity);
                                        }
                                    else    // a->mType == Wall. Everything is destroyed when hitting a
                                            // wall; but bullets can also damage the wall.
                                    {
                                        if (b->mType == EntityType::playerbullet
                                            || b->mType == EntityType::enemybullet )
                                            a->hit(b->mDMG, b->mPolarity);
                                        b->hit(b->mHP, EntityPolarity::neutral);
                                    }
                
                                }
                            }
                        }
                }
            }
    return checkCount;
}

void cSceneContainer::render(sf::RenderTarget& t, sf::RenderStates s)
{
    for ( auto& i : mLevel0 )       // entities
    {
        if ( ((*i).mStatus == StatusID::onscreen) ||
             ((*i).mStatus == StatusID::exploding)) t.draw(*i, s);
    }
    for ( auto& i : mRockets )      // then rocket beams
        i->render(t, s);
}

// Produces an array of pointers-to-Entity, of designated polarity,
// for homing beams to use.
cEntity** cSceneContainer::setTargets(int num, EntityPolarity pol)
{
    for ( auto& i: mTargetList ) i = nullptr;   // resetting target list
    if ( num > gMaxTargetCount ) num = gMaxTargetCount;
    if ( pol == EntityPolarity::light )
    {
        if ( mLightBossCount > 0 )
        {
            for ( auto i = 0; i < num; ++i )
                mTargetList[i] = mLightBosses[ i % mLightBossCount ];
            return &mTargetList[0];
        }
        if ( mLightEnemyCount > 0 )
        {
            for ( auto i = 0; i < num; ++i )
                mTargetList[i] = mLightEnemies[i % mLightEnemyCount ];
            return &mTargetList[0];
        }
        return &mTargetList[0];
    }
    
    if ( pol == EntityPolarity::dark )
    {
        if ( mDarkBossCount > 0 )
        {
            for ( auto i = 0; i < num; ++i )
                mTargetList[i] = mDarkBosses[i & mDarkBossCount];
            return &mTargetList[0];
        }
        if ( mDarkEnemyCount > 0 )
        {
            for ( auto i = 0; i < num; ++i )
                mTargetList[i] = mDarkEnemies[i % mDarkEnemyCount];
            return &mTargetList[0];
        }
        return &mTargetList[0];
    }
    
    // If we get to this point, then we're in for mixed polarity shots.
    
    if (mLightBossCount || mDarkBossCount)
    {
        int i = 0;
        while ( i < num )
        {
            if (mLightBossCount)
            {
                mTargetList[i] = mLightBosses [ (i / 2) % mLightBossCount ];
                ++i;
            }
            if ( i >= num ) return &mTargetList[0];
            if (mDarkBossCount)
            {
                mTargetList[i] = mDarkBosses [ (i / 2) % mDarkBossCount ];
                ++i;
            }
        }
        return &mTargetList[0];
    }
    
    if (!(mLightEnemyCount || mDarkEnemyCount)) return nullptr;
    int i = 0;
    while ( i < num )
    {
        if ( mLightEnemyCount )
        {
            mTargetList[i] = mLightEnemies[ (i / 2) % mLightEnemyCount ];
            ++i;
        }
        if ( i >= num ) return &mTargetList[0];
        if ( mDarkEnemyCount )
        {
            mTargetList[i] = mDarkEnemies[ (i / 2) % mDarkEnemyCount ];
            ++i;
        }
    }
    return &mTargetList[0];
}

// Launches rockets. Returns true if rockets were launched
// (there were targets around). Otherwise we don't
// alter the state of mPlayerData.superMeter. (I.e. you don't
// lose meter if you press the rocket button while there
// are no targets around.)
bool cSceneContainer::launchRockets(const sf::Vector2f& pos,
                                   int count,
                                   EntityPolarity pol)
{
    // Are there any targets around?
    // Let's check, for now: using the default target
    // seeking priorities (everything counts <= EPol::neutral)
    
    cEntity** e = setTargets(count, EntityPolarity::neutral);
    if ((e == nullptr) || (e[0]==nullptr))
        return false; // no targets around.
    
    // If there are targets around, then let's set up a
    // new set, designate their targets, and attach this new
    // set to our list of rockets.
    
    std::unique_ptr<cRocket> r { new cRocket(pos, count, pol, e, ShooterID::player1) };
    mRockets.push_back(std::move(r));
    return true;
}