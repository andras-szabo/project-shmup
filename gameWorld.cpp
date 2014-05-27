#include "gameWorld.h"
#include "ResourcePath.hpp"
#include "commands.h"
#include <fstream>

// If Engine is the Regional Manager, gameWorld is the assistant (to the) regional
// manager; the guy that makes everything run IN PRACTICE.
// In places you'll find traces of 2 player support here.

GameWorld::GameWorld(CQtype& cq, fHolder& f, tHolder& t,
                     sf::RenderWindow& w, Difficulty& d,
                     const std::string& s):
mCQ { cq },
mFontHolder { f },
mTextureHolder { t },
mWindow { w },
mDifficulty { d },
mView { mWindow.getDefaultView() },
mLeftView { mWindow.getDefaultView() },
mRightView { mWindow.getDefaultView() },
mStageName { s },
mScene { cq, w },
mPosOnStage { 0 },
mPlayer01Data { "Player01", mFontHolder.get(FontID::smallfont), 100 }, // 100 is just a placeholder,
mBulletsFired { 0 },
mEnemiesKilled { 0 },
mJumpTo { 0 },
mChargeSoundCoolDown { sf::seconds(0) },
mPlaceOfBossDeath { gBossNotDead },
mPolChgCount { 0 },
mLifeLost { false },
mBonusStage { false }
{
    // A lot is going to depend on the current stage, so let's
    // load that first. This will also set up player 1's starting
    // position and rotation; and script if any. (E.g. the tutorial
    // is a simple stage like any other, but the player entity
    // is following a pre-defined script. If there is a script defined,
    // then user inputs will be ignored, letting the script run its
    // course.)
    loadStage();
    
    // Setting up views. mView is the main view for action;
    // mLeftView is the left sidebar, mRightView is the right
    // sidebar.

    mView.setSize(gViewSize.x, gViewSize.y);
    mLeftView.setSize(gSideBarSize.x, gSideBarSize.y);
    mRightView.setSize(gSideBarSize.x, gSideBarSize.y);

    // Setting up viewports: which part(s) of the main window
    // are going to be used to display the views?
    
    float tViewLength = gViewSize.x + 2 * gSideBarSize.x;   // total logical view size

    // If the window is larger than the total play area, we need to display black
    // spaces around the play area, so as not to distort aspect ratios. That's what
    // happens here.
    
    sf::FloatRect flview, fmview, frview;
    
    auto ws = mWindow.getSize();
    if ( ws.x > tViewLength )
    {
        float diff = ws.x - tViewLength;
        flview.left = (diff / 2.0) / ws.x;
        flview.width = gSideBarSize.x / 1.0 / ws.x;
        
        float doff = ws.x - gViewSize.x;
        fmview.left = (doff / 2.0) / ws.x;
        fmview.width = gViewSize.x / 1.0 / ws.x;
        
        frview.left = (ws.x - (doff / 2.0)) / ws.x;
        frview.width = gSideBarSize.x / 1.0 / ws.x;
    }
    else if ( ws.x > ws.y )
    {
        float diff = (ws.x - ws.y) / 2.0;
        tViewLength += diff * 2;
        float pa = diff / tViewLength;
        float ps = (diff + gSideBarSize.x) / tViewLength;
        float pm = (diff + gSideBarSize.x + gViewSize.x) / tViewLength;
        
        flview.left = pa;
        fmview.left = ps;
        frview.left = pm;
        
        flview.width = ps - pa;
        fmview.width = pm - ps;
        frview.width = flview.width;
    }
    else
    {
        float sbarratio = gSideBarSize.x / 1.0 / tViewLength;
        float viewratio = gViewSize.x / 1.0 / tViewLength;
        flview.left = 0.0; flview.width = sbarratio;
        fmview.left = sbarratio; fmview.width = viewratio;
        frview.left = sbarratio + viewratio; frview.width = sbarratio;
    }
    
    if ( ws.y > gViewSize.y )
    {
        float diff = ws.y - gViewSize.y;
        float top = (diff / 2.0) / ws.y;
        float height = gViewSize.y / 1.0 / ws.y;
        
        flview.top = top;
        fmview.top = top;
        frview.top = top;
        
        flview.height = height;
        fmview.height = height;
        frview.height = height;
    }
    else
    {
        flview.top = 0.0; flview.height = 1.0;
        fmview.top = 0.0; fmview.height = 1.0;
        frview.top = 0.0; frview.height = 1.0;
    }
    
    mLeftView.setViewport(flview);
    mView.setViewport(fmview);
    mRightView.setViewport(frview);
    
    mView.setCenter(gViewSize.x / 2, mWorldSize.y - (gViewSize.y/2));
    mViewOffset.x = float (gViewSize.x / 2 ) * -1;
    mViewOffset.y = float (gViewSize.y / 2 ) * -1;
        
    mLeftView.setCenter(gSideBarSize.x / 2, gSideBarSize.y / 2);
    mRightView.setCenter(gSideBarSize.x / 2, gSideBarSize.y / 2);

    // Now we know how to translate local coords into global ones:
    // local.translate(mView.getCenter() + mViewOffset)
    
    mScrTopLeft = mView.getCenter() + mViewOffset;
    
    // Add player1, get pointer to it (mPlayer01) for further reference
    
    mPlayer01Data.mTopSpeed = gPlayerSpeed;
    mPlayer01Data.mTimer = sf::Time::Zero;
    mPlayer01Data.mCooldown = sf::seconds(0.08);   // main gun cooldown time
    mPlayer01Data.mSuperMeter = 0;
    
    std::unique_ptr<cEntity> p { new cEntity(mEntityHolder.get("player01"),
                                             EntityPolarity::light,
                                             mPlayer01StartPos,
                                             sf::Vector2f { 0.0, 0.0 },      // starting velocity 0:0
                                             mPlayer01StartRot,
                                             mCQ,
                                             mEntityTextures.get("tex"),
                                             mScriptHolder.get(mPlayerScript)) };
    mPlayer01 = p.get();
        
    mPlayer01Data.setHP(mPlayer01->mHP);
    mPlayer01Data.setSHP(mPlayer01->mHP);
        
    mScene.addEntity(std::move(p));
}

