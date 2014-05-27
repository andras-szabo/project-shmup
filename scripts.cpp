#include "scripts.h"
#include <fstream>

//diag
#include <iostream>

bool cScript::loadFromFile(const std::string& filename)
{
    std::ifstream in { filename + ".scr" };

    std::string stmp;
    in >> stmp;             // scripts begin with their name
    mSteps = 0;
    
    in >> stmp;
    
    while ( stmp != "END" )
    {
        if ( stmp != "STEP" ) return false;
        ++mSteps;
        cInstruction instr;
        
        // first: time trigger
        
        float ftmp;
        in >> ftmp;
        instr.mTrigger = sf::seconds(ftmp);
        
        // then: type of command
        
        in >> stmp;
        if ( stmp == "accelerate" ) instr.mCommand.type = CommandType::accelerate;
        if ( stmp == "brown" ) instr.mCommand.type = CommandType::brown;
        if ( stmp == "do_nothing" ) instr.mCommand.type = CommandType::donothing;
        if ( stmp == "fire" ) instr.mCommand.type = CommandType::fire;   // fires primary gun
        if ( stmp == "fire_secondary" ) instr.mCommand.type = CommandType::fire_secondary;   // secondary gun
        if ( stmp == "forward" ) instr.mCommand.type = CommandType::forward; // accelerates in the dir it faces
        if ( stmp == "iddqd" ) instr.mCommand.type = CommandType::iddqd;
        if ( stmp == "move" ) instr.mCommand.type = CommandType::move;
        if ( stmp == "repeat_until" ) instr.mCommand.type = CommandType::repeat;
        if ( stmp == "rotate" ) instr.mCommand.type = CommandType::rotate;
        if ( stmp == "scale" ) instr.mCommand.type = CommandType::scale;
        if ( stmp == "scale_up" ) instr.mCommand.type = CommandType::scale_up;
        if ( stmp == "setpolarity" ) instr.mCommand.type = CommandType::setpolarity;
        if ( stmp == "slowdown" ) instr.mCommand.type = CommandType::slowdown;
        if ( stmp == "shoot" ) instr.mCommand.type = CommandType::shoot; // should not be used by entity scripts
        if ( stmp == "spawn" ) instr.mCommand.type = CommandType::spawn; // should not be used by entity scripts
        if ( stmp == "speed_up" ) instr.mCommand.type = CommandType::speed_up;
        if ( stmp == "track" ) instr.mCommand.type = CommandType::track;
        if ( stmp == "turn" ) instr.mCommand.type = CommandType::turn;
        if ( stmp == "rockets" ) instr.mCommand.type = CommandType::rocketrequest;
        if ( stmp == "selfdestruct" ) instr.mCommand.type = CommandType::selfdestruct;

        // Then parameters accordingly. As a placeholder, we set the Entity*
        // param to nullptr.
        instr.mCommand.p = nullptr;

        switch (instr.mCommand.type)
        {
            case ( CommandType::accelerate ):
            {
                in >> instr.mCommand.vel.x;
                in >> instr.mCommand.vel.y;
                break;
            }
            case ( CommandType::brown ):
            {
                break;
            }
            case ( CommandType::donothing ):
            {
                break;
            }
            case ( CommandType::fire ):
            {
                break;
            }
            case ( CommandType::fire_secondary ):
            {
                break;
            }
            case ( CommandType::forward ):
            {
                in >> instr.mCommand.rot;   // it's not actually rotation, but the point is that we need
                break;                      // a float in the union
            }
            case ( CommandType::iddqd ):
            {
                in >> stmp;
                instr.mCommand.boolean = (stmp == "on") ? true : false;
                break;
            }
            case ( CommandType::move ):
            {
                in >> instr.mCommand.vel.x >> instr.mCommand.vel.y;
                break;
            }
            case ( CommandType::repeat ):
            {
                in >> ftmp;
                instr.mCommand.timer = sf::seconds(ftmp);
                break;
            }
            case ( CommandType::rotate ):
            {
                in >> instr.mCommand.rot;
                break;
            }
            case ( CommandType::scale ):
            {
                in >> instr.mCommand.rot;
                break;
            }
            case ( CommandType::scale_up ):
            {
                in >> instr.mCommand.rot;
                break;
            }
            case ( CommandType::setpolarity ):
            {
                in >> stmp;
                if ( stmp == "light" ) instr.mCommand.polarity = EntityPolarity::light;
                if ( stmp == "dark" ) instr.mCommand.polarity = EntityPolarity::dark;
                if ( stmp == "alternate" ) instr.mCommand.polarity = EntityPolarity::alternate;
                if ( stmp == "neutral" ) instr.mCommand.polarity = EntityPolarity::neutral;
                break;
            }
            case ( CommandType::slowdown ):
            {
                in >> stmp;
                instr.mCommand.boolean = (stmp == "on") ? true : false;
                break;
            }
            case ( CommandType::speed_up ):
            {
                in >> instr.mCommand.rot;
                break;
            }
            case ( CommandType::track ):
            {
                in >> stmp;
                if ( stmp == "player1" ) instr.mCommand.tr.player = PlayerType::player1;
                if ( stmp == "player2" ) instr.mCommand.tr.player = PlayerType::player2;
                if ( stmp == "off" ) instr.mCommand.tr.player = PlayerType::none;
                else in >> instr.mCommand.tr.trackingspeed;
                break;
            }
            case ( CommandType::turn ):
            {
                in >> instr.mCommand.rot;
                break;
            }
            case ( CommandType::selfdestruct ):
                break;
            default: break;
        }
        mOrder.push_back(instr);
        in >> stmp;
    }
    in.close();
    return true;
}