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
