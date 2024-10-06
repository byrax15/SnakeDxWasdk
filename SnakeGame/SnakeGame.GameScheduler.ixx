module;
export module SnakeGame:GameScheduler;
import :Scheduler;

import std;
import <DirectXMath.h>;

namespace SnakeGame {
namespace {
    using namespace DirectX;
}

export struct GridSquare final {
    enum class Type {
        HEAD,
        TAIL,
        APPLE,
    };

    XMINT4 position;
    XMFLOAT4 color;
    Type type;

    static GridSquare MakeHead();
    static GridSquare MakeTail(XMINT4 position);
    static GridSquare MakeApple();
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
    for (size_t j = 0; j < NComponent; ++j) {
        distribs[j] = Distribution { (&lower.x)[j], (&upper.x)[j] };
    }

    std::vector<Vector> vectors(count);
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = 0; j < NComponent; ++j) {
            (&vectors[i].x)[j] = distribs[j](g);
        }
    }

    return vectors;
}

export template <typename Vector>
Vector Random(Vector lower, Vector upper)
{
    using TComponent = decltype(Vector::x);
    constexpr auto NComponent = sizeof(Vector) / sizeof(TComponent);
    using Distribution = std::conditional_t<std::is_integral_v<TComponent>,
        std::uniform_int_distribution<TComponent>,
        std::uniform_real_distribution<TComponent>>;

    static std::random_device g;
    std::array<Distribution, NComponent> distribs;
    for (size_t j = 0; j < NComponent; ++j) {
        distribs[j] = Distribution { (&lower.x)[j], (&upper.x)[j] };
    }

    Vector vector;
    for (size_t j = 0; j < NComponent; ++j) {
        (&vector.x)[j] = distribs[j](g);
    }

    return vector;
}

export class GameScheduler : public IScheduler {
public:
    static constexpr std::pair BOUNDS = { XMINT4 { -5, -5, 0, 0 }, XMINT4 { 4, 4, 0, 0 } };

    enum class Direction {
        NONE,
        UP,
        DOWN,
        LEFT,
        RIGHT,
    } direction
        = Direction::NONE;

protected:
    GameScheduler();
    ~GameScheduler();

    void StepFixed() override;

    std::vector<GridSquare> Squares() const;
    size_t SizeSquares() const;
    size_t SizeBytesSquares() const;

private:
    GridSquare head;
    std::vector<GridSquare> tail, apples;
};
}