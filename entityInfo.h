#ifndef __shmup_project__entityInfo__
#define __shmup_project__entityInfo__

// OK so cEntityHolder doesn't actually hold entities,
// rather, it holds INFO about entities. When gameWorld
// spawns an entity, it will look up info available in these
// entityholders to know its characteristics.

#include <SFML/Graphics.hpp>
#include "globalVariables.h"

class cEntityHolder {
public:
    bool                loadFromFile(const std::string&);
    
    sf::IntRect         mTexRect;
    sf::FloatRect       mAABBRect;
    sf::Vector2f        mTurretPosition;
    std::string         mID;
    std::string         mTextureFile;
    std::string         mShotType;
    std::string         mSecondShotType;
    EntityType          mType;
    bool                mBoss;
    int                 mHP;            // for ships
    int                 mDMG;           // for bullets
    bool                mKick;          // for bullets: does this bullet kick back?
    bool                mFinalBoss;     // is this the final boss?
};
#endif /* defined(__shmup_project__entityInfo__) */