unsigned int GameWorld::getScore() const
{
    return mPlayer01Data.mScore;
}
        

// Helper function for loadStage()
bool ix(std::ifstream& in, const std::string& s)
{
    std::string stmp;
    in >> stmp;
    return stmp == s;
}


void GameWorld::loadStage()
{
    std::ifstream in { resourcePath() + mStageName + ".def" };
    std::string stmp = " ";

    // First: load world info: size and scroll velocities
    in >> stmp; if ( stmp != "WORLD" ) throw std::runtime_error("World definition missing.");
    in >> mWorldSize.x >> mWorldSize.y;
    in >> mScrollVelocity.x >> mScrollVelocity.y;
    mWorldBounds = sf::FloatRect { 0, 0, mWorldSize.x, mWorldSize.y };
    
    // Then: player starting pos, rot
    in >> stmp;
    if ( stmp != "PLAYER" )
        throw std::runtime_error("Player starting def missing. " + stmp);
    in >> mPlayerScript;
    mPlayerActive = mPlayerScript == "noscript" ? true : false;
    in >> mPlayer01StartPos.x >> mPlayer01StartPos.y;
    in >> mPlayer01StartRot;

    // Then: load scripts
    in >> stmp; if ( stmp != "SCRIPTS") throw std::runtime_error("No scripts defined.");
    in >> stmp;
    while ( stmp != "END" )
    {
        mScriptHolder.load(stmp, stmp);         // scripts will simply be identified by their names.
        in >> stmp;
    }
    
    // Then: load shot scripts
    
    in >> stmp; if ( stmp != "SHOTSCRIPTS" ) throw std::runtime_error("No shotscripts defined.");
    in >> stmp;
    while ( stmp != "END" )
    {
        mShotScriptHolder.load(stmp, stmp);
        in >> stmp;
    }
    
    // Then: load textures
    in >> stmp; if ( stmp != "TEXTURES" ) throw std::runtime_error("No textures defined.");
    in >> stmp;
    while ( stmp != "END" )
    {
        mEntityTextures.load(stmp, stmp + ".png");
        in >> stmp;
    }
    
    // Then: load entities - i.e. EntityInfo structs that serve as templates
    // for Entity creation
    
    in >> stmp; if ( stmp != "ENTITIES" ) throw std::runtime_error("No entities defined.");
    in >> stmp;
    while ( stmp != "END" )
    {
        mEntityHolder.load(stmp, stmp);
        in >> stmp;
    }
    
    // And now: background definition
    if ( !ix(in, "BACKGROUND") ) throw std::runtime_error("No background defined.");
    if ( !ix(in, "VEL" ) ) throw std::runtime_error("No bkg velocity defined.");
    sf::Vector2f vtmp;
    in >> vtmp.x >> vtmp.y;
    mBkGrid.setVelocity(vtmp);
    
    if ( !ix(in, "COL") ) throw std::runtime_error("No bkg starting colour defined.");
    sf::Color ctmp;
    int t;
    in >> t; ctmp.r = t;
    in >> t; ctmp.g = t;
    in >> t; ctmp.b = t;
    in >> t; ctmp.a = t;
    mBkGrid.setColor(ctmp);
    
    if ( !ix(in, "ROT") ) throw std::runtime_error("No bkg starting rotation defined.");
    float ftmp;
    in >> ftmp;
    mBkGrid.setRotate(ftmp);
    if ( !ix(in, "SPI") ) throw std::runtime_error("No bkg spin defined.");
    in >> ftmp;
    mBkGrid.setSpin(ftmp);
    if ( !ix(in, "FAD") ) throw std::runtime_error("No bkg fade time defined.");
    in >> ftmp;
    mBkGrid.setFadeTime(sf::seconds(ftmp));
    if ( !ix(in, "GRA") ) throw std::runtime_error("No bkg gradient defined.");
    
    int u, v, w;
    
    in >> t;
    in >> u;
    in >> v;
    in >> w;
    
    mBkGrid.setGradient(t, u, v, w);
    

    in.close();
    
    // Finally, let's load the stage map:
    // events assigned to a particular distance from the start.
    
    in = std::ifstream { resourcePath() + mStageName + ".map" };
    
    // Then read the sequence of distance - StageEvents pairs
    
    // For testing: this will look for a START label, and - start from there.
    // After the START label stands a distance indicator - that's what we should
    // understand as the new 0. => if its value is the same as that of the first
    // expression, than things will start at 0.0!
    
    stmp = "";
    while ( stmp != "START" ) in >> stmp;
    float offset; in >> offset;
    mPosOnStage = offset;
    
    // Now then.
    
    double  dtmp;       // distance, temporary variable
                        // Distance can be specified in 2 ways: using positive or negative numbers.
                        // Positive ones are read and interpreted just so; negative ones
                        // are interpreted as relative to the previous position. When the map file
                        // is read, negative ones are converted into positive ones as well.
                        // So the following two snippets are equivalent:
    
    // 2500 message Blahblah END
    // 2600 message Blahblahblah END
    // 2700 spawn dark ship something ... etc.
    
    // 2500 message Blahblah END
    // -100 message Blahblahblah END
    // 2700 spawn dark ship something ... etc.
    
    // So relative and absolute distance notation can be freely combined.
    
    // dtmp actually should be a long int, as i'm not using fractions to specify
    // events in stage maps. but i used to, and set up the maps accordingly.
    //
    // so this is where some "refactoring" (removing the .0s) is due.
    
    unsigned long int prev_distance = offset;
    cStageEvents s;
    while ( in >> dtmp )
    {
        if ( dtmp <= 0.0 )
        {
            dtmp *= -1;
            dtmp += prev_distance;
        }
        prev_distance = dtmp;
        s.distance = dtmp;
        in >> stmp;
        if ( stmp == "spawn" ) s.eventcommand.type = CommandType::spawn;
        else if ( stmp == "rel_spawn" ) s.eventcommand.type = CommandType::relative_spawn;
        else if ( stmp == "message" ) s.eventcommand.type = CommandType::message;
        else if ( stmp == "background" ) s.eventcommand.type = CommandType::background;
        else if ( stmp == "STAGE_OVER") s.eventcommand.type = CommandType::stageover;
        else if ( stmp == "STAGE_CLEAR") s.eventcommand.type = CommandType::stageclear;
        else if ( stmp == "JUMP_TO" ) s.eventcommand.type = CommandType::mapjump;
        else if ( stmp == "JUMP_IF_NOT_DEAD" ) s.eventcommand.type = CommandType::mapjump_if;
        // ... other kinds of commands
        
        // Here: switch-case re: event-type
        if ( s.eventcommand.type == CommandType::spawn || s.eventcommand.type == CommandType::relative_spawn )
        {
            in >> stmp;
            if ( stmp == "dark" ) s.eventcommand.sp.polarity = EntityPolarity::dark;
            if ( stmp == "light" ) s.eventcommand.sp.polarity = EntityPolarity::light;
            if ( stmp == "neutral") s.eventcommand.sp.polarity = EntityPolarity::neutral;
            if ( stmp == "invisible" )
                {
                    s.eventcommand.sp.polarity = EntityPolarity::neutral;
                    s.eventcommand.boolean = true;    // here the boolean signals invisibility
                }
            
            in >> stmp;
            if ( stmp == "ship" ) s.eventcommand.sp.type = EntityType::ship;
            if ( stmp == "bullet" ) s.eventcommand.sp.type = EntityType::bullet;
            if ( stmp == "player" ) s.eventcommand.sp.type = EntityType::player;
            if ( stmp == "wall" ) s.eventcommand.sp.type = EntityType::wall;
            
            in >> s.eventcommand.EntityID;
            sf::Vector2f vtmp;
            in >> vtmp.x >> vtmp.y;
            s.eventcommand.sp.pos = vtmp;
            in >> vtmp.x >> vtmp.y;
            s.eventcommand.sp.vel = vtmp;
            in >> dtmp;
            s.eventcommand.sp.rot = dtmp;
            
            in >> s.eventcommand.ScriptID;
        }
        else if ( s.eventcommand.type == CommandType::message )
        {
            in >> stmp;
            s.eventcommand.ScriptID = "";
            while ( stmp != "END" )
            {
                if ( stmp == "_" ) stmp = " ";
                if ( stmp == "__" ) stmp = "\n";
                s.eventcommand.ScriptID += stmp;
                in >> stmp;
            }
        }
        else if ( s.eventcommand.type == CommandType::background )
        {
            in >> s.eventcommand.ScriptID; // "setVEL," "adjSPI", etc
            if ( s.eventcommand.ScriptID == "setVEL" || s.eventcommand.ScriptID == "adjVEL" )
                in >> s.eventcommand.vel.x >> s.eventcommand.vel.y;
            else if ( s.eventcommand.ScriptID == "setSPI" || s.eventcommand.ScriptID == "adjSPI" ||
                      s.eventcommand.ScriptID == "setFAD" || s.eventcommand.ScriptID == "setROT" )
                in >> s.eventcommand.rot;
            else if ( s.eventcommand.ScriptID == "setCOL" )
                {
                    in >> t; s.eventcommand.col.r = t;
                    in >> t; s.eventcommand.col.g = t;
                    in >> t; s.eventcommand.col.b = t;
                    in >> t; s.eventcommand.col.a = t;
                }
            else if ( s.eventcommand.ScriptID == "setGRA" )
            {
                in >> s.eventcommand.gr.r;
                in >> s.eventcommand.gr.g;
                in >> s.eventcommand.gr.b;
                in >> s.eventcommand.gr.a;
            }
        }
        else if ( s.eventcommand.type == CommandType::mapjump || s.eventcommand.type == CommandType::mapjump_if)
            in >> s.eventcommand.chargeAmount;
            // ChargeAmount will serve as a holder of the jump target position.
        
        // ... dealing with other kinds of events ... ///
        mStageMap.push(s);
    }
    in.close();
    std::cout << "Stagemap loaded\n";
}

