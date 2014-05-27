// The background is created in the following manner:
// we set up an area larger than the actual play field
// - in the current case, 4 times as large; i.e. double
// the size in width and in height -; and we draw vertical
// and horizontal lines from one edge of that area to
// the other. These lines may shift colour; and they may move
// about, vertical lines on the horizontal axes, horizontal
// ones on the vertical axes. When they move out of the area,
// they are put on the other end, so they simply turn around.
// This in practice creates an effect of movement.
// This moving grid is also rotated around its centre,
// for added coolness.

#include "bkGrid.h"
#include <cassert>

void cBkGrid::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = nullptr;
    target.draw(&mPoints[0], gGridPointCount, sf::Lines, states);
}

void cBkGrid::update(sf::Time dt)
{    
    mAccumulator += dt;
    if ( mAccumulator >= mTimer )
    {
        mAccumulator = sf::Time::Zero;
        adjustColor();
    }
    
    // Horizontal lines' velocity
    sf::Vector2f    dVhori { 0.0, mVelocity.y };
    dVhori *= dt.asSeconds();
    for (auto i = 0; i < gGridSize.y * 2; ++i)
    {
        mPoints[i].position += dVhori;
        
        if (mPoints[i].position.y < 0)
        {
            mPoints[i].position.y += mSize;
        }
        if (mPoints[i].position.y > mSize)
        {
            mPoints[i].position.y -= mSize;
        }
    }
    
    // Vertical lines' velocity
    sf::Vector2f    dVvert { mVelocity.x, 0.0 };
    dVvert *= dt.asSeconds();
    for (auto i = mFirstVline; i < gGridPointCount; ++i)
    {
        mPoints[i].position += dVvert;
        
        if (mPoints[i].position.x < 0)
        {
            mPoints[i].position.x += mSize;
        }
        if (mPoints[i].position.x > mSize)
        {
            mPoints[i].position.x -= mSize;
        }
    }
    
    // Now rotate
    mRot += mSpin * dt.asSeconds();
    if ( mRot < -360 ) mRot += 360;
    if ( mRot > 360 ) mRot -= 360;
    setRotation(mRot);
    
    // Adjust position
    setPosition(mHalfSize / 2.0, mHalfSize / 2.0);
}

cBkGrid::cBkGrid(sf::Color c)
{
    // mPoints is our own vertexArray. Its first gGridSize.Y*2 (!! Y !)
    // elements
    // contain vertices for horizontal lines, its second gGridSize.X*2
    // elements contain vertices for vertical lines (obviously these can
    // then rotate later on).
    // The grid covers an area that's 4 times the size of the view, to
    // make absolutely sure that whichever way we turn, all the view is
    // going to be covered.
    
    mHalfSize = std::max(gViewSize.x, gViewSize.y);
    mSize = mHalfSize * 2;
    mRot = 0;
    mSpin = 0;
    mR = 0;
    mG = 0;
    mB = 0;
    mA = 0;
    mTimer = sf::seconds(30);
    
    // Horizontal lines
    float gap { mSize / float (gGridSize.y) };
    
    auto i = 0;
    for ( ; i < gGridSize.y * 2; i += 2)
    {
        assert(i+1 < gGridPointCount);
        mPoints[i].position.x = 0;
        mPoints[i].position.y = (i / 2) * gap;
        mPoints[i+1].position.x = mSize;
        mPoints[i+1].position.y = mPoints[i].position.y;
        mPoints[i].color = c;
        mPoints[i+1].color = c;
    }
    
    // Vertical lines
    mFirstVline = i;
    gap = mSize / float(gGridSize.x);
    for ( ; i < mFirstVline + (gGridSize.x * 2); i += 2)
    {
        assert(i+1 < gGridPointCount);
        mPoints[i].position.x = ((i - mFirstVline) / 2) * gap;
        mPoints[i].position.y = 0;
        mPoints[i+1].position.x = mPoints[i].position.x;
        mPoints[i+1].position.y = mSize;
        mPoints[i].color = c;
        mPoints[i+1].color = c;
    }
    
    // Set up origin at the centre of the grid; this is the point around which
    // we rotate.
    setOrigin(mHalfSize, mHalfSize);
}
