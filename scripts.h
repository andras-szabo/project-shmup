#ifndef __shmup_project__scripts__
#define __shmup_project__scripts__

#include "commands.h"
#include <SFML/System/Time.hpp>

/* A Script is a vector of Instructions; and an Instruction is
 a series of sf::Time - Command pairs. */

struct cInstruction {
    cInstruction() { };
    sf::Time        mTrigger;
    Command         mCommand;
};

class cScript {
public:
    std::vector<cInstruction>   mOrder;
    int                         mSteps;
    
    bool                        loadFromFile(const std::string&);
};

#endif /* defined(__shmup_project__scripts__) */
