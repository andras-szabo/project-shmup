#ifndef __shmup_project__soundPlayer__
#define __shmup_project__soundPlayer__

#include <SFML/Audio.hpp>
#include "resourceHolders.h"
#include "globalVariables.h"
#include <list>

class cSoundPlayer {
public:
    cSoundPlayer();
    void                play(SfxID);
    void                removeStoppedSounds();
    void                setVolume(unsigned short int v) { mVolume = v; }
    unsigned short int  getVolume() const { return mVolume; }
    
private:
    ResourceHolder<sf::SoundBuffer, SfxID>  mSoundBuffers;
    std::list<sf::Sound>                    mSounds;
    unsigned short int                      mVolume;
};

#endif /* defined(__shmup_project__soundPlayer__) */
