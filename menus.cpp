#include "menus.h"

cMenu::cMenu(std::vector<std::string>& items,
             sf::Font& font, int size,
             sf::Vector2f pos,
             sf::Color piColor,
             sf::Color aiColor,
             sf::Color hColor):
mActiveItemColor { aiColor },
mPassiveItemColor { piColor },
mHighlightColor { hColor },
mActiveID { 0 },
mItemCount { 0 },
mHeight { 0 },
mDrawHighlight { true }
{
    // Set up menu items and check which one is the widest
    // one for positioning purposes
    
    float length = 0.0;
    for( auto& i : items )
    {
        mItem.push_back(sf::Text(i, font, size));
        mItem.back().setColor(piColor);
        if ( mItem.back().getLocalBounds().width > length )
            length = mItem.back().getLocalBounds().width;
            if ( mItem.back().getLocalBounds().height > mHeight)
                mHeight = mItem.back().getLocalBounds().height;
                ++mItemCount;
    }
    
    mTop = mItem[0].getLocalBounds().top + pos.y - mHeight / 2;
    mHighlight[0].position.x = pos.x - 20;
    mHighlight[1].position.x = pos.x + length + 35;
    mHighlight[2].position.x = pos.x + length + 15;
    mHighlight[3].position.x = pos.x - 20;
    update_highlight();
    
    for ( auto& i : mHighlight )
    { i.color = hColor; }
    
    mHighlight[2].color.a = 80;     // This is design.
    mHighlight[3].color.a = 80;     // In case you didn't notice.
    
    // align items centered on top of one another
    size_t c = 0;
    for ( auto& i : mItem )
    {
        auto diff = (length - i.getLocalBounds().width) / 2;
        i.setPosition(pos.x + diff, pos.y + c*mHeight);
        c += 2;
    }
    
    mItem[mActiveID].setColor(mActiveItemColor);
}

size_t cMenu::getActiveID() const
{
    return mActiveID;
}

void cMenu::render(sf::RenderTarget& t, sf::RenderStates s)
{
    // First: draw highlight, if needed
    if (mDrawHighlight)
    {
        mItem[mActiveID].setColor(mActiveItemColor);
        t.draw(&mHighlight[0], 4, sf::Quads, s);
    } else
        mItem[mActiveID].setColor(mPassiveItemColor);
    
    // Then draw items
    for ( auto& i : mItem )
        t.draw(i, s);
}

void cMenu::update_highlight()
{
    mHighlight[0].position.y = mActiveID * 2 * mHeight + mTop - mHeight / 8;
    mHighlight[1].position.y = mHighlight[0].position.y;
    mHighlight[2].position.y = mHighlight[1].position.y + mHeight + mHeight;
    mHighlight[3].position.y = mHighlight[2].position.y;
    for (auto i = 0; i < mItemCount; ++i)
    {
        if ( i == mActiveID ) mItem[i].setColor(mActiveItemColor);
        else mItem[i].setColor(mPassiveItemColor);
    }
        
}

void cMenu::move_up()
{
    mItem[mActiveID].setColor(mPassiveItemColor);
    mActiveID = mActiveID == 0 ? mItemCount-1 : mActiveID-1;
    mItem[mActiveID].setColor(mActiveItemColor);
    update_highlight();
    pSoundQueue->push(SfxID::menumove);
}

void cMenu::move_down()
{
    mItem[mActiveID].setColor(mPassiveItemColor);
    mActiveID = mActiveID == mItemCount-1 ? 0 : mActiveID+1;
    mItem[mActiveID].setColor(mActiveItemColor);
    update_highlight();
    pSoundQueue->push(SfxID::menumove);
}