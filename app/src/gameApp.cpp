
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
