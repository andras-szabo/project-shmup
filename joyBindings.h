#ifndef shmup_project_joyBindings_h
#define shmup_project_joyBindings_h

struct jBindings{
    sf::Uint8               ID;             // Which joystick/gamepad, if multiple present
    sf::Joystick::Axis      UpDownAxis;
    sf::Joystick::Axis      LeftRightAxis;
    sf::Uint8               Fire;
    sf::Uint8               Polarity;
    sf::Uint8               Rockets;
    sf::Uint8               UpButton;
    sf::Uint8               DownButton;
    sf::Uint8               LeftButton;
    sf::Uint8               RightButton;
    sf::Uint8               StartButton;
};


#endif
