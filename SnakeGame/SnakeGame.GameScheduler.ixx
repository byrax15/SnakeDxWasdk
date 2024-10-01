module;
export module SnakeGame:GameScheduler;
import :Scheduler;

import std;
import <DirectXMath.h>;

namespace SnakeGame {
namespace {
    using namespace DirectX;
}

export class GameScheduler : public IScheduler {
public:
    using float4 = DirectX::XMFLOAT4;

protected:
    ~GameScheduler() = default;

    void StepFixed()
    {
        // auto iter = std::views::zip(positions, velocities);
        // std::for_each(std::execution::par, iter.begin(), iter.end(), [](auto&& i) {
        //     auto&& [p, v] = i;
        //     XMStoreFloat4(&p, XMVectorMultiplyAdd(XMVectorReplicate(PERIOD.count()), XMLoadFloat4(&v), XMLoadFloat4(&p)));
        //     if (AABB_MIN.x > p.x || p.x > AABB_MAX.x)
        //         p.x *= -1;
        //     if (AABB_MIN.y > p.y || p.y > AABB_MAX.y)
        //         p.y *= -1;
        //     p.z = 1;
        //     p.w = 1;
        // });
    }

    static constexpr float4 AABB_MIN { -1, -1, -1, 1 };
    static constexpr float4 AABB_MAX { 1, 1, 1, 1 };
    // std::vector<float4> positions { Random(3, AABB_MIN, AABB_MAX) };
    // std::vector<float4> velocities { Random(3, { -1, -1, 0, 1 }, { 1, 1, 0, 1 }) };
    std::vector<float4> positions {
        { 0, 0, 0, 1 },
        { 1, 0, 0, 1 },
        { -1, 0, 0, 1 },
        { 0, 1, 0, 1 },
        { 0, -1, 0, 1 },
    };

private:
    static std::vector<float4> Random(size_t count, float4 lower, float4 upper)
    {
        static std::mt19937 g;
        std::uniform_real_distribution<float> x(lower.x, upper.x);
        std::uniform_real_distribution<float> y(lower.y, upper.y);
        std::uniform_real_distribution<float> z(lower.z, upper.z);
        std::uniform_real_distribution<float> w(lower.w, upper.w);
        std::vector<float4> v(count);
        for (auto& i : v)
            i = { x(g), y(g), z(g), w(g) };
        return v;
    }
};
}