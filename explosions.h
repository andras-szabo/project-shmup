
#ifndef __explosion_test__Explosion__
#define __explosion_test__Explosion__

#include <SFML/Graphics.hpp>

// Explosions are a bunch of square objects
// created at a given pos; and rendered transparent after a while.
// Let's put all squares' vertices into a single vertex array, and draw them,
// instead of calling draw as many times as there are squares.

const size_t    MAXSQUARES = 40;
const float     DURATION = 2.2;           // seconds.
const float     EXPLOSIONTIME = 0.16;    // seconds; the time that explosion squares are actually drawn

class ExplosionObject : public sf::Drawable, public sf::Transformable {
public:
    ExplosionObject(const sf::Vector2f&, const sf::Color&, int);
    bool                update(sf::Time); // returns false if there's nothing more to display
    bool                mVisible;
    bool                mDrawingFinished;   // if true, we can remove the exploding object's sprite
    // from underneath.
    
private:
    virtual void        draw(sf::RenderTarget&, sf::RenderStates) const;
    void                square_to_point(size_t);    // helper function
    
private:
    
    struct square {
        sf::Vector2f    velocity;
        sf::Vector2f    topleft;
        sf::Color       color;
        sf::Uint8       width;
        sf::Uint8       height;
    };
    
    sf::Vertex          mPoints[MAXSQUARES * 4];
    square              mSquares[MAXSQUARES];
    sf::Time            mTimer;
    sf::Time            mAccumulator;
    sf::Vector2f        mPosition;
    size_t              mSquareCount;
    size_t              mDebrisCount;
    size_t              mDisappearTracker;
    float               mExplTime;          // in seconds, the amount of time we spend creating
    // explosion squares; after this we'll start to
    // make them dissolve
    int                 mSize;
    bool                mExplosionOver;
};


#endif /* defined(__explosion_test__Explosion__) */
