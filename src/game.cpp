#include "game.h"
#include "assets.h"

// #############################################################################
// MARK:                     Game Constants
// #############################################################################
constexpr int WORLD_WIDTH = 320;
constexpr int WORLD_HEIGHT = 180;
constexpr int TILESIZE = 8;

// #############################################################################
// MARK:                     Game Structs
// #############################################################################

// #############################################################################
// MARK:                     Game Functions
// #############################################################################
EXPORT_FN void update_game(GameState* gameStateIn, RenderData* renderDataIn, Input* inputIn)
{
    if (renderData != renderDataIn)
    {
        gameState = gameStateIn;
        renderData = renderDataIn;
        input = inputIn;
    }

    if (!gameState->initialized)
    {
        renderData->gameCamera.dimensions = {WORLD_WIDTH, WORLD_HEIGHT};
        gameState->initialized = true;
    }

    renderData->gameCamera.position.x = 0.0f;
    renderData->gameCamera.position.y = 0.0f;

    draw_sprite(SPRITE_FACE, {0.0f, 0.0f});
}
