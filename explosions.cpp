#include "explosions.h"

// Construction:
// receivers parameters: position, color (of exploding object and hence of the debris), size (of explosion)
// It creates a random # of appropriately colored small squares representing
// debris. These start from the given position with huge velocities
// outward. It notes the number of debris squares.
// Does nothing else.

// Note that mDebrisCount MUST NOT EXCEED the value defined by MAXSQUARES (in explosions.h);
// i.e. the way explosions work is that debris and explosion squares use the same
// vertexarray; the more debris, the fewer explosion rectangles.

ExplosionObject::ExplosionObject(const sf::Vector2f& pos, const sf::Color& color, int size):
mPosition { pos },
mTimer { sf::Time::Zero },
mAccumulator { sf::Time::Zero },
mExplTime { EXPLOSIONTIME },
mSize { size },
mExplosionOver { false },
mDrawingFinished { false },
mVisible { true }
{
    mDebrisCount = rand() % 22 + 10;
    mSquareCount = mDebrisCount;
    mDisappearTracker = mDebrisCount;   // When starting to make squares disappear, we don't want
    // debris to do so; that's why this pointer will point
    // to the one after the last piece of debris.
    
    // Let's create debris!
    for(auto i = 0; i < mDebrisCount; ++i)
    {
        mSquares[i].topleft = pos;
        mSquares[i].color = color;
        
        // Debris is by default the same color as the exploding object. But let's make it a bit
        // lighter (opaque), so as to make it easier to avoid bullets that may be of the same color.
        mSquares[i].color.a = 50;
        
        // We try to avoid either velocities being very close to 0
        mSquares[i].velocity.x = rand() % 400 - 200;
        if (mSquares[i].velocity.x <= 0)
            mSquares[i].velocity.x -= 50;
        else mSquares[i].velocity.x += 50;
            
        mSquares[i].velocity.y = rand() % 400 - 200;
        if (mSquares[i].velocity.y <= 0)
            mSquares[i].velocity.y -= 50;
        else mSquares[i].velocity.y += 50;
                
        mSquares[i].width = 4 + rand() % (mSize / 3);
        mSquares[i].height = 4 + rand() % (mSize / 3);
        square_to_point(i);
    }
}

// This makes the connection between squares and vertices in our vertexarray.
void ExplosionObject::square_to_point(size_t i)
{
    for (auto j = 0; j < 4; ++j)
    {
        mPoints[i*4 + j].position = mSquares[i].topleft;
        mPoints[i*4 + j].color = mSquares[i].color;
    }
    
    mPoints[i*4 + 1].position.x += mSquares[i].width;
    mPoints[i*4 + 3].position.y += mSquares[i].height;
    mPoints[i*4 + 2].position.x += mSquares[i].width;
    mPoints[i*4 + 2].position.y += mSquares[i].height;
}

bool ExplosionObject::update(sf::Time dt)
{
    mTimer += dt;
    mAccumulator += dt; // used for calculating when the next drawing of squares is due
    if (mTimer >= sf::seconds(DURATION)) { mSquareCount = 0; mVisible = false; return false; }
    
    // First: update flying debris position
    for (auto i = 0; i < mDebrisCount; ++i)
        for (auto j = 0; j < 4; ++j)
            mPoints[i*4 + j].position += mSquares[i].velocity * dt.asSeconds();
    
    // If we're at MAXSQUARES already, we start making 'em disappear
    if (mExplosionOver) return true;
    if (mDrawingFinished && (mDisappearTracker < MAXSQUARES-1))
    {
        mSquares[mDisappearTracker].color = sf::Color::Transparent;
        square_to_point(mDisappearTracker++);
        if (mDisappearTracker == MAXSQUARES-1) mExplosionOver = true;
        return true;
    }
    
    // Then we add some more squares - if needed - and also insert them in the vertexarray
    
    int squares_needed = (MAXSQUARES-mDebrisCount) * (mAccumulator.asSeconds() / mExplTime);
    if (!squares_needed) return true;
    mAccumulator = sf::Time::Zero;
    int i = 0;
    while (( i < squares_needed )  && (mSquareCount < MAXSQUARES-1))
    {
        mSquares[mSquareCount].topleft = mPosition + sf::Vector2f(rand() % 48 - 24, rand() % 48 - 24);
        mSquares[mSquareCount].width = rand() % mSize + mSquareCount / 4 + 4;
        mSquares[mSquareCount].height = rand() % mSize + mSquareCount / 4 + 4;
        int r = rand() % 3;
        switch (r)
        {
            case 0: mSquares[mSquareCount].color = sf::Color::White; break;
            case 1: mSquares[mSquareCount].color = sf::Color::Red; break;
            case 2: mSquares[mSquareCount].color = sf::Color::Yellow; break;
        }
        square_to_point(mSquareCount);
        ++mSquareCount;
        ++i;
    }
    if (mSquareCount == MAXSQUARES-1) mDrawingFinished = true;
    return true;
}

void ExplosionObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //states.transform *= getTransform();
    // We don't need to call getTransform() again, because explosions will be called
    // by Entities with their already-transformed-states; relative to that, explosions
    // have the same coordinates.
    states.texture = nullptr;
    int i = mExplosionOver ? mDebrisCount : mSquareCount;
    target.draw(&mPoints[0], i * 4, sf::Quads, states);
}