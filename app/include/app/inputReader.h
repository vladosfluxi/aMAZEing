// Reading the keyboard. Only this part of the program touches raylib input,
// so the rules of the game stay free of any interface code.
#pragma once

#include "core/mazeTypes.h"

#include <optional>
#include <string>

// Turns the arrow keys (and W, A, S, D) into a direction.
//
// A short press gives one step, and a key that stays down repeats, so walking
// through a long corridor does not need dozens of single presses. The repeat
// itself comes from raylib, which uses the settings of the system for it.
std::optional<Direction> readMoveDirection();

// -1 for up, +1 for down and 0 when nothing was pressed.
int pollMenuStep();

// Enter or space.
bool pollConfirm();

// Escape, used to leave a screen or to go back to the menu.
bool pollCancel();

// Adds the typed characters to the text and removes the last one when
// backspace is pressed. Only characters that are allowed for the field are
// accepted, which is the first step of the input validation.
void editTypedText(std::string& text, int maxLength, bool digitsOnly);
