#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"

struct Point {
    int x;
    int y;
};

enum class MovementDir {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Player {
    explicit Player(Point pos = {.x = 10, .y = 10}) :
            coords(pos), old_coords(coords) {};

    bool Moved() const;

    void ProcessInput(MovementDir dir, int **wall);

    void Draw(Image &screen, Image &level, Image &ppng);

    void SetCoords(int x, int y);

    Point GetCoords();

    Pixel color{.r = 255, .g = 255, .b = 0, .a = 255};
private:
    Point coords{.x = 10, .y = 10};
    Point old_coords{.x = 10, .y = 10};
    int move_speed = 1;

};

#endif //MAIN_PLAYER_H
