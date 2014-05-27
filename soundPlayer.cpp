#include "soundPlayer.h"

// Soundplayer constructor: loading all the sound
// effects!
cSoundPlayer::cSoundPlayer():
mSoundBuffers { },
mSounds { },
mVolume { 100 }
{
    mSoundBuffers.load(SfxID::menumove, "menumove.wav");
    mSoundBuffers.load(SfxID::menuselect, "menuselect.wav");
    mSoundBuffers.load(SfxID::selectorsound, "selectorsound.wav");
    mSoundBuffers.load(SfxID::playershoot, "playershoot.wav");
    mSoundBuffers.load(SfxID::rocketshoot, "rocketshoot.wav");
    mSoundBuffers.load(SfxID::expl1, "explosion1.wav" );
    mSoundBuffers.load(SfxID::expl2, "explosion2.wav" );
    mSoundBuffers.load(SfxID::expl3, "explosion3.wav" );
    mSoundBuffers.load(SfxID::expl4, "explosion4.wav" );
    mSoundBuffers.load(SfxID::expl5, "explosion5.wav" );
    mSoundBuffers.load(SfxID::expl6, "explosion6.wav" );
    mSoundBuffers.load(SfxID::pausemenu, "pausemenu.wav" );
    mSoundBuffers.load(SfxID::message, "msg dismissed.wav" );
    mSoundBuffers.load(SfxID::charge, "charge.wav" );
    mSoundBuffers.load(SfxID::polarity, "polarity.wav" );
    mSoundBuffers.load(SfxID::burst_slow, "burst_slow.wav" );
    mSoundBuffers.load(SfxID::burst2, "burst2.wav" );
    mSoundBuffers.load(SfxID::burst, "burst.wav" );
    mSoundBuffers.load(SfxID::crossbeam, "crossbeam.wav" );
    mSoundBuffers.load(SfxID::triangle, "triangle.wav" );
    mSoundBuffers.load(SfxID::rosescatter, "rosescatter.wav");
    mSoundBuffers.load(SfxID::weakbeam, "weakbeam.wav" );
    mSoundBuffers.load(SfxID::slowangle, "slowangle.wav" );
    mSoundBuffers.load(SfxID::spread, "spread.wav" );
    mSoundBuffers.load(SfxID::fivetimes4, "5times4.wav" );
    mSoundBuffers.load(SfxID::unfolding, "unfolding.wav" );
    mSoundBuffers.load(SfxID::big, "big.wav" );
    mSoundBuffers.load(SfxID::beam, "beam.wav" );
    mSoundBuffers.load(SfxID::roger, "msg dismissed.wav");
    // ... other sounds
}

void cSoundPlayer::play(SfxID fx)
{
    mSounds.push_back(sf::Sound { mSoundBuffers.get(fx) } );
    mSounds.back().setVolume(mVolume * gMixer[fx]);
    mSounds.back().play();
}

void cSoundPlayer::removeStoppedSounds()
{
    if ( mSounds.empty() ) return;
    mSounds.remove_if( [] (const sf::Sound& s)
        { return s.getStatus() == sf::Sound::Stopped; } );
}