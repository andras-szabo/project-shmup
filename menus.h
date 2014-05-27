#ifndef __menutest__menus__
#define __menutest__menus__

#include <SFML/Graphics.hpp>
#include "soundPlayer.h"
#include <queue>

class cMenu {
public:
    cMenu() { };

    cMenu(std::vector<std::string>& items,
          sf::Font& font,
          int size,
          sf::Vector2f pos,
          sf::Color piColor = sf::Color::White,
          sf::Color aiColor = sf::Color::Black,
          sf::Color hColor = sf::Color::White);
    
    size_t      getActiveID() const;
    void        setActiveID(size_t p) { mActiveID = p; update_highlight(); };
    void        render(sf::RenderTarget& t, sf::RenderStates);
    void        move_up();
    void        move_down();
    void        setDrawHighlight(bool x) { mDrawHighlight = x; }
    void        setActiveText(const std::string& s)
                {
                    mItem[mActiveID].setString(s);
                }
    size_t      getSize() const { return mItemCount; }
    void        setUpSoundQueue(std::queue<SfxID>* p)
                { pSoundQueue = p; }
    
private:
    void        update_highlight();
    
private:
    std::vector<sf::Text>       mItem;
    sf::Color                   mPassiveItemColor;
    sf::Color                   mActiveItemColor;
    sf::Color                   mHighlightColor;
    sf::Vertex                  mHighlight[4];
    size_t                      mActiveID;
    size_t                      mItemCount;
    float                       mHeight;
    float                       mTop;
    bool                        mDrawHighlight;

    std::queue<SfxID>*          pSoundQueue;
};


#endif /* defined(__menutest__menus__) */