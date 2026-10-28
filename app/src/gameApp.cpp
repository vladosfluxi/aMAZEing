
#include "app/gameApp.h"

#include "app/inputReader.h"
#include "app/mazeRenderer.h"
#include "app/uiScreens.h"
#include "app/uiTheme.h"

#include "core/mazeGenerator.h"
#include "storage/dataFiles.h"
#include "storage/savedGame.h"

#include <charconv>
#include <utility>

static constexpr float messageSeconds = 3.0f;

// The lines of the main menu, in the order they are shown.
enum class MenuItem {
  newGame = 0,
  continueGame = 1,
  scoreBoard = 2,
  help = 3,
  playerName = 4,
  quit = 5
};
