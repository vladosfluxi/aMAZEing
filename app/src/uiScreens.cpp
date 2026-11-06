#include "app/uiScreens.h"

#include "app/uiTheme.h"

#include <algorithm>
#include <array>

static constexpr int lineHeight = 28;

static void drawCenteredText(const std::string &text, int centerX,
                             int positionY, int fontSize, Color color) {
  const int textWidth = MeasureText(text.c_str(), fontSize);
  DrawText(text.c_str(), centerX - textWidth / 2, positionY, fontSize, color);
}

static void drawLabelledValue(const std::string &label,
                              const std::string &value, int positionX,
                              int positionY, Color valueColor) {
  DrawText(label.c_str(), positionX, positionY, textFontSize, dimTextColor);
  const int labelWidth = MeasureText(label.c_str(), textFontSize);
  DrawText(value.c_str(), positionX + labelWidth + 8, positionY, textFontSize,
           valueColor);
}

static void drawTextField(const std::string &content, Rectangle area,
                          bool isActive) {
  DrawRectangleRec(area, panelColor);
  DrawRectangleLinesEx(area, isActive ? 3.0f : 1.0f,
                       isActive ? highlightColor : panelBorderColor);

  const std::string shownText = isActive ? content + "_" : content;
  DrawText(
      shownText.c_str(), static_cast<int>(area.x + 12.0f),
      static_cast<int>(
          area.y + (area.height - static_cast<float>(headingFontSize)) * 0.5f),
      headingFontSize, textColor);
}
static Rectangle centeredPanel(float width, float height) {
  const float screenWidth = static_cast<float>(GetScreenWidth());
  const float screenHeight = static_cast<float>(GetScreenHeight());
  return Rectangle{(screenWidth - width) * 0.5f, (screenHeight - height) * 0.5f,
                   width, height};
}

static Rectangle panelUnderTitle(float width, float height, float topY) {
  return Rectangle{(static_cast<float>(GetScreenWidth()) - width) * 0.5f, topY,
                   width, height};
}

static Rectangle fullScreenPanel() {
  return Rectangle{panelPadding, topBarHeight,
                   static_cast<float>(GetScreenWidth()) - 2.0f * panelPadding,
                   static_cast<float>(GetScreenHeight()) - topBarHeight -
                       panelPadding};
}
std::string timeText(double seconds) {
  const int wholeSeconds = std::max(0, static_cast<int>(seconds));
  const int minutePart = wholeSeconds / 60;
  const int secondPart = wholeSeconds % 60;

  std::string result = std::to_string(minutePart);
  result += ':';
  if (secondPart < 10) {
    result += '0';
  }
  result += std::to_string(secondPart);
  return result;
}
void drawTopBar(const std::string &title, const std::string &subtitle) {
  DrawText(title.c_str(), static_cast<int>(panelPadding), 14, headingFontSize,
           textColor);
  if (!subtitle.empty()) {
    const int titleWidth = MeasureText(title.c_str(), headingFontSize);
    DrawText(subtitle.c_str(), static_cast<int>(panelPadding) + titleWidth + 16,
             20, smallFontSize, dimTextColor);
  }

  DrawLineEx(Vector2{panelPadding, topBarHeight - 12.0f},
             Vector2{static_cast<float>(GetScreenWidth()) - panelPadding,
                     topBarHeight - 12.0f},
             1.0f, panelBorderColor);
}

void drawPanel(Rectangle area) {
  DrawRectangleRec(area, panelColor);
  DrawRectangleLinesEx(area, 1.0f, panelBorderColor);
}

