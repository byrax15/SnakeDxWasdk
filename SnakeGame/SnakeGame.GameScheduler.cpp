module;
module SnakeGame:GameScheduler;

import std;
import <DirectXMath.h>;

namespace SnakeGame {

namespace {
    using namespace DirectX;
}

GameScheduler::GameScheduler()
    : squares {
        GridSquare { { 0, 0, 0, 0 }, { 1, 0, 0, 1 } },
    }
{
}

GameScheduler::~GameScheduler() = default;

void GameScheduler::StepFixed()
{
    auto& [position, _color] = squares[0];
    switch (direction) {
    case Direction::UP:
        position.y += 1;
        break;
    case Direction::DOWN:
        position.y -= 1;
        break;
    case Direction::LEFT:
        position.x -= 1;
        break;
    case Direction::RIGHT:
        position.x += 1;
        break;
    }

    for (auto [c, l, u] : std::views::zip(std::span { &position.x, 2 }, std::span { &BOUNDS.first.x, 2 }, std::span { &BOUNDS.second.x, 2 })) {
        if (c < l) {
            c = u;
        } else if (u < c) {
            c = l;
        }
    }
}
}
