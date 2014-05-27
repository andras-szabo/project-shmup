#ifndef __shmup_project__gameWorld__
#define __shmup_project__gameWorld__

#include "globalVariables.h"
#include "commands.h"
#include "resourceHolders.h"
#include "scripts.h"
#include "shotscripts.h"
#include "entityInfo.h"
#include <queue>
#include "sceneContainer.h"
#include "player.h"
#include "bkGrid.h"

typedef ResourceHolder<sf::Font, FontID> fHolder;
typedef ResourceHolder<sf::Texture, TexID> tHolder;

class GameWorld : sf::NonCopyable {
public:
    GameWorld(CQtype&, fHolder&, tHolder&,
              sf::RenderWindow&,
              Difficulty&,
              const std::string&);
    void            update( sf::Time );
    void            render();
    void            playerShoot(bool secondary = false);
    void            playerMove(bool*);
    void            playerSwitch();
    void            playerRockets();
    void            playerAccelerate(float, float, bool userInput = true);
    void            slowdown();
    void            adjustBkGamma(short int c)
                    { mBkGrid.adjustColor(c); }
    void            setUpSoundQueue(std::queue<SfxID>* p)
                    { pSoundQueue = p; }
    unsigned int    getScore() const;

    
private:
    void                        loadStage();
    sf::Transform&              getViewTransform() const;


public:
    std::queue<std::string>     msgQueue;   // Message queue - this is how it can communicate with
                                            // its parent cGameState.
    bool                        mPlayerActive;  // Is the player active? (She isn't in the tutorial,
                                                // so no need to check for player input, except for
                                                // things such as pausing the game.)
    unsigned int                mBulletsFired;
    unsigned int                mEnemiesKilled;
    unsigned int                mPolChgCount;
    bool                        mLifeLost;
    bool                        mBonusStage;    // if true, we get a different kinda shot
                                                // set by message _BONUS_

    
private:
    // References to global resources:
    
    CQtype&                                 mCQ;
    ResourceHolder<sf::Font, FontID>&       mFontHolder;
    ResourceHolder<sf::Texture, TexID>&     mTextureHolder;
    sf::RenderWindow&                       mWindow;
    Difficulty&                             mDifficulty;
    
    // GameWorld's own stuff:
    sf::View                                    mView;
    sf::View                                    mLeftView;
    sf::View                                    mRightView;
    sf::Vector2f                                mViewOffset;
    std::string                                 mStageName;
    sf::Vector2f                                mWorldSize;
    sf::Vector2f                                mScrollVelocity;
    ResourceHolder<cScript, std::string>        mScriptHolder;
    ResourceHolder<cShotScript, std::string>    mShotScriptHolder;
    ResourceHolder<cEntityHolder, std::string>  mEntityHolder;
    ResourceHolder<sf::Texture, std::string>    mEntityTextures;
    cSceneContainer                             mScene;
    sf::Vector2f                                mPlayer01StartPos;
    float                                       mPlayer01StartRot;
    cEntity*                                    mPlayer01;
    sf::Vector2f                                mScrTopLeft;
    cPlayer                                     mPlayer01Data;
    sf::FloatRect                               mWorldBounds;
    std::string                                 mPlayerScript;
    cBkGrid                                     mBkGrid;
    
    std::queue<SfxID>*                          pSoundQueue;

    // mStageMap is the queue in which stage map commands (events) are read.
    // mPosOnStage keeps track of where we are on the stage, how far we've advanced
    // from the starting position.
    std::queue<cStageEvents>                    mStageMap;
    unsigned long int                           mPosOnStage;
    unsigned long int                           mJumpTo;
        // When no enemies left on screen, let's jump to this position on map.

    sf::Time                                    mChargeSoundCoolDown;

    // The following is needed for the explosion sequence that is to follow the
    // final boss's death. This vector is set to some starting value (-10, -10)
    // in the gameworld's constructor. When it is changed by an entity, we know
    // that the Boss has actually died (as opposed to leaving the screen after
    // its time was up).
    sf::Vector2f                                mPlaceOfBossDeath;

};

#endif /* defined(__shmup_project__gameWorld__) */
