#ifndef shmup_project_commands_h
#define shmup_project_commands_h

#include "globalVariables.h"
#include <queue>

class cEntity;  // forward declaration

// A Command is an in-game event that tells the world
// that some Entity got spawned, moved, rotated, shot,
// etc. Different kinds of commands are represented by
// a union, since every command executes only one thing.

// But some parts of Commands are NOT represented by the
// union, namely:
// Entity*      p: which entity is executing the command
// std::string  ScriptID: identifying scripts. Strings can't be
// std::string  EntityID: part of the union, so, yea, that's a bit of an overhead.

struct Command {
    
    Command(): boolean { false } { };
    
    struct shootparams {
        sf::Vector2f    vel;    // velocity needs to be rotated accordingly
        sf::Vector2f    pos;    // relative position from turret
        float           rot;    // relative rotation
    };
    
    struct spawnparams {
        sf::Vector2f    pos;
        sf::Vector2f    vel;
        float           rot;
        EntityType      type;
        EntityPolarity  polarity;
    };
    
    struct trackparams {
        PlayerType      player;
        float           trackingspeed;
    };
    
    struct explodeparams {
        sf::Vector2f    pos;
        EntityPolarity  polarity;
        unsigned int    count;
    };
    
    struct gradient {
        int             r,g,b, a;
    };

    CommandType         type;
    cEntity*            p;
    std::string         ScriptID;   // these will matter in case of shotscripts,
    std::string         EntityID;   // spawns, etc.
    bool                boolean;
    union {
        shootparams     sh;
        spawnparams     sp;
        trackparams     tr;
        explodeparams   ep;
        gradient        gr;

        sf::Color       col;        // for background-adjusting commands
        float           rot;
        sf::Vector2f    vel;
        EntityPolarity  polarity;
        sf::Time        timer;
        unsigned long int   chargeAmount;
        unsigned int        rocketCount;
    };
};

// StageEvents: this is how levels (stages) are scripted: when we reach StageEvent.distance,
// then we execute Command eventcommand.

struct cStageEvents {
    cStageEvents() { };
    unsigned long int       distance;
    Command                 eventcommand;
};

typedef std::queue<Command>     CQtype;

#endif
