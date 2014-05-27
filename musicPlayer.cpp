#include "musicPlayer.h"
#include "ResourcePath.hpp"

cMusicPlayer::cMusicPlayer():
mMusic { },
mFileName { },
mVolume { 100.f }
{
    mFileName[MusicID::menutheme] = "menutheme.ogg";
    mFileName[MusicID::gametheme] = "gametheme.ogg";
}

void cMusicPlayer::play(MusicID id)
{
    std::string filename = mFileName[id];
    
    if (!mMusic.openFromFile(resourcePath() + filename))
        throw std::runtime_error("Music " + filename + " load error.");
    
    mMusic.setVolume(mVolume * 0.3);
    mMusic.setLoop(true);
    mMusic.play();
}

void cMusicPlayer::stop()
{
    mMusic.stop();
}

void cMusicPlayer::setPaused(bool p)
{
    if (p) mMusic.pause();
    else mMusic.play();
}

void cMusicPlayer::setVolume(float f)
{
    mVolume = f;
    mMusic.setVolume(mVolume * 0.3);
}