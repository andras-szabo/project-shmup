#ifndef shmup_project_globalVariables_h
#define shmup_project_globalVariables_h

#include <SFML/Graphics.hpp>

enum class CommandType { accelerate, background, bossdeath,
                         brown, charge_meter, donothing, explode_spawn,
                         fire, fire_secondary, forward, iddqd,
                         increase_score, launch_rockets, mapjump,
                         mapjump_if, message,
                         move, playerdeath, relative_spawn,
                         repeat, rocketrequest,
                         rotate, scale, scale_up, selfdestruct,
                         setpolarity,
                         shoot, slowdown, spawn, speed_up,
                         stageclear, stageover, track, turn };

// mapjump_if: we jump if the final boss has escaped without having been defeated.
// relative_spawn: like spawn, but the spawn position is calculated relative to
//      mGameWorld.mPlaceOfBossDeath;

enum class ControlType { keyboard, gamepad };

enum class Difficulty { easy, medium, hard };

enum class EntityPolarity { light, dark, neutral, alternate };

enum class EntityType { player, background, ship,
                        playerbullet, enemybullet, bullet,
                        wall };

enum class FontID   { smallfont };

enum class MusicCommand { playMenu, playGame, pause, unpause };

enum class MusicID { menutheme, gametheme };

enum class PlayerType { none, player1, player2 };

enum class SfxID { menumove, menuselect, playershoot, rocketshoot, expl1, expl2, expl3, expl4, expl5, expl6,
                    selectorsound, pausemenu, message, charge, polarity, burst_slow, burst2, burst,
                    crossbeam, triangle, rosescatter, weakbeam, slowangle, spread, fivetimes4, unfolding,
                    big, beam, roger, playerspread };

enum class StatusID { onscreen, onbuffer, exploding, destroyed, spawning };

enum class TexID    { initscreen, menuscreen, fadescreen, GUItexture };

enum class TextStatus { entering, waiting, leaving };   // for message textboxes

enum class ShooterID { nobody, player1, player2 };

enum class SAID { push, pop, swap, return_to, clear, nothing };    // Stack Action ID.

const std::string   gVersion { "alpha 1.0" };

const sf::Vector2u  gViewSize { 600, 800 };     // logical size of view of actual playing field
const sf::Vector2u  gSideBarSize { 150, 800 };  // logical size of sidebars
const float         gDesFPS { 60.0 };           // desired FPS rate
const sf::Time      gTimePerFrame { sf::seconds(1.0 / gDesFPS) };
const float         gSlowFactor { 2.0 };
const int           gMaxEnemyCount { 100 };
const int           gBrown { 40 };
const int           gMaxTargetCount { 10 };
const int           gRocketDMG { 150 };
const float         gHitStun { 0.05 };
const int           gChargeRate { 30 };
const sf::Color     gCLight { 200, 200, 240 };
const sf::Color     gCDark { 255, 170, 170 };
const sf::Color     gCNeutral { 180, 180, 180 };
const sf::IntRect   gExplInt { -100, 200, 200, 200 };

// gExplInt: intervals for the starting velocity of bullets that are spawned
// when an entity explodes. the intervals are [from "left" to "width"; from "top" to "height"]

const float         gPlayerSpeed { 900.0 };
const float         gSQRTWO { 1.41 };
const sf::Vector2f  gTextVel { 0.0, 1800.0 };   // scroll speed for textboxes
const sf::Time      gSpawnTime { sf::seconds(4.0) };    // spawn-induced invulnerability for players

const sf::Vector2u  gGridSize(20, 20);      // Size of "background grid"
const size_t        gGridPointCount { 80 }; // (gGridSize.x + gGridSize.y) * 2
const float         gJoyThreshold { 8.0 };
const float         gMenuBlink { 0.4 };
const sf::IntRect   gArrowRect { 2, 6, 26, 20 };    // left arrow rect in gui texture file

const short int     gDefMusicVolume { 100 };
const short int     gDefSFXVolume { 100 };
const short int     gDefGamma { 0 };

const sf::Time      gSoundCoolDown { sf::seconds(0.08) };    // ? could be experimented with

const short int     gMaxSfxCopy { 1 };
const sf::Vector2f  gBossNotDead { -10, -10 };

static std::map<std::string, SfxID>  gSFXMap {
    {"playershot", SfxID::playershoot},
    {"5times4", SfxID::fivetimes4},
    {"beam", SfxID::beam},
    {"big", SfxID::big},
    {"bigrose", SfxID::rosescatter},
    {"burst", SfxID::burst2},
    {"burst2", SfxID::burst2},
    {"burst_slow", SfxID::burst2},
    {"crossbeam", SfxID::crossbeam},
    {"rosescatter", SfxID::rosescatter},
    {"slowangle", SfxID::slowangle},
    {"spread", SfxID::spread},
    {"triangle", SfxID::triangle},
    {"unfolding", SfxID::unfolding},
    {"weakbeam", SfxID::weakbeam},
    {"playerspread", SfxID::playershoot },
    {"last_one", SfxID::playershoot }
    };

// Volume settings for individual effects,
// set up after trial and error.

static std::map<SfxID, float> gMixer {
    { SfxID::playershoot, 1.5 },
    { SfxID::fivetimes4, 0.8 },
    { SfxID::beam, 0.4 },
    { SfxID::big, 1.0 },
    { SfxID::rosescatter, 0.75 },
    { SfxID::burst2, 1.0 },
    { SfxID::crossbeam, 0.5},
    { SfxID::slowangle, 0.75},
    { SfxID::spread, 0.6 },
    { SfxID::triangle, 1.0 },
    { SfxID::unfolding, 0.5 },
    { SfxID::weakbeam, 0.3 },
    { SfxID::expl1, 0.7 },
    { SfxID::expl2, 0.7 },
    { SfxID::expl3, 0.7 },
    { SfxID::expl4, 0.7 },
    { SfxID::expl5, 0.7 },
    { SfxID::expl6, 0.7 },
    { SfxID::menumove, 1.0 },
    { SfxID::menuselect, 1.0 },
    { SfxID::rocketshoot, 1.2 },
    { SfxID::selectorsound, 0.8 },
    { SfxID::pausemenu, 1.0 },
    { SfxID::message, 1.0 },
    { SfxID::charge, 1.0 },
    { SfxID::polarity, 0.5 },
    { SfxID::roger, 1.0 }
    };

#endif
