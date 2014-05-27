#include "selector.h"

// This is just this little GUI element with a field
// of text and two arrows by its side.

cSelector::cSelector(const std::vector<std::string>& v, const sf::Vector2f& p)
{
    // Fill up text-vector, find longest string
    mLength = 0;
    mSize = 0;
    mLongest = 0;
    for ( auto& i : v )
    {
        mItem.push_back(i);
        
        if ( i.length() > mLength )
        {
            mLength = i.length();
            mLongest = mSize;
        }
        
        ++mSize;
    }
    
    // Set up textbox
    mTextBox = sf::RectangleShape(sf::Vector2f(10, 10));
    mTextBox.setOutlineColor(sf::Color::White);
    mTextBox.setFillColor(sf::Color::Black);
    mTextBox.setOutlineThickness(2);
    
    setPosition(p);
}

void cSelector::setArrowTexture(const sf::Texture& t, const sf::IntRect& i)
{
    mLeftArrow.setTexture(t);
    mLeftArrow.setTextureRect(i);
    mRightArrow.setTexture(t);
    mRightArrow.setTextureRect(i);
    mRightArrow.setRotation(180);
    
    mArrowWidth = i.width;
    mArrowHeight = i.height;
}


// Not only sets up font, but also calculates length
// of the textbox (longest string + buffer); sets up
// positions
void cSelector::setFont(const sf::Font& f, unsigned int sz)
{
    mFont = f;
    mText = sf::Text(mItem[mLongest], mFont, sz);
    mTextBoxWidth = mText.getLocalBounds().width + 30;
    
    mTextBox.setSize(sf::Vector2f(mTextBoxWidth, mText.getLocalBounds().height + 16));
    
    mLeftArrow.setPosition(mPos.x, mPos.y);
    mTextBox.setPosition(mPos.x + mArrowWidth + 6, mPos.y);
    mRightArrow.setPosition(mPos.x + mArrowWidth + 6 + mTextBoxWidth + 32, mPos.y + mArrowHeight);
    // We need to add mArrowHeight to the y pos of right arrow because it's the left arrow
    // rotated 180 degrees around its top left corner
}

void cSelector::setPosition(const sf::Vector2f& p)
{
    mPos = p;
    mLeftArrow.setPosition(mPos);
    mTextBox.setPosition(mPos.x + mArrowWidth + 4, mPos.y);
    mRightArrow.setPosition(mPos.x + mArrowWidth + 4 + mTextBoxWidth + 4, mPos.y);
}

void cSelector::setTextColor(const sf::Color& c)
{
    mText.setColor(c);
}

void cSelector::setBkColor(const sf::Color& c)
{
    mTextBox.setFillColor(c);
}

void cSelector::setActiveItem(size_t id)
{
    mActiveItem = id;
    update();
}

std::string cSelector::getStateStr() const
{
    return mItem[mActiveItem];
}

size_t cSelector::getStateID() const
{
    return mActiveItem;
}

void cSelector::stepLeft()
{
    if ( mActiveItem > 0 )
    {
        --mActiveItem; update();
    }
    // else: play sound effect letting the user know that she can't!
}

void cSelector::stepRight()
{
    if ( mActiveItem < mSize-1 )
    {
        ++mActiveItem; update();
    }
    // else: play sound effect letting the user know that she can't
}

// a == 0: left arrow
// a == 1: right arrow
void cSelector::toggleArrow(size_t a)
{
    switch (a) {
        case 0:
        {
            if ( mLeftArrowState == on )
            {
                mLeftArrowState = lit;
                mLeftArrow.setColor(sf::Color(255,255,255,155));
            }
            else if ( mLeftArrowState == lit )
            {
                mLeftArrowState = on;
                mLeftArrow.setColor(sf::Color::White);
            }
            break;
        }
        case 1:
        {
            if ( mRightArrowState == on )
            {
                mRightArrowState = lit;
                mRightArrow.setColor(sf::Color(255,255,255,155));
            }
            else if ( mRightArrowState == lit )
            {
                mRightArrowState = on;
                mRightArrow.setColor(sf::Color::White);
            }
            break;
        }
        default:
            break;
    }
    update();
}

void cSelector::update()
{
    // set up text and position it nicely in textbox
    mText.setString( mItem[mActiveItem] );
    mText.setPosition( mTextBox.getPosition().x + (mTextBoxWidth - mText.getLocalBounds().width) / 2,
                      mPos.y);
    
    // check state of arrows (present? lit?)
    if ( mActiveItem == 0 ) mLeftArrowState = off;
    else if ( mLeftArrowState != lit ) mLeftArrowState = on;
    if ( mActiveItem == mSize-1 ) mRightArrowState = off;
    else if ( mRightArrowState != lit ) mRightArrowState = on;
    
}

void cSelector::render(sf::RenderTarget& t, sf::RenderStates s)
{
    if ( mLeftArrowState != off ) t.draw(mLeftArrow, s);
    t.draw(mTextBox, s);
    t.draw(mText, s);
    if ( mRightArrowState != off ) t.draw(mRightArrow, s);
}