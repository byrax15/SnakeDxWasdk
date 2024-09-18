module;
export module SnakeGame:Scheduler;

import std;
using namespace std::literals;
using namespace std::chrono_literals;
using namespace std::chrono;

import :Synchronized;

namespace SnakeGame {

export class Scheduler {
public:
    using clock = steady_clock;
    using seconds = std::ratio<1>;
    using timestep = duration<float, seconds>;
    static constexpr std::uint64_t FREQUENCY { 50 };
    static constexpr timestep PERIOD { 1. / FREQUENCY };

    Scheduler(Scheduler const&) = delete;
    Scheduler& operator=(Scheduler const&) = delete;

public:
    Scheduler()
        : event_loop([&](std::stop_token const& t) {
            while (!t.stop_requested()) {
                const auto now
                    = duration_cast<timestep>(clock::now().time_since_epoch());
                if (now >= next) {
                    next += PERIOD;
                    StepFixed();
                }

                StepDelta(now - last);
                last = now;
            }
        })
    {
    }

protected:
    ~Scheduler() { event_loop.request_stop(); }

    virtual void StepFixed()
    {
        std::cout << "Scheduler::StepFixed()" << '\n';
    }

    virtual void StepDelta(timestep const& delta)
    {
        std::cout << "Scheduler::StepDelta(" << delta << ")\n";
    }

private:
    std::jthread event_loop;
    timestep last = duration_cast<timestep>(clock::now().time_since_epoch());
    timestep next = duration_cast<timestep>(clock::now().time_since_epoch());
};

}