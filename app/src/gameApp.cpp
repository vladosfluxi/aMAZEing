
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

static std::optional<int> parseNumber(const std::string &text) {
  if (text.empty()) {
    return std::nullopt;
  }

  int value = 0;
  const char *firstCharacter = text.data();
  const char *lastCharacter = text.data() + text.size();
  const std::from_chars_result result =
      std::from_chars(firstCharacter, lastCharacter, value);

  if (result.ec != std::errc{} || result.ptr != lastCharacter) {
    return std::nullopt;
  }
  return value;
}

static void changeSideText(std::string &text, int step) {
  const std::optional<int> currentValue = parseNumber(text);
  text =
      std::to_string(clampMazeSide(currentValue.value_or(minMazeSide) + step));
}

GameApp::GameApp() {
  settings = loadSettings();
  scoreEntries = loadScoreBoard();

  menuItems = {"new game",    "continue saved game", "best runs",
               "how to play", "change player name",  "quit"};

  typedRowText = std::to_string(settings.rowCount);
  typedColumnText = std::to_string(settings.columnCount);
  typedName = settings.playerName;
}

void GameApp::run() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
  InitWindow(defaultWindowWidth, defaultWindowHeight, "Labyrinth");
  SetWindowMinSize(minimumWindowWidth, minimumWindowHeight);
  SetTargetFPS(targetFramesPerSecond);

  SetExitKey(
      KEY_NULL); // escape goes back inside the game, it must not close it

  while (!WindowShouldClose() && !quitRequested) {
    const float deltaSeconds = GetFrameTime();

    if (statusMessageTimer > 0.0f) {
      statusMessageTimer -= deltaSeconds;
      if (statusMessageTimer <= 0.0f) {
        statusMessage.clear();
      }
    }

    updateActiveScreen(deltaSeconds);

    BeginDrawing();
    ClearBackground(backgroundColor);
    drawActiveScreen();
    EndDrawing();
  }

  // Keep the size and the name for the next start.
  if (!saveSettings(settings)) {
    TraceLog(LOG_WARNING, "the settings could not be written");
  }

  CloseWindow();
}
