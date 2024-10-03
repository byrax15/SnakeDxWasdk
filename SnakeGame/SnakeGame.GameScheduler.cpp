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
    squares = {
        GridSquare { { 0, 0, 0, 0 }, { .2, 1, .2, 1 }, GridSquare::Type::HEAD },
        GridSquare { Random<XMINT4>(BOUNDS.first, BOUNDS.second), Random<XMFLOAT4>({ 1, 0, 0, 1 }, { 1, .4, 0, 1 }), GridSquare::Type::APPLE },
    };
}

GameScheduler::~GameScheduler()
    = default;

void GameScheduler::StepFixed()
{
    auto& head = Head();

    // Move head
    {
        auto& [position, _color, _type] = head;
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

    std::vector<GridSquare*> deletions(squares.size(), nullptr);
    std::atomic<bool> ate_apple = false;
    const auto head_pos = XMLoadSInt4(&head.position);
    auto iter = std::views::zip(squares, deletions) | std::views::drop(1);
    std::for_each(std::execution::par, iter.begin(), iter.end(), [&](auto&& tuple) {
        auto& [s, d] = tuple;
        switch (s.type) {
        case GridSquare::Type::APPLE:
            if (XMVector4EqualInt(head_pos, XMLoadSInt4(&s.position))) {
                d = &s;
                ate_apple = true;
            }
            break;
        }
    });

    for (auto i : deletions | std::views::reverse) {
        if (i == nullptr)
            continue;
        std::erase_if(squares, [&](auto&& j) { return &j == i; });
    }

    if (ate_apple) {
        squares.emplace_back(Random<XMINT4>(BOUNDS.first, BOUNDS.second), Random<XMFLOAT4>({ 1, 0, 0, 1 }, { 1, .4, 0, 1 }), GridSquare::Type::APPLE);
    }
}
}
