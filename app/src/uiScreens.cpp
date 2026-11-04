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
