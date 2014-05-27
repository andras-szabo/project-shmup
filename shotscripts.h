#ifndef __shmup_project__shotscripts__
#define __shmup_project__shotscripts__

#include <SFML/Graphics.hpp>
#include "globalVariables.h"

// Shoot scripts tell us what to do upon a given
// "Fire" command: spawn as many bullets, with as many
// parameters, as we find in mOrder.

struct BulletInfo {
    BulletInfo() { };
    sf::Vector2f        pos;
    sf::Vector2f        vel;
    float               rot;
    std::string         script;
    std::string         id;
};

class cShotScript {
public:
    
    std::vector<BulletInfo>     mOrder;
    SfxID                       mSFX;
    bool                        loadFromFile(const std::string&);
};

#endif /* defined(__shmup_project__shotscripts__) */
