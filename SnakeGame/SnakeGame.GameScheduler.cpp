module;
module SnakeGame:GameScheduler;

import std;
import <DirectXMath.h>;

namespace SnakeGame {

namespace {
    using namespace DirectX;
}

GameScheduler::GameScheduler()
{
    squares = std::deque<GridSquare> {
        GridSquare { { 0, 0, 0, 0 }, { .2, 1, .2, 1 }, GridSquare::Type::HEAD },
        GridSquare { Random<XMINT4>(BOUNDS.first, BOUNDS.second), Random<XMFLOAT4>({ 1, 0, 0, 1 }, { 1, .4, 0, 1 }), GridSquare::Type::APPLE },
    };
}

GameScheduler::~GameScheduler()
    = default;

void GameScheduler::StepFixed()
{
    std::for_each(std::execution::par, squares.begin(), squares.end(), [&](GridSquare& s) {
        auto& [position, _color, type] = s;
        switch (type) {
        case GridSquare::Type::HEAD:
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
            break;
        case GridSquare::Type::TAIL:
            break;
        case GridSquare::Type::APPLE:
            break;
        }
    });
}

}
