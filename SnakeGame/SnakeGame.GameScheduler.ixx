module;
export module SnakeGame:GameScheduler;
import :Scheduler;

import std;
import <DirectXMath.h>;

namespace SnakeGame {

export class GameScheduler : public IScheduler {
protected:
    ~GameScheduler() = default;

    void StepFixed()
    {
        auto iter = std::views::zip(positions, velocities);
        std::for_each(std::execution::par, iter.begin(), iter.end(), [](auto&& i) {
            auto&& [p, v] = i;
            DirectX::XMStoreFloat4(&p, DirectX::XMVectorAdd(XMLoadFloat4(&p), XMLoadFloat4(&v)));
        });
    }

    using float4 = DirectX::XMFLOAT4;
    std::vector<float4> positions { Random(3, { -1, -1, -1, 1 }, { 1, 1, 1, 1 }) };
    std::vector<float4> velocities { Random(3, { -1, -1, -1, 1 }, { 1, 1, 1, 1 }) };

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