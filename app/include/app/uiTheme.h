// Every colour, text size and distance of the interface in one place.
#pragma once

#include <raylib.h>

inline constexpr int defaultWindowWidth = 1100;
inline constexpr int defaultWindowHeight = 760;

// The smallest window still has to show the whole side panel.
inline constexpr int minimumWindowWidth = 820;
inline constexpr int minimumWindowHeight = 680;
inline constexpr int targetFramesPerSecond = 60;

inline constexpr float panelPadding = 18.0f;
inline constexpr float sidePanelWidth = 310.0f;
inline constexpr float topBarHeight = 64.0f;

inline constexpr int titleFontSize = 44;
inline constexpr int headingFontSize = 26;
inline constexpr int textFontSize = 20;
inline constexpr int smallFontSize = 16;

inline constexpr Color backgroundColor{18, 20, 28, 255};
inline constexpr Color panelColor{30, 34, 48, 255};
inline constexpr Color panelBorderColor{58, 64, 84, 255};

inline constexpr Color textColor{228, 232, 240, 255};
inline constexpr Color dimTextColor{150, 158, 176, 255};
inline constexpr Color highlightColor{250, 204, 92, 255};
inline constexpr Color warningColor{240, 120, 110, 255};
inline constexpr Color successColor{126, 214, 148, 255};

inline constexpr Color cellColor{44, 50, 68, 255};
inline constexpr Color visitedCellColor{60, 76, 110, 255};
inline constexpr Color wallColor{140, 150, 176, 255};
inline constexpr Color doorColor{236, 150, 60, 255};
inline constexpr Color keyColor{250, 220, 90, 255};
inline constexpr Color playerColor{96, 198, 250, 255};
inline constexpr Color exitColor{86, 190, 130, 255};
inline constexpr Color hintColor{120, 140, 240, 180};
