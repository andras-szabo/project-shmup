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

#ifndef __shmup_project__bkGrid__
#define __shmup_project__bkGrid__

#include <SFML/Graphics.hpp>
#include "globalVariables.h"

class cBkGrid : public sf::Drawable, public sf::Transformable
{
public:
    cBkGrid(sf::Color = sf::Color(0,80,0,255));
    void            update(sf::Time);
    void            setColor(sf::Color c)
    {
        mColor = c;
        for (auto& i : mPoints)
            i.color = mColor;
    }
    
    void            adjustColor(sf::Color c)
    {
        mColor += c;
        for (auto& i : mPoints)
            i.color = mColor;
    }
    
    void            adjustColor(short int c)
    {
        int t = mColor.r + c;
        if ( t > 255 ) t = 255;
        if ( t < 0 ) t = 0;
        mColor.r = t;
        
        t = mColor.g + c;
        if ( t > 255 ) t = 255;
        if ( t < 0 ) t = 0;
        mColor.g = t;
        
        t = mColor.b + c;
        if ( t > 255 ) t = 255;
        if ( t < 0 ) t = 0;
        mColor.b = t;
        for ( auto& i : mPoints )
            i.color = mColor;
    }
    
    // Helper function; forces "x" to be between low and hi (inclusive),
    // and inverts "y" if bounds reached; To be used with adjustcolor's
    // "bounce".
    void            bounds(sf::Uint8 &x, int &y, int lo = 3, int hi = 252)
    {
        if ( y + int(x) <= hi && y + int(x) >= lo ) x += y;
        else
        {
            if ( y + int(x) > hi )
            { x = hi + 1; }
            
            if ( y + int(x) < lo )
            { x = lo -1; }
        }
        
        if ( x > hi ) { x = hi; y *= -1; }
        if ( x < lo ) { x = lo; y *= -1; }
    }
    
    void            adjustColor(bool bounce = true)
    {
        
        // If bounce, then upon reaching a colour boundary,
        // the gradient inverts.
        
        if ( bounce )
        {
            bounds(mColor.r, mR);
            bounds(mColor.g, mG);
            bounds(mColor.b, mB);
            bounds(mColor.a, mA);
        }
        
        for ( auto& i : mPoints )
        {
            i.color = mColor;
        }
    }
    
    void            setFadeTime(sf::Time t) { mTimer = t; }
    void            setGradient(int r, int g, int b, int a)
    {
        mR = r; mG = g; mB = b; mA = a;
    }
    
    void            setVelocity(sf::Vector2f x) { mVelocity = x; }
    void            adjustVelocity(sf::Vector2f x) { mVelocity += x; }
    sf::Vector2f    getVelocity() const { return mVelocity; }
    
    void            setRotate(float f) { mRot = f; }
    void            adjustRotate(float f) { mRot += f; }
    float           getRotate() const { return mRot; }
    
    void            setSpin(float f) { mSpin = f; }
    void            adjustSpin(float f) { mSpin += f; }
    float           getSpin() const { return mSpin; }
    
private:
    virtual void    draw(sf::RenderTarget&, sf::RenderStates) const;
    
private:
    sf::Vertex      mPoints[gGridPointCount];
    sf::Vector2f    mVelocity;
    sf::Color       mColor;
    int             mR;     // Color gradients. Can't be a simple sf::Color
    int             mG;     // because that uses unsigned ints, so it would
    int             mB;     // be cumbersome to fade out (i.e. to adjust
    int             mA;     // colors by negative numbers).
    float           mRot;
    float           mSpin;  // Spin: constant rotation
    
    float           mHalfSize;
    float           mSize;
    
    size_t          mFirstVline;    // in the vertex array.
    sf::Time        mAccumulator;
    sf::Time        mTimer;
    short int       mGamma;         // our own Gamma correction variable
    
    // Timer and Accumulator: accumulator collects dt; when it reaches mTimer,
    // it adjusts colour with the gradient. So you can adjust the speed of colour
    // fading ("grading") through setting mTimer.
};


#endif /* defined(__shmup_project__bkGrid__) */
