#ifndef __shmup_project__sceneContainer__
#define __shmup_project__sceneContainer__

#include "globalVariables.h"
#include <SFML/Graphics.hpp>
#include <list>
#include "entities.h"
#include "rockets.h"

// cGridContainer is the class for handling the grid that's maintained for
// collision detection.
// The screen is divided into a grid of nGridX * nGridY cells. Each cell
// is allowed to contain nMaxEntities Entities. Cells are represented by
// struct Cell, which contains a fixed size array of pointers to Entity -
// this is done so that we don't have to dynamically allocate memory
// to each cell when in need of resizing them (no need to bother about
// vectors or anything).
// Then grid itself is accessed via cell[nGridX][nGridY], which identifies
// a particular Cell. So, the first entity in the first grid cell
// can be accessed at grid.cell[0][0].items[0], for example.

// "Clearing" a cell - which must happen in every update - is as quick
// and simple as setting the nEntities variable to 0. The items[] array
// will still contain pointers (possibly), that point to entities that
// used to be in that cell; but that's OK, we won't ever refer to those
// pointers again. This is much quicker than setting each pointer in items[]
// to nullptr, or to use a vector<> instead of a fixed size array.

class cGridContainer {
public:
    cGridContainer();
    ~cGridContainer();

    static const int    nGridX = 5;
    static const int    nGridY = 5;
    static const int    nMaxEntities = 100;
    
    struct      Cell {
        cEntity*    items[nMaxEntities];
        int         nEntities;
    };
    
    Cell        cell[nGridX][nGridY];

public:
    void        append(int, int, cEntity*);
    void        reset();
};

class cSceneContainer {
public:
    cSceneContainer(CQtype&, sf::RenderWindow&);
    unsigned int    update(sf::Time);   // Update returns the number of non-player entities currently on scene
    
    void            render(sf::RenderTarget&, sf::RenderStates);
    void            addEntity(std::unique_ptr<cEntity>);
    StatusID        onScreen(const sf::Vector2f&);
    bool            launchRockets(const sf::Vector2f&, int, EntityPolarity);
    cEntity**       setTargets(int, EntityPolarity);
                    // Returns a pointer to an array of Entities, of int size, and
                    // of EP polarity (if neutral, it'll be mixed). The size of
                    // the array equals the number of homing beams fired.
    void            updateTracking(cEntity* old, cEntity* fresh)
                    {
                        for (auto& i : mLevel0)
                            i->updateTarget(old, fresh);
                    }

public:
    bool                                mSlowdown;
    CQtype&                             mCQ;
    std::unique_ptr<cGridContainer>     mGrid;
    sf::Vector2u                        mWindowSize;
    sf::Vector2u                        mGridSize;

private:
    int                                 collisionCheck();
    
private:
    const float                             mBufferSize = 200.0;
    sf::FloatRect                           mScreen;
    sf::FloatRect                           mBuffer;
    std::list<std::unique_ptr<cEntity>>     mLevel0;
    std::list<std::unique_ptr<cRocket>>     mRockets;
    cEntity*                                mLightEnemies[gMaxEnemyCount];
    cEntity*                                mDarkEnemies[gMaxEnemyCount];
    cEntity*                                mLightBosses[gMaxEnemyCount];
    cEntity*                                mDarkBosses[gMaxEnemyCount];
    cEntity*                                mTargetList[gMaxEnemyCount];
    int                                     mLightEnemyCount;
    int                                     mDarkEnemyCount;
    int                                     mLightBossCount;
    int                                     mDarkBossCount;
};

#endif /* defined(__shmup_project__sceneContainer__) */
