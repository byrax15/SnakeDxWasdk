module;
#include "pch.h"
export module SnakeDx:CoroutineScheduler;

import std;
import :Token;

namespace SnakeDx {
namespace {
    using namespace std::chrono;
    using namespace std::literals;
}
namespace winrt {
    using namespace ::winrt;
    using namespace ::winrt::Windows::Foundation;
}

export class CoroutineScheduler {
public:
    using clock = steady_clock;
    using seconds = std::ratio<1>;
    using timestep = duration<float, seconds>;
    static constexpr std::uint64_t FREQUENCY { 50 };
    static constexpr timestep PERIOD { 1. / FREQUENCY };

    enum class Message {
        Continue,
        Success,
        Error,
    } message
        = Message::Continue;

protected:
    CoroutineScheduler()
    {
        loop = Start();
    }

    ~CoroutineScheduler()
    {
        message = Message::Success;
    }

    virtual winrt::fire_and_forget StepFixed() = 0;
    virtual winrt::fire_and_forget StepDelta(timestep const& delta) = 0;

private:
    winrt::apartment_context thread;
    winrt::IAsyncAction loop;
    timestep last = duration_cast<timestep>(clock::now().time_since_epoch());
    timestep next = duration_cast<timestep>(clock::now().time_since_epoch());

    winrt::IAsyncAction Start()
    {
        co_await winrt::resume_background();
        thread = {};

        while (message == Message::Continue) {
            const auto now
                = duration_cast<timestep>(clock::now().time_since_epoch());
            if (now >= next) {
                next += PERIOD;
                StepFixed();
            }

            StepDelta(now - last);
            last = now;
        }
        if (message == Message::Error){
            throw std::exception("Event Loop crash");
        }
    }
};
}