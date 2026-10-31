#include "app/inputReader.h"

#include <array>
#include <raylib.h>

// Every direction can be used with an arrow key or with a letter key.
struct DirectionKeys
{
    Direction direction;
    int arrowKey;
    int letterKey;
};

static constexpr std::array<DirectionKeys, directionCount> directionKeyTable{
    DirectionKeys{Direction::north, KEY_UP, KEY_W},
    DirectionKeys{Direction::east, KEY_RIGHT, KEY_D},
    DirectionKeys{Direction::south, KEY_DOWN, KEY_S},
    DirectionKeys{Direction::west, KEY_LEFT, KEY_A}};

