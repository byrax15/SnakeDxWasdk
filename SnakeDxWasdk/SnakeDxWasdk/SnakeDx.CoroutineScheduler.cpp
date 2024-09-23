module;
#include "pch.h"
module SnakeDx:CoroutineScheduler;

import std;

namespace SnakeDx {
namespace {
    using namespace std::chrono;
    using namespace std::literals;
    using namespace std::chrono_literals;
}

void CoroutineScheduler::StepFixed()
{
    GameScheduler::StepFixed();

    static std::mt19937 g;
    static std::uniform_int_distribution d { 0, 200 };
    if (d(g) == 0) {
        auto [m, r] = resources.ToRef();
        std::shuffle(r.m_clearColor.begin(), r.m_clearColor.begin() + 3, g);
    }
}

void CoroutineScheduler::StepDelta(timestep const& delta)
{
    GameScheduler::StepDelta(delta);
    {
        if (!resources->Ready())
            goto RENDER_DONE;
        auto [m, r] = resources.ToRef();
        std::unique_lock lock(m, std::try_to_lock);
        if (lock) {
            r.Draw(trianglePass);
            frameTime = delta;
        }
    }
    {
    RENDER_DONE:
        auto [m, dl] = deltaListeners.ToRef();
        std::scoped_lock lock(m);
        if (dl.empty())
            return;
        for (auto& l : dl) {
            l->operator()();
        }
    }
}

winrt::IAsyncAction CoroutineScheduler::Run()
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

        // TODO : Remove delay
        co_await winrt::resume_after(1ms);
    }
    if (message == Message::Error) {
        throw std::exception("Event Loop crash");
    }
}
}
