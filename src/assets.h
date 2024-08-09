#pragma once

#include "schnitzel_lib.h"

// #############################################################################
// MARK:                     Asset Constants
// #############################################################################

// #############################################################################
// MARK:                     Assets Structs
// #############################################################################
enum SpriteID
{
    SPRITE_FACE,
    SPRITE_COUNT
};

struct Sprite
{
    IVec2 atlasOffset;
    IVec2 spriteSize;
};

// #############################################################################
// MARK:                     Assets Functions
// #############################################################################
Sprite get_sprite(SpriteID spriteID)
{
    Sprite sprite = {};

    switch (spriteID)
    {
        case SPRITE_FACE:
        {
            sprite.atlasOffset = {0, 0};
            sprite.spriteSize = {16, 16};
        }
    }

    return sprite;
}