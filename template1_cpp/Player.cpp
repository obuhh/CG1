#include <iostream>
#include "Player.h"


bool Player::Moved() const {
    if (coords.x == old_coords.x && coords.y == old_coords.y)
        return false;
    else
        return true;
}

Point Player::GetCoords() {
    return  coords;
}

void Player::SetCoords(int x, int y) {
    //old_coords = coords;
    coords.x = x;
    coords.y = y;
    old_coords = coords;
}

void Player::ProcessInput(MovementDir dir, int **wall) {
    int move_dist = move_speed * 4;
    bool move = true;
    switch (dir) {
        case MovementDir::UP:
            if (coords.y == 0)
                break;
            old_coords.y = coords.y;
            for (int i = coords.x; i < coords.x + tileSize; ++i)
                if (wall[coords.y - 1][i] == 1) {
                    move = false;
                    break;
                }
            if (move)
                coords.y -= move_dist;
            break;
        case MovementDir::DOWN:
            if (coords.y == 480)
                break;
            old_coords.y = coords.y;
            for (int i = coords.x; i < coords.x + tileSize; ++i)
                if (wall[coords.y + tileSize][i] == 1) {
                    move = false;
                    break;
                }
            if (move)
                coords.y += move_dist;
            break;
        case MovementDir::LEFT:
            if (coords.x == 0)
                break;
            old_coords.x = coords.x;
            for (int i = coords.y; i < coords.y + tileSize; ++i)
                if (wall[i][coords.x - 1] == 1) {
                    move = false;
                    break;
                }
            if (move)
                coords.x -= move_dist;
            break;
        case MovementDir::RIGHT:
            if (coords.x == 768)
                break;
            old_coords.x = coords.x;
            for (int i = coords.y; i < coords.y + tileSize; ++i)
                if (wall[i][coords.x + tileSize] == 1) {
                    move = false;
                    break;
                }
            if (move)
                coords.x += move_dist;
            break;
        default:
            break;
    }
}

void Player::Draw(Image &screen, Image &level, Image &ppng) {
    if (Moved()) {
        for (int y = old_coords.y; y <= old_coords.y + tileSize; ++y) {
            for (int x = old_coords.x; x <= old_coords.x + tileSize; ++x) {
                screen.PutPixel(x, y, level.GetPixel(x, y));
                //std::cout << x << ' ' << y << std::endl;
                /*std::cout << unsigned(level.GetIPixel(336, 432).r) << " " <<
                          unsigned(level.GetIPixel(336, 432).g) << " " <<
                          unsigned(level.GetIPixel(336, 432).b) << " " <<
                          unsigned(level.GetIPixel(336, 432).a) << " " << std::endl;*/
            }
        }
        old_coords = coords;
    }

    for (int y = coords.y; y < coords.y + tileSize; ++y) {
        for (int x = coords.x; x < coords.x + tileSize; ++x) {
            Pixel now = ppng.GetIPixel(x - coords.x, y - coords.y);
            if ((now.r + now.g + now.b + now.a) > 0) {
                screen.PutPixel(x, y, now);
            }
        }
    }
}