sf::Transform& GameWorld::getViewTransform() const
{
    sf::RenderStates tmp;
    return tmp.transform.translate(mScrTopLeft);
}

void GameWorld::render()
{
    // First thing: render the scene on the main view
    mWindow.setView(mView);
    
    // First we draw the background.
    mWindow.draw(mBkGrid, getViewTransform());
    
    // We need to translate local coordinates to global ones.
    // The way to do that is to pass around parameters of
    // where the view is set right now (mView.getCenter() + mViewOffset).
    mScene.render(mWindow, getViewTransform());
    
    // Then player data on right/left view
    mWindow.setView(mLeftView);
    mPlayer01Data.render(mWindow, sf::RenderStates::Default);
    
    mWindow.setView(mRightView);
    // Here we'd render player 02's data. We don't;
    // but the FPS counter will come here.
}


void GameWorld::update(sf::Time dt)
{
    // First we evaluate commands in the master command queue.
    // In each update, the command queue is emptied.
    while ( !mCQ.empty() )
    {
        Command c = mCQ.front();
        mCQ.pop();
        switch ( c.type )
        {
            case CommandType::bossdeath:    // announcing that the final boss has been defeated
            {
                mPlaceOfBossDeath = c.vel;
                break;
            }
            case CommandType::mapjump_if:   // we jump if the final boss HAS NOT been defeated
            {
                if ( mPlaceOfBossDeath == gBossNotDead )
                    mJumpTo = c.chargeAmount;
                break;
            }
            case CommandType::mapjump:
            {
                mJumpTo = c.chargeAmount;
                break;
            }
            case CommandType::background:
            {
                if ( c.ScriptID == "setVEL" ) { mBkGrid.setVelocity(c.vel); break; }
                if ( c.ScriptID == "adjVEL" ) { mBkGrid.adjustVelocity(c.vel); break; }
                if ( c.ScriptID == "setSPI" ) { mBkGrid.setSpin(c.rot); break; }
                if ( c.ScriptID == "adjSPI" ) { mBkGrid.adjustSpin(c.rot); break; }
                if ( c.ScriptID == "setFAD" ) { mBkGrid.setFadeTime(sf::seconds(c.rot)); break; }
                if ( c.ScriptID == "setCOL" ) { mBkGrid.setColor(c.col); break; }
                if ( c.ScriptID == "setGRA" ) { mBkGrid.setGradient(c.gr.r, c.gr.g, c.gr.b, c.gr.a); break; }
                if ( c.ScriptID == "setROT" ) { mBkGrid.setRotate(c.rot); break; }
                break;
            }
            case CommandType::playerdeath:
            {
                if ( c.p == mPlayer01 ) // Player 1 lost a life!
                {
                    // We need to replace it, so as not to leave mPlayer01 hanging,
                    // pointing to who knows where.
                    c.p->mStatus = StatusID::destroyed;
                    
                    std::unique_ptr<cEntity> p { new cEntity(mEntityHolder.get("player01"),
                                                             EntityPolarity::light,
                                                             mPlayer01StartPos,
                                                             sf::Vector2f { 0.0, 0.0 },      // starting velocity 0:0
                                                             mPlayer01StartRot,
                                                             mCQ,
                                                             mEntityTextures.get("tex"),
                                                             mScriptHolder.get(mPlayerScript)) };
                    mPlayer01 = p.get();
                    
                    // We also need to update all those Entities who (were) tracking
                    // poor player one.
                    
                    mScene.updateTracking(c.p, mPlayer01);  // old pointer, new pointer.
                    
                    // But we don't actually add the new player to the scene if there
                    // are no lives left.
                    
                    mLifeLost = true;
                    
                    if ( mPlayer01Data.mLivesLeft ) // ... player has some more lives!
                    {
                        --mPlayer01Data.mLivesLeft;
                        mPlayer01Data.mSuperMeter = 0;
                        mPlayer01Data.mHP = mPlayer01Data.mStartingHP;
                        mScene.addEntity(std::move(p));
                        break;
                    }
                    else    // Game Over!
                    {
                        msgQueue.push("_GAMEOVER_");
                        break;
                    }
                }
                break;
            }
            case CommandType::rocketrequest:
            {
                // based on c.p, we can distinguish between
                // p1 and p2
                playerRockets();
                break;
            }
            case CommandType::message:
            {
                msgQueue.push(c.ScriptID);
                break;
            }
            case CommandType::stageover:
            {
                msgQueue.push("_STAGEOVER_");
                break;
            }
            case CommandType::stageclear:
            {
                msgQueue.push("_STAGECLEAR_");
                break;
            }
            case CommandType::increase_score:
            {
                if ( c.ScriptID == "player01" )
                {
                    switch (mDifficulty) {
                        case Difficulty::easy: c.chargeAmount /= 1.5; break;
                        case Difficulty::hard: c.chargeAmount *= 2.2; break;
                    }
                    mPlayer01Data.mScore += c.chargeAmount;
                    ++mEnemiesKilled;
                }
                break;
            }
            case CommandType::fire:                 // a "fire" command translates into 1 or more "shoot"
            {                                       // commands that themselves get their parameters from
                Command c_new;                      // the appropriate shooting script (.ssp)
                c_new.type = CommandType::shoot;
                c_new.p = c.p;
                cShotScript shotScript = mShotScriptHolder.get(c.p->mShotScript);
                auto end = shotScript.mOrder.end();
                for ( auto it = shotScript.mOrder.begin(); it != end; ++it )
                {
                    c_new.ScriptID = it->script;
                    c_new.EntityID = it->id;
                    c_new.sh.pos = it->pos;
                    c_new.sh.vel = it->vel;
                    c_new.sh.rot = it->rot;
                    mCQ.push(c_new);
                }
                
                // Depending on the shotscript, we play appropriate sound effect,
                // if the entity's "sound cool down" is reached (oder?)

                // For now, a global sound cool down value is used; later
                // this could be regulated through shotscripts
                if ( c.p->mSoundTimer <= sf::Time::Zero )
                {
                    pSoundQueue->push(shotScript.mSFX);
                    c.p->mSoundTimer = gSoundCoolDown;
                }
                break;
            }
            case CommandType::fire_secondary:   // fire alternate gun
            {
                Command c_new;
                c_new.type = CommandType::shoot;
                c_new.p = c.p;
                cShotScript shotScript = mShotScriptHolder.get(c.p->mSecondShotScript);
                auto end = shotScript.mOrder.end();
                for ( auto it = shotScript.mOrder.begin(); it != end; ++it)
                {
                    c_new.ScriptID = it->script;
                    c_new.EntityID = it->id;
                    c_new.sh.pos = it->pos;
                    c_new.sh.vel = it->vel;
                    c_new.sh.rot = it->rot;
                    mCQ.push(c_new);
                }
                // Depending on the shotscript, we play appropriate sound effect,
                // if the entity's "sound cool down" is reached (oder?)
                
                // For now, a global sound cool down value is used; later
                // this could be regulated through shotscripts
                if ( c.p->mSoundTimer <= sf::Time::Zero )
                {
                    pSoundQueue->push(shotScript.mSFX);
                    c.p->mSoundTimer = gSoundCoolDown;
                }
                break;
            }
            case CommandType::brown:
            {
                sf::Vector2f vtmp { static_cast<float>(rand() % gBrown - (gBrown / 2)), 0.0 };
                c.p->adjustVelocity(vtmp);
                break;
            }
            case CommandType::shoot :
            {
                // First calculate the turret's position from the parent Entity's data
                if ( (c.p != nullptr) && (c.p->mStatus != StatusID::destroyed) )
                {
                    sf::Vector2f    bpos { c.p->getTurretPosition(c.sh.pos) };
                    EntityPolarity  bpol { c.p->mPolarity };
                    float           brot { c.p->getRotation() };
                    
                    sf::Transform   tmp;
                    tmp.rotate(brot + c.sh.rot);
                    
                    sf::Vector2f    bvel { tmp.transformPoint(c.sh.vel) };
                    
                    if (c.p->mType != EntityType::player)
                        bvel += c.p->getVelocity();
                    
                    // now we have bullet's position, polarity, velocity and rotation
                    // speed all covered, so we can actualyl spawn it. for that,
                    // we'll also need a texture id (supplied by a cEntityHolder);
                    // and we'll also want to know if this bullet was shot by a
                    // player or by an enemy.

                    std::string texid = mEntityHolder.get(c.EntityID).mTextureFile;
                    ShooterID shooter = ShooterID::nobody;
                    if (c.p == mPlayer01) shooter = ShooterID::player1;
                    std::unique_ptr<cEntity> p { new cEntity(mEntityHolder.get(c.EntityID),
                                                             bpol,
                                                             bpos,
                                                             bvel,
                                                             brot,
                                                             mCQ,
                                                             mEntityTextures.get(texid),
                                                             mScriptHolder.get(c.ScriptID),
                                                             shooter) };
                    if ( c.p->mType == EntityType::player ) p->mType = EntityType::playerbullet;
                    else p->mType = EntityType::enemybullet;
                    p->rotate(c.sh.rot);
                    mScene.addEntity(std::move(p));
                }
                break;
            }
            case CommandType::move:
            {
                c.p->setVelocity(c.vel);
                break;
            }
            case CommandType::accelerate:
            {
                c.p->adjustVelocity(c.vel);
                break;
            }
            case CommandType::turn:
            {
                c.p->turn(c.rot);
                break;
            }
            case CommandType::scale:
            {
                c.p->setScaleFactor(c.rot);
                break;
            }
            case CommandType::scale_up:
            {
                c.p->adjustScaleFactor(c.rot);
                break;
            }
            case CommandType::speed_up:
            {
                c.p->speedup(c.rot);
                break;
            }
            case CommandType::rotate:
            {
                c.p->setRotate(c.rot);
                break;
            }
            case CommandType::iddqd:
            {
                c.p->setIDDQD(c.boolean);
                break;
            }
            case CommandType::spawn:
            {
                std::string texid = mEntityHolder.get(c.EntityID).mTextureFile;
                std::unique_ptr<cEntity> p { new cEntity(mEntityHolder.get(c.EntityID),
                                                         c.sp.polarity,
                                                         c.sp.pos,
                                                         c.sp.vel,
                                                         c.sp.rot,
                                                         mCQ,
                                                         mEntityTextures.get(texid),
                                                         mScriptHolder.get(c.ScriptID)) };

                // p->mType = c.sp.type;       //  <- legacy, no longer needed
                
                if (c.sp.polarity == EntityPolarity::neutral && c.boolean == true ) // invisible!
                    p->setInvisible(true);
                
                mScene.addEntity(std::move(p));
                break;
            }
            case CommandType::relative_spawn:
                // This is used for spawning exploding little objects when the final boss
                // has been defeated. Spawn positions are thus relative to mPlaceOfBossDeath.
            {
                Command c_new { c };
                c_new.type = CommandType::spawn;
                c_new.sp.pos += mPlaceOfBossDeath;
                mCQ.push(c_new);
                break;
            }
            case CommandType::explode_spawn:
                // exploding entities might spawn bullets ( depending on difficulty level )
                // AND at any rate they spawn - an explosion sound effect! haha!
                // I have 6 different kinds of explosion effects, let's pick one at
                // random. Rand() % 6 doesn't _actually_ generate uniformly random
                // numbers, but that's really beside the point here.
            {
                auto r = rand() % 6 + 1;
                switch (r){
                    case 1: pSoundQueue->push(SfxID::expl1); break;
                    case 2: pSoundQueue->push(SfxID::expl2); break;
                    case 3: pSoundQueue->push(SfxID::expl3); break;
                    case 4: pSoundQueue->push(SfxID::expl4); break;
                    case 5: pSoundQueue->push(SfxID::expl5); break;
                    case 6: pSoundQueue->push(SfxID::expl6); break;
                }
                
                
                if ( mDifficulty == Difficulty::easy ) break;
                if ( (mDifficulty == Difficulty::medium) && (c.ep.polarity != mPlayer01->mPolarity) ) break;
                for ( auto i = 0; i < c.ep.count; ++i )
                {
                    sf::Vector2f bvel { static_cast<float>(rand() % gExplInt.width + gExplInt.left),
                                        static_cast<float>(rand() % gExplInt.height + gExplInt.top) };
                    std::string texid = mEntityHolder.get("explosionbullet").mTextureFile;
                    std::unique_ptr<cEntity> p { new cEntity(mEntityHolder.get("explosionbullet"),
                                                             c.ep.polarity,
                                                             c.ep.pos,
                                                             bvel,
                                                             0.0,
                                                             mCQ,
                                                             mEntityTextures.get(texid),
                                                             mScriptHolder.get("brownscript")) };
                    p->mType = EntityType::enemybullet;
                    p->setRotate(270.0);    // make explosion bullets nicely spin around
                    mScene.addEntity(std::move(p));
                }
                break;
            }
            case CommandType::setpolarity:
            {
                c.p->switchPolarity(c.polarity);
                pSoundQueue->push(SfxID::polarity);
                break;
            }
            case CommandType::slowdown:
            {
                mScene.mSlowdown = c.boolean;
                break;
            }
            case CommandType::charge_meter:
            {
                // So far this lacks 2 player support
                auto maxcharge = gMaxTargetCount * 10;
                if (mPlayer01Data.mSuperMeter < maxcharge) mPlayer01Data.mSuperMeter += c.chargeAmount;
                if (mPlayer01Data.mSuperMeter > maxcharge) mPlayer01Data.mSuperMeter = maxcharge;
                if ( mChargeSoundCoolDown <= sf::Time::Zero )
                {
                    pSoundQueue->push(SfxID::charge);
                    mChargeSoundCoolDown = gSoundCoolDown;
                }
                break;
            }
            case CommandType::launch_rockets:
            {
                // Forward launch calls to mScene.
                // This also lacks 2 player support so far.
                if ( mScene.launchRockets(mPlayer01->getPosition(),
                                          c.rocketCount,
                                          mPlayer01->mPolarity) )
                {
                    mPlayer01Data.mSuperMeter = 0;
                    pSoundQueue->push(SfxID::rocketshoot);
                }
                break;
            }
            case CommandType::track:
            {
                if ( c.tr.player == PlayerType::none ) c.p->setTracking(nullptr, 0.0);
                if ( c.tr.player == PlayerType::player1 ) c.p->setTracking(mPlayer01, c.tr.trackingspeed);
                // if ( c.tr.player == PlayerType::player2 ) c.p->setTracking(mPlayer02, c.tr.trackingspeed);
                break;
            }
            default: break;
        }
    }

    // mScene.update() returns the number of managed entities on scene.
    // When there are no entities apart from the player(s), scrolling is
    // sped up a little bit so that we don't have to wait so much for
    // the appearance of the next enemy.
    

    // Taking possible bullet-timing into consideration:
    if ( mScene.mSlowdown ) dt /= gSlowFactor;
    mPlayer01Data.mTimer -= dt;
    
    auto mNumEntities = mScene.update(dt);   // This updates the whole scene. Ta-dah.

    // Update background
    mBkGrid.update(dt);
    
    // Update both the view and the position tracker (where we are on the stage,
    // by measuring how much we moved ( scrollvelocity * delta t )
    
    sf::Vector2f updatevector { mScrollVelocity * dt.asSeconds() };
    mView.move(updatevector);
    
    // This is also where we speed things up: if the number of managed
    // non-player entities is 0, then we jump to the next relevant
    // position on the map (at least in case that such a valid jump-to
    // position is actually defined).
    
    // Jumping also means ignoring all map statements until reaching the
    // target of the jump. So those statements in-between are popped
    // off mStageMap.
    
    // This is done so that the player doesn't have to wait long empty stretches,
    // in case she manages to destroy enemies quickly.
    
    auto inc = (abs(updatevector.x) + abs(updatevector.y));
    mPosOnStage += inc;
    
    if ( mNumEntities == 0 && mJumpTo > mPosOnStage )
    {
        mPosOnStage = mJumpTo;
        while ( !mStageMap.empty() && mPosOnStage > mStageMap.front().distance )
            mStageMap.pop();
    }
    
    // Let's advance score just by moving forward.
    mPlayer01Data.mScore += inc / 8;
    
    // Update HP &c
    mPlayer01Data.setHP(mPlayer01->mHP);
    
    // It was found that mScrTopLeft cannot simply be updated by the same updatevector
    // as the View, because of floating point imprecision. It's better to recalculate
    // it in each update.
    
    mScrTopLeft = mView.getCenter() + mViewOffset;
    
    // Theoretical: if we reach the world's end, we turn around.
    
    if ((mScrTopLeft.x < mWorldBounds.left) ||
        (mScrTopLeft.x > mWorldBounds.width)) mScrollVelocity.x *= -1;
    if ((mScrTopLeft.y < mWorldBounds.top) ||
        (mScrTopLeft.y > mWorldBounds.height - gViewSize.y)) mScrollVelocity.y *= -1;
    
    // Finally, on the basis of where we are on the stage, we push commands onto the queue
    while ( !mStageMap.empty() && (mPosOnStage >= mStageMap.front().distance) )
    {
        cStageEvents e = mStageMap.front();
        mCQ.push(e.eventcommand);
        mStageMap.pop();
    }
    
    // Update charge sound cooldown
    // (the meter charge has a special sound effect, in that it can be played
    // at a higher rate than other effects; because this sounds better.)
    if ( mChargeSoundCoolDown > sf::Time::Zero )
        mChargeSoundCoolDown -= dt;
}

