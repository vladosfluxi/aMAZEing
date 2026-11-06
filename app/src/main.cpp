// labyrinthCore (static)  - the rules of the maze
// labyrinthStorage (shared) - the JSON files
// this executable           - the window, the drawing and the keyboard
#include "app/gameApp.h"

#include <exception>
#include <iostream>

int main()
{
    try
    {
        GameApp game;
        game.run();
    }
    catch (const std::exception& error)
    {
        std::cerr << "The game had to stop: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
