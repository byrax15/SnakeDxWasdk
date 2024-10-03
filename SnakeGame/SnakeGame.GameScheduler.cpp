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
    static std::random_device g;
    std::uniform_int_distribution d(0, 200);
    if (d(g) != 0)
        return;
    auto iter = std::views::zip(squares, Random<XMINT4>(squares.size(), { -1, -1, 0, 0 }, { 1, 1, 0, 0 }));
    std::for_each(std::execution::par_unseq, iter.begin(), iter.end(), [](auto&& i) {
        auto& [square, vel] = i;
        XMStoreSInt4(&square.position,
            XMVectorClamp(
                XMLoadSInt4(&square.position) + XMLoadSInt4(&vel),
                XMLoadSInt4(&BOUNDS.first),
                XMLoadSInt4(&BOUNDS.second)));
    });
}
}