void GameWorld::playerShoot(bool secondary)
{
    
    // This gets called when the player pushes the shoot button.
    // if the player entity is in the state of being able to actually shoot,
    // and its gun cooldown (measured by mPlayer01Data.mTimer) has expired,
    // then it pushes an appropriate command onto the main command queue.
    
    if (!mPlayerActive || mPlayer01->mStatus==StatusID::exploding) return;
    if (mPlayer01Data.mTimer <= sf::Time::Zero)
    {
        Command c { };
        if (secondary == false && mBonusStage == false) c.type = CommandType::fire;
        else c.type = CommandType::fire_secondary;
        c.p = mPlayer01;
        mCQ.push(c);
        mPlayer01Data.mTimer = mPlayer01Data.mCooldown;
        mBulletsFired += secondary ? 11 : 2;
    }
}

void GameWorld::playerMove(bool* dbuttons)
{
    if (!mPlayerActive || mPlayer01->mStatus==StatusID::exploding) return;
    if (mPlayer01->mHitStun > sf::seconds(0)) return;
    Command c;
    c.type = CommandType::move;
    c.p = mPlayer01;
    
    c.vel.x = -mPlayer01Data.mTopSpeed * dbuttons[0];  // left
    c.vel.x += mPlayer01Data.mTopSpeed * dbuttons[2];  // right
    
    c.vel.y = -mPlayer01Data.mTopSpeed * dbuttons[1];  // up
    c.vel.y += mPlayer01Data.mTopSpeed * dbuttons[3];  // down
    
    // Cut diagonal velocities
    
    if ((dbuttons[0] || dbuttons[2]) && (dbuttons[1] || dbuttons[3]))
    {
        c.vel.x /= gSQRTWO;       // as defined in GlobalVariables
        c.vel.y /= gSQRTWO;       // as defined in GlobalVariables
    }
    
    // If the "slow me down!" button is pressed too, then - slow down
    // if (dbuttons[4]) { c.mo.vel.x /= 3.0; c.mo.vel.y /= 3.0; }

    // If new velocity differs from the old one, we push it onto the
    // command queue, otherwise why bother.
    
    if (c.vel != mPlayer01->getVelocity())
        mCQ.push(c);
}
        
