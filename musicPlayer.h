#ifndef __shmup_project__musicPlayer__
#define __shmup_project__musicPlayer__

#include "globalVariables.h"
#include <SFML/Audio.hpp>

class cMusicPlayer {
public:
    cMusicPlayer();
    
    void        play(MusicID);
    void        stop();
    void        setPaused(bool);
    void        setVolume(float);

private:
    sf::Music                       mMusic;
    std::map<MusicID, std::string>  mFileName;
    float                           mVolume;
};

#endif /* defined(__shmup_project__musicPlayer__) */
