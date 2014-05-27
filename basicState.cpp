// This is the base and parent of all state classes.
// It's set up by initializing a pointer to the Engine,
// references to the Engine's basic resources (gamewindow,
// fontholder, textureholder, sound and music queues),
// and to variables that should exist only in one copy
// (for the engine), but which should be accessible to
// all states - namely, variables describing control
// settings (keyboard or joystick; particular control keys)

#include "basicState.h"

cState::cState(cEngine* p, const std::string& name):
pEngine { p },
mWindow { pEngine->mWindow },
mFontHolder { pEngine->mFontHolder },
mTextureHolder { pEngine->mTextureHolder },
mSoundQueue { pEngine->mSoundQueue },
mMusicQueue { pEngine->mMusicQueue },
mName { name },
mControl { pEngine->mControl },
mIsActive { true },
mK { pEngine->k },
mJ { pEngine->j }
{

}

void cState::pushRequest(std::string s)
{
    mIsActive = false;
    pEngine->request(SAID::push, s);
}

void cState::popRequest()
{
    pEngine->request(SAID::pop);
}

void cState::swapRequest(std::string s)
{
    pEngine->request(SAID::swap, s);
}

void cState::returnRequest(std::string s)
{
    pEngine->request(SAID::return_to, s);
}

void cState::clearRequest()
{
    pEngine->request(SAID::clear);
}