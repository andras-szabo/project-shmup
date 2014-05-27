#ifndef __selector_class__selector__
#define __selector_class__selector__

#include <SFML/Graphics.hpp>
#include <vector>

class cSelector {
public:
    cSelector() { }
    cSelector(const std::vector<std::string>&, const sf::Vector2f&);
    void        setArrowTexture(const sf::Texture&, const sf::IntRect&);
    void        setFont(const sf::Font&, unsigned int = 22);
    void        setPosition(const sf::Vector2f&);
    void        setTextColor(const sf::Color&);
    void        setBkColor(const sf::Color&);
    void        setActiveItem(size_t);
    
    std::string getStateStr() const;
    size_t      getStateID() const;
    
    void        stepLeft();
    void        stepRight();
    
    void        toggleArrow(size_t);    // whether or not an arrow is lit,
                                        // b/c of being pressed down.
    void        render(sf::RenderTarget&, sf::RenderStates = sf::RenderStates::Default);
    
private:
    void        update();
    
private:
    std::vector<std::string>        mItem;
    size_t                          mSize;
    size_t                          mLongest;       // ID of longest item
    size_t                          mLength;        // longest item, in characters
    size_t                          mActiveItem;
    sf::Text                        mText;
    unsigned int                    mTextSize;
    sf::RectangleShape              mTextBox;
    sf::Font                        mFont;
    sf::Sprite                      mLeftArrow;
    sf::Sprite                      mRightArrow;
    sf::Vector2f                    mPos;
    unsigned int                    mTextBoxWidth;
    unsigned int                    mArrowWidth;
    unsigned int                    mArrowHeight;
    enum arrowstate { off, on, lit} mLeftArrowState, mRightArrowState;
};

#endif /* defined(__selector_class__selector__) */
