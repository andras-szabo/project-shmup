#include "shotscripts.h"
#include <fstream>

bool cShotScript::loadFromFile(const std::string& filename)
{
    std::ifstream in { filename + ".ssp" };
    std::string stmp;
    in >> stmp;
    // First check what sound effect accompanies the shot.
    mSFX = gSFXMap[stmp];
    
    // Then we read info about each bullet.
    in >> stmp;
    while ( stmp == "BULLET" )
    {
        BulletInfo b;
        in >> b.id;
        in >> b.pos.x >> b.pos.y;
        in >> b.vel.x >> b.vel.y;
        in >> b.rot;
        in >> b.script;
        
        mOrder.push_back(b);
        in >> stmp;
    }
    in.close();
    return true;
}