void drawMainMenu(const std::vector<std::string> &menuItems, int selectedIndex,
                  const std::string &playerName, const std::string &message) {
  const int centerX = GetScreenWidth() / 2;

  drawCenteredText("LABYRINTH", centerX, 90, titleFontSize, highlightColor);
  drawCenteredText("find the way from the top left corner to the exit", centerX,
                   146, textFontSize, dimTextColor);

  const Rectangle menuArea = panelUnderTitle(
      420.0f, static_cast<float>(menuItems.size()) * 46.0f + 40.0f, 190.0f);
  drawPanel(menuArea);

  for (std::size_t itemIndex = 0; itemIndex < menuItems.size(); ++itemIndex) {
    const bool isSelected = static_cast<int>(itemIndex) == selectedIndex;
    const int positionY =
        static_cast<int>(menuArea.y + 20.0f) + static_cast<int>(itemIndex) * 46;

    if (isSelected) {
      DrawRectangle(static_cast<int>(menuArea.x + 10.0f), positionY - 6,
                    static_cast<int>(menuArea.width - 20.0f), 38,
                    Color{46, 54, 76, 255});
      DrawText(">", static_cast<int>(menuArea.x + 22.0f), positionY,
               headingFontSize, highlightColor);
    }

    DrawText(menuItems[itemIndex].c_str(), static_cast<int>(menuArea.x + 54.0f),
             positionY, headingFontSize, isSelected ? textColor : dimTextColor);
  }

  drawCenteredText("player: " + playerName, centerX,
                   static_cast<int>(menuArea.y + menuArea.height) + 18,
                   textFontSize, dimTextColor);
  drawCenteredText("arrow keys choose, enter confirms", centerX,
                   GetScreenHeight() - 32, smallFontSize, dimTextColor);

  if (!message.empty()) {
    drawCenteredText(message, centerX, static_cast<int>(menuArea.y - 44.0f),
                     textFontSize, warningColor);
  }
}

void drawSizeScreen(const std::string &rowText, const std::string &columnText,
                    int activeField, const std::string &message) {
  const int centerX = GetScreenWidth() / 2;
  drawTopBar("New game", "step 1 of 1: the size of the maze");

  const Rectangle area = centeredPanel(520.0f, 320.0f);
  drawPanel(area);

  drawCenteredText("how big should the maze be?", centerX,
                   static_cast<int>(area.y + 26.0f), headingFontSize,
                   textColor);
  drawCenteredText("allowed: 10 to 50 for both sides", centerX,
                   static_cast<int>(area.y + 64.0f), smallFontSize,
                   dimTextColor);

  const float fieldWidth = 160.0f;
  const float fieldHeight = 54.0f;
  const float fieldY = area.y + 124.0f;

  DrawText("rows", static_cast<int>(area.x + 60.0f),
           static_cast<int>(fieldY - 26.0f), smallFontSize, dimTextColor);
  drawTextField(rowText,
                Rectangle{area.x + 60.0f, fieldY, fieldWidth, fieldHeight},
                activeField == 0);

  DrawText("columns", static_cast<int>(area.x + 300.0f),
           static_cast<int>(fieldY - 26.0f), smallFontSize, dimTextColor);
  drawTextField(columnText,
                Rectangle{area.x + 300.0f, fieldY, fieldWidth, fieldHeight},
                activeField == 1);

  drawCenteredText("up / down change the field, left / right change the number",
                   centerX, static_cast<int>(area.y + 224.0f), smallFontSize,
                   dimTextColor);
  drawCenteredText("enter starts the game, escape goes back", centerX,
                   static_cast<int>(area.y + 252.0f), smallFontSize,
                   dimTextColor);

  if (!message.empty()) {
    drawCenteredText(message, centerX,
                     static_cast<int>(area.y + area.height) + 20, textFontSize,
                     warningColor);
  }
}

void drawNameScreen(const std::string &typedName, const std::string &message) {
  const int centerX = GetScreenWidth() / 2;
  drawTopBar("Player name", "used for the score board");

  const Rectangle area = centeredPanel(520.0f, 220.0f);
  drawPanel(area);

  drawCenteredText("who is playing?", centerX, static_cast<int>(area.y + 26.0f),
                   headingFontSize, textColor);
  drawTextField(
      typedName,
      Rectangle{area.x + 60.0f, area.y + 86.0f, area.width - 120.0f, 54.0f},
      true);
  drawCenteredText("letters, digits, space, - and _ are allowed", centerX,
                   static_cast<int>(area.y + 156.0f), smallFontSize,
                   dimTextColor);
  drawCenteredText("enter saves the name, escape goes back", centerX,
                   static_cast<int>(area.y + 182.0f), smallFontSize,
                   dimTextColor);

  if (!message.empty()) {
    drawCenteredText(message, centerX,
                     static_cast<int>(area.y + area.height) + 20, textFontSize,
                     warningColor);
  }
}