// UserInput: if Accelerate command follows user input, we need to
// handle it as a percentage value of mPlayerData.mTopSpeed.
// Otherwise we just take the acceleration vector as it is.
// userInput = true is the default setting.
void GameWorld::playerAccelerate(float mx, float my, bool userInput)
{
    if ((mx != 0.0) || (my != 0.0))
    {
        if (userInput)
        {
            mx = mx / 100.0 * mPlayer01Data.mTopSpeed;
            my = my / 100.0 * mPlayer01Data.mTopSpeed;
        }
        Command c;
        c.type = CommandType::accelerate;
        c.p = mPlayer01;
        c.vel.x = mx;
        c.vel.y = my;
        mCQ.push(c);
    }
}

void GameWorld::playerSwitch()
{
    // Switching polarities.
    
    if (!mPlayerActive || mPlayer01->mStatus==StatusID::exploding) return;
    mPlayer01->switchPolarity(EntityPolarity::alternate);
    pSoundQueue->push(SfxID::polarity);
    ++mPolChgCount;
}

void GameWorld::playerRockets()
{
    if (mPlayer01->mStatus==StatusID::exploding) return;
    if (mPlayer01Data.mSuperMeter >= 10)
    {
        Command c;
        c.type = CommandType::launch_rockets;
        c.rocketCount = mPlayer01Data.mSuperMeter / 10;
        mCQ.push(c);
    }
}

void GameWorld::slowdown()
{
    mScene.mSlowdown = !mScene.mSlowdown;
}