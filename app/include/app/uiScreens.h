// The single screens: menu, the two questions, help, score board, the panel
// beside the maze and the win message.
#pragma once

#include "core/gameSession.h"
#include "storage/scoreBoard.h"

#include <raylib.h>
#include <string>
#include <vector>

// Seconds as "mm:ss".
std::string timeText(double seconds);

void drawTopBar(const std::string& title, const std::string& subtitle);
void drawPanel(Rectangle area);

void drawMainMenu(const std::vector<std::string>& menuItems, int selectedIndex, const std::string& playerName,
                  const std::string& message);

// activeField is 0 for the rows and 1 for the columns.
void drawSizeScreen(const std::string& rowText, const std::string& columnText, int activeField,
                    const std::string& message);

void drawNameScreen(const std::string& typedName, const std::string& message);

void drawHelpScreen(const std::string& dataFolderText);

void drawScoreScreen(const std::vector<ScoreEntry>& entries);

// shownCell is the cell under the mouse, or the one the player stands on when
// the mouse is somewhere else.
void drawGamePanel(Rectangle area, const GameSession& session, const std::string& playerName,
                   const std::string& message, bool hintVisible, bool numbersVisible,
                   const CellReport& shownCell, bool shownCellIsPlayerCell);

void drawWinOverlay(const GameSession& session, int score, int place);
