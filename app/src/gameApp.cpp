
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

void GameApp::updateActiveScreen(float deltaSeconds) {
  switch (activeScreen) {
  case Screen::mainMenu:
    updateMainMenu();
    break;
  case Screen::sizeSetup:
    updateSizeSetup();
    break;
  case Screen::nameSetup:
    updateNameSetup();
    break;
  case Screen::playing:
    updatePlaying(deltaSeconds);
    break;
  case Screen::won:
    updateWonScreen();
    break;
  case Screen::scoreBoard:
  case Screen::help:
    updateInfoScreen();
    break;
  }
}

void GameApp::updateMainMenu() {
  const int menuStep = pollMenuStep();
  if (menuStep != 0) {
    const int itemCount = static_cast<int>(menuItems.size());
    menuIndex = (menuIndex + menuStep + itemCount) % itemCount;
  }

  if (pollConfirm()) {
    chooseMenuItem(menuIndex);
  }
}

void GameApp::chooseMenuItem(int itemIndex) {
  switch (static_cast<MenuItem>(itemIndex)) {
  case MenuItem::newGame:
    typedRowText = std::to_string(settings.rowCount);
    typedColumnText = std::to_string(settings.columnCount);
    sizeFieldIndex = 0;
    statusMessage.clear();
    activeScreen = Screen::sizeSetup;
    break;

  case MenuItem::continueGame:
    continueSavedGame();
    break;

  case MenuItem::scoreBoard:
    scoreEntries = loadScoreBoard();
    activeScreen = Screen::scoreBoard;
    break;

  case MenuItem::help:
    activeScreen = Screen::help;
    break;

  case MenuItem::playerName:
    typedName = settings.playerName;
    statusMessage.clear();
    activeScreen = Screen::nameSetup;
    break;

  case MenuItem::quit:
    quitRequested = true;
    break;
  }
}

void GameApp::updateSizeSetup() {
  const int fieldStep = pollMenuStep();
  if (fieldStep != 0) {
    sizeFieldIndex = (sizeFieldIndex + fieldStep + 2) % 2;
  }

  std::string &activeText =
      sizeFieldIndex == 0 ? typedRowText : typedColumnText;

  if (IsKeyPressed(KEY_LEFT)) {
    changeSideText(activeText, -1);
  }
  if (IsKeyPressed(KEY_RIGHT)) {
    changeSideText(activeText, 1);
  }

  editTypedText(activeText, 2, true); // a side never has more than two digits

  if (pollCancel()) {
    activeScreen = Screen::mainMenu;
    return;
  }

  if (!pollConfirm()) {
    return;
  }

  const std::optional<int> chosenRows = parseNumber(typedRowText);
  const std::optional<int> chosenColumns = parseNumber(typedColumnText);

  if (!chosenRows.has_value() || !chosenColumns.has_value()) {
    showMessage("please type a number into both fields");
    return;
  }
  if (!isValidMazeSide(*chosenRows) || !isValidMazeSide(*chosenColumns)) {
    showMessage("the sides have to be between 10 and 50");
    return;
  }

  settings.rowCount = *chosenRows;
  settings.columnCount = *chosenColumns;
  startNewGame();
}

void GameApp::updateNameSetup() {
  editTypedText(typedName, maxPlayerNameLength, false);

  if (pollCancel()) {
    activeScreen = Screen::mainMenu;
    return;
  }

  if (!pollConfirm()) {
    return;
  }

  const std::string chosenName = cleanPlayerName(typedName);
  settings.playerName = chosenName;
  typedName = chosenName;

  showMessage(saveSettings(settings) ? "name saved as " + chosenName
                                     : "the name could not be saved");
  activeScreen = Screen::mainMenu;
}

void GameApp::updatePlaying(float deltaSeconds) {
  if (!session.isRunning()) {
    activeScreen = Screen::mainMenu;
    return;
  }

  session.addElapsedTime(static_cast<double>(deltaSeconds));

  const MazeLayout layout = calculateMazeLayout(session.maze(), mazeViewport());
  cellUnderMouse = cellAtPoint(GetMousePosition(), session.maze(), layout);

  const std::optional<Direction> chosenDirection = readMoveDirection();
  if (chosenDirection.has_value()) {
    applyMoveOutcome(session.movePlayer(*chosenDirection));
  }

  // The last step may have won the game, and then this screen is over.
  if (activeScreen != Screen::playing) {
    return;
  }

  if (IsKeyPressed(KEY_H)) {
    settings.showHintPath = !settings.showHintPath;
    showMessage(settings.showHintPath ? "hint is on" : "hint is off");
  }
  if (IsKeyPressed(KEY_TAB)) {
    numbersVisible = !numbersVisible;
  }
  if (IsKeyPressed(KEY_F2)) {
    saveCurrentGame();
  }
  if (IsKeyPressed(KEY_F5)) {
    startNewGame();
    showMessage("a new maze was built");
  }

  if (pollCancel()) {
    saveCurrentGame();
    activeScreen = Screen::mainMenu;
  }
}

void GameApp::updateWonScreen() {
  if (pollConfirm() || pollCancel()) {
    session.clearGame();
    activeScreen = Screen::mainMenu;
  }
}

void GameApp::updateInfoScreen() {
  if (pollCancel() || pollConfirm()) {
    activeScreen = Screen::mainMenu;
  }
}
