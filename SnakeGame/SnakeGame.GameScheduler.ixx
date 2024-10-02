module;
export module SnakeGame:GameScheduler;
import :Scheduler;

import std;
import <DirectXMath.h>;

namespace SnakeGame {
namespace {
    using namespace DirectX;
}

export struct GridSquare {
    XMINT4 position;
    XMFLOAT4 color;
};

export template <typename Vector>
std::vector<Vector> Random(size_t count, Vector lower, Vector upper)
{
    using TComponent = decltype(Vector::x);
    constexpr auto NComponent = sizeof(Vector) / sizeof(TComponent);
    using Distribution = std::conditional_t<std::is_integral_v<TComponent>,
        std::uniform_int_distribution<TComponent>,
        std::uniform_real_distribution<TComponent>>;

    static std::random_device g;
    std::array<Distribution, NComponent> distribs;
    for (size_t i = 0; i < NComponent; ++i) {
        distribs[i] = Distribution { (&lower.x)[i], (&upper.x)[i] };
    }

    std::vector<Vector> vectors(count);
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = 0; j < NComponent; ++j) {
            (&vectors[i].x)[j] = distribs[j](g);
        }
    }

    return vectors;
}

export class GameScheduler : public IScheduler {
public:
protected:
    ~GameScheduler() = default;

    void StepFixed()
    {
    }

    std::vector<GridSquare> squares
        = std::views::zip_transform(
              [](auto&& position, auto&& color) {
                  return GridSquare { position, color };
              },
              Random<XMINT4>(100, { -5, -5, 0, 0 }, { 4, 4, 0, 0 }),
              Random<XMFLOAT4>(100, { 0, 0, 0, 1 }, { 1, 1, 1, 1 }))
        | std::ranges::to<std::vector>();

private:
};
}