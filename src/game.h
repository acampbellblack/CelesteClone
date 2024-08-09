#pragma once

#include "input.h"
#include "schnitzel_lib.h"
#include "render_interface.h"

// #############################################################################
// MARK:                     Game Globals
// #############################################################################
constexpr int tset = 5;

// #############################################################################
// MARK:                     Game Structs
// #############################################################################
struct GameState
{
    bool initialized = false;
    IVec2 playerPos;
};

// #############################################################################
// MARK:                     Game Globals
// #############################################################################
static GameState* gameState;

// #############################################################################
// MARK:                     Game Functions (Exposed)
// #############################################################################
extern "C"
{
    EXPORT_FN void update_game(GameState* gameStateIn, RenderData* renderDataIn, Input* inputIn);
}