void drawHelpScreen(const std::string &dataFolderText) {
  drawTopBar("How to play", "escape goes back to the menu");

  const Rectangle area = fullScreenPanel();
  drawPanel(area);

  const std::array<std::string, 16> helpLines{
      "Goal",
      "  You start in the top left cell and have to reach the exit in the "
      "bottom right cell.",
      "  The whole maze is visible from above, so you can plan your way before "
      "you walk it.",
      "",
      "Rules",
      "  - Grey lines are walls and cannot be crossed.",
      "  - An orange line is a door. A door is a shortcut, but it needs one "
      "key to open.",
      "  - Yellow circles are keys. Walking over a key puts it into your "
      "inventory.",
      "  - Every cell shows a number from 1 to 9. Walking on it adds the "
      "number to your cost.",
      "  - Points grow with the size of the maze and with opened doors, and "
      "shrink with",
      "    steps, cost and time, so the short and cheap way is the best way.",
      "",
      "Keys",
      "  arrows or W A S D move      H shows or hides the hint      TAB shows "
      "the numbers",
      "  F2 saves the run           F5 builds a new maze           escape goes "
      "to the menu",
      "  the mouse asks about any cell of the maze"};

  int positionY = static_cast<int>(area.y + 22.0f);
  for (const std::string &helpLine : helpLines) {
    const bool isHeadline = !helpLine.empty() && helpLine.front() != ' ';
    DrawText(helpLine.c_str(), static_cast<int>(area.x + 22.0f), positionY,
             textFontSize, isHeadline ? highlightColor : textColor);
    positionY += lineHeight;
  }

  DrawText(("your games are stored in: " + dataFolderText).c_str(),
           static_cast<int>(area.x + 22.0f), positionY + 6, smallFontSize,
           dimTextColor);
}

void drawScoreScreen(const std::vector<ScoreEntry> &entries) {
  drawTopBar("Best runs", "escape goes back to the menu");

  const Rectangle area = fullScreenPanel();
  drawPanel(area);

  if (entries.empty()) {
    drawCenteredText(
        "no finished run yet - win a game and your name shows up here",
        static_cast<int>(area.x + area.width * 0.5f),
        static_cast<int>(area.y + 40.0f), textFontSize, dimTextColor);
    return;
  }

  const std::array<int, 8> columnOffsets{0, 40, 230, 330, 420, 500, 590, 690};
  const std::array<std::string, 8> columnTitles{
      "#", "player", "maze", "steps", "keys", "time", "points", "played at"};

  const int leftEdge = static_cast<int>(area.x + 24.0f);
  int positionY = static_cast<int>(area.y + 20.0f);

  for (std::size_t columnIndex = 0; columnIndex < columnTitles.size();
       ++columnIndex) {
    DrawText(columnTitles[columnIndex].c_str(),
             leftEdge + columnOffsets[columnIndex], positionY, textFontSize,
             dimTextColor);
  }
  positionY += lineHeight + 6;

  for (std::size_t entryIndex = 0; entryIndex < entries.size(); ++entryIndex) {
    const ScoreEntry &entry = entries[entryIndex];
    const Color rowColor = entryIndex == 0 ? highlightColor : textColor;

    const std::array<std::string, 8> rowTexts{
        std::to_string(entryIndex + 1),
        entry.playerName,
        std::to_string(entry.rowCount) + "x" +
            std::to_string(entry.columnCount),
        std::to_string(entry.stepCount),
        std::to_string(entry.keysUsed),
        timeText(entry.elapsedSeconds),
        std::to_string(entry.score),
        entry.playedAt};

    for (std::size_t columnIndex = 0; columnIndex < rowTexts.size();
         ++columnIndex) {
      const Color columnColor =
          columnIndex + 1 == rowTexts.size() ? dimTextColor : rowColor;
      DrawText(rowTexts[columnIndex].c_str(),
               leftEdge + columnOffsets[columnIndex], positionY, textFontSize,
               columnColor);
    }
    positionY += lineHeight;
  }
}

