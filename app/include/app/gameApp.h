// The window, the current run and the switching between the screens.
// This is the one place where the three layers meet.
#pragma once

#include "core/gameSession.h"
#include "storage/gameSettings.h"
#include "storage/scoreBoard.h"

#include <optional>
#include <raylib.h>
#include <string>
#include <vector>

enum class Screen {
  mainMenu,
  sizeSetup,
  nameSetup,
  playing,
  won,
  scoreBoard,
  help
};

class GameApp {
public:
  GameApp();

  // Opens the window and runs until the player quits.
  void run();

private:
  void updateActiveScreen(float deltaSeconds);
  void drawActiveScreen();

  void updateMainMenu();
  void updateSizeSetup();
  void updateNameSetup();
  void updatePlaying(float deltaSeconds);
  void updateWonScreen();
  void updateInfoScreen();

  void chooseMenuItem(int itemIndex);
  void startNewGame();
  void continueSavedGame();
  void saveCurrentGame();
  void finishRun();
  void applyMoveOutcome(MoveOutcome outcome);

  void showMessage(std::string text);
  void drawMazeArea() const;

  // The free area of the window that the maze is drawn into.
  Rectangle mazeViewport() const;

  CellReport cellForThePanel() const;

  Screen activeScreen = Screen::mainMenu;

  GameSession session;
  GameSettings settings;
  std::vector<ScoreEntry> scoreEntries;
  std::vector<std::string> menuItems;

  // Set while the mouse points at a cell of the maze.
  std::optional<Position> cellUnderMouse;

  std::string statusMessage;
  float statusMessageTimer = 0.0f;

  // What the player types on the size and the name screen.
  std::string typedRowText;
  std::string typedColumnText;
  std::string typedName;

  int menuIndex = 0;
  int sizeFieldIndex = 0;
  int lastScore = 0;
  int lastPlace = 0;

  bool numbersVisible = true;
  bool quitRequested = false;
};
