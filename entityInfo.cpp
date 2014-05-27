#include "entityInfo.h"
#include <fstream>

bool i(std::ifstream& in, const std::string& s)
{
    std::string stmp;
    in >> stmp;
    return stmp == s;
}

bool cEntityHolder::loadFromFile(const std::string& filename)
{
    std::ifstream in { filename + ".ent" };
    std::string stmp;
    
    if ( !i(in,"ID") ) return false;
    in >> mID;  // Entity name ( = ID)
    
    if ( !i(in,"TYPE") ) return false;
    in >> stmp;
    if ( stmp == "ship" )
    {
        mType = EntityType::ship;
        mBoss = false;
    }
    if ( stmp == "boss" )
    {
        mType = EntityType::ship;
        mBoss = true;
    }
    if ( stmp == "finalboss" )
    {
        mType = EntityType::ship;
        mBoss = true;
        mFinalBoss = true;
    }
    if ( stmp == "bullet" ) mType = EntityType::bullet;
    if ( stmp == "player" ) mType = EntityType::player;
    if ( stmp == "wall" ) mType = EntityType::wall;
    
    if (!i(in,"TEXTURE")) return false;
    in >> mTextureFile;
    
    if (!i(in,"TEXRECT")) return false;
    in >> mTexRect.left >> mTexRect.top >> mTexRect.width >> mTexRect.height;
    
    if (!i(in,"AABB")) return false;
    in >> mAABBRect.left >> mAABBRect.top >> mAABBRect.width >> mAABBRect.height;
    
    if (!i(in,"HPDMG")) return false;
    if ( stmp == "ship" || stmp == "player" || stmp == "boss" ||
             stmp == "wall"  || stmp == "finalboss" ) { in >> mHP; mDMG = mHP / 2; }      // in case of non-bullet entities,
    if ( stmp == "bullet" ) { in >> mDMG; mHP = 1; }                // mDMG is also the score the player gets
                                                                    // when blowing that entity up
    if ( mType == EntityType::ship || mType == EntityType::player )
    {
        if (!i(in,"TURRET")) return false;
        in >> mTurretPosition.x >> mTurretPosition.y;

        if (!i(in,"SHOTS")) return false;
        in >> mShotType >> mSecondShotType;
        mKick = false;
    }
    else if ( mType == EntityType::bullet )
    {
        mTurretPosition = sf::Vector2f { };
        mShotType = " ";
        if (!i(in,"KICKBACK")) return false;
        in >> stmp;
        mKick = (stmp == "true") ? true : false;
    }
    in >> stmp;
    in.close();
    return true;
}