void drawGamePanel(Rectangle area, const GameSession &session,
                   const std::string &playerName, const std::string &message,
                   bool hintVisible, bool numbersVisible,
                   const CellReport &shownCell, bool shownCellIsPlayerCell) {
  drawPanel(area);

  const int leftEdge = static_cast<int>(area.x + 18.0f);
  int positionY = static_cast<int>(area.y + 18.0f);

  DrawText(playerName.c_str(), leftEdge, positionY, headingFontSize,
           highlightColor);
  positionY += 40;

  const GameStatistics &statistics = session.statistics();
  const Position playerPosition = session.playerPosition();

  drawLabelledValue("position:",
                    "row " + std::to_string(playerPosition.row) + ", col " +
                        std::to_string(playerPosition.column),
                    leftEdge, positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("steps:", std::to_string(statistics.stepCount), leftEdge,
                    positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("keys:", std::to_string(statistics.keyCount), leftEdge,
                    positionY,
                    statistics.keyCount > 0 ? keyColor : dimTextColor);
  positionY += lineHeight;
  drawLabelledValue("doors opened:", std::to_string(statistics.keysUsed),
                    leftEdge, positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("cost:", std::to_string(statistics.collectedCost), leftEdge,
                    positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("time:", timeText(statistics.elapsedSeconds), leftEdge,
                    positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("points now:", std::to_string(session.currentScore()),
                    leftEdge, positionY, successColor);
  positionY += lineHeight + 14;

  const std::string cellTitle =
      shownCellIsPlayerCell
          ? "this cell"
          : "cell row " + std::to_string(shownCell.position.row) + ", col " +
                std::to_string(shownCell.position.column);
  DrawText(cellTitle.c_str(), leftEdge, positionY, textFontSize,
           highlightColor);
  positionY += lineHeight;
  drawLabelledValue("number:", std::to_string(shownCell.stepCost), leftEdge,
                    positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("open sides:", std::to_string(shownCell.openSides),
                    leftEdge, positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("doors around:", std::to_string(shownCell.doorSides),
                    leftEdge, positionY,
                    shownCell.doorSides > 0 ? doorColor : textColor);
  positionY += lineHeight;

  drawLabelledValue(
      "way to exit:",
      hintVisible ? std::to_string(shownCell.distanceToExit) + " steps"
                  : "press H",
      leftEdge, positionY, hintVisible ? textColor : dimTextColor);
  positionY += lineHeight + 14;

  DrawText("keys of the game", leftEdge, positionY, textFontSize,
           highlightColor);
  positionY += lineHeight;

  const std::array<std::string, 6> controlLines{
      "arrows / WASD  move",
      std::string("H  hint: ") + (hintVisible ? "on" : "off"),
      std::string("TAB  numbers: ") + (numbersVisible ? "on" : "off"),
      "F2 save    F5 new maze",
      "escape  save and go back",
      "mouse  asks about a cell"};

  for (const std::string &controlLine : controlLines) {
    DrawText(controlLine.c_str(), leftEdge, positionY, smallFontSize,
             dimTextColor);
    positionY += 24;
  }

  if (!message.empty()) {
    const int bottomMessageY = static_cast<int>(area.y + area.height) - 48;
    DrawText(message.c_str(), leftEdge,
             std::max(positionY + 16, bottomMessageY), smallFontSize,
             warningColor);
  }
}
void drawWinOverlay(const GameSession &session, int score, int place) {
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                Color{10, 12, 18, 200});

  const Rectangle area = centeredPanel(520.0f, 380.0f);
  drawPanel(area);

  const int centerX = static_cast<int>(area.x + area.width * 0.5f);
  const GameStatistics &statistics = session.statistics();

  drawCenteredText("you found the exit", centerX,
                   static_cast<int>(area.y + 28.0f), titleFontSize,
                   successColor);

  int positionY = static_cast<int>(area.y + 108.0f);
  const int leftEdge = static_cast<int>(area.x + 70.0f);

  drawLabelledValue("maze:",
                    std::to_string(session.maze().rowCount()) + "x" +
                        std::to_string(session.maze().columnCount()),
                    leftEdge, positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("steps:", std::to_string(statistics.stepCount), leftEdge,
                    positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("cost:", std::to_string(statistics.collectedCost), leftEdge,
                    positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("doors opened:", std::to_string(statistics.keysUsed),
                    leftEdge, positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("time:", timeText(statistics.elapsedSeconds), leftEdge,
                    positionY, textColor);
  positionY += lineHeight;
  drawLabelledValue("points:", std::to_string(score), leftEdge, positionY,
                    highlightColor);
  positionY += lineHeight + 18;

  drawCenteredText(place > 0 ? "place " + std::to_string(place) +
                                   " on the score board"
                             : "not in the best ten this time",
                   centerX, positionY, textFontSize, dimTextColor);

  drawCenteredText("enter goes back to the menu", centerX,
                   static_cast<int>(area.y + area.height) - 36, smallFontSize,
                   dimTextColor);
}
