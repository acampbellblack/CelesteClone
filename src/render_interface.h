#pragma once

#include "assets.h"
#include "schnitzel_lib.h"

// #############################################################################
// MARK:                     Renderer Constants
// #############################################################################
constexpr int MAX_TRANSFORMS = 1000;

// #############################################################################
// MARK:                     Renderer Structs
// #############################################################################
struct OrthographicCamera2D
{
    float zoom = 1.0f;
    Vec2 dimensions;
    Vec2 position;
};

struct Transform
{
    IVec2 atlasOffset;
    IVec2 spriteSize;
    Vec2 pos;
    Vec2 size;
};

struct RenderData
{
    OrthographicCamera2D gameCamera;
    OrthographicCamera2D uiCamera;

    int transformCount;
    Transform transforms[MAX_TRANSFORMS];
};

// #############################################################################
// MARK:                     Renderer Globals
// #############################################################################
static RenderData* renderData;

// #############################################################################
// MARK:                     Renderer Functions
// #############################################################################
void draw_sprite(SpriteID spriteID, Vec2 pos)
{
    Sprite sprite = get_sprite(spriteID);

    Transform transform = {};
    transform.atlasOffset = sprite.atlasOffset;
    transform.spriteSize = sprite.spriteSize;
    transform.pos = pos - vec_2(sprite.spriteSize) / 2.0f;
    transform.size = vec_2(sprite.spriteSize);

    renderData->transforms[renderData->transformCount++] = transform;
}