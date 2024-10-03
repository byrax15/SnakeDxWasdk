module;
export module SnakeGame:Scheduler;

import std;
using namespace std::literals;
using namespace std::chrono_literals;
using namespace std::chrono;

import :Synchronized;

namespace SnakeGame {

class IScheduler {
public:
    using clock = steady_clock;
    using seconds = std::ratio<1>;
    using timestep = duration<float, seconds>;
    static constexpr std::uint64_t FREQUENCY { 12 };
    static constexpr timestep PERIOD { 1. / FREQUENCY };

public:
protected:
    ~IScheduler() = default;
    virtual void StepFixed() { }
    virtual void StepDelta([[maybe_unused]] timestep const& delta) { }

private:
};

}