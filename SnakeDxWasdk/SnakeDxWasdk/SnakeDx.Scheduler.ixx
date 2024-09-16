module;
#include "pch.h"
export module SnakeDx:Scheduler;

import SnakeGame;
import :Resources;
import :Token;

namespace SnakeDx {
export class Scheduler final : SnakeGame::Scheduler {

protected:
    // void StepFixed() override { }

    void StepDelta(timestep const&) override
    {
        if (!(*resources).Ready())
            return;
        resources.TryLock([&](SnakeDx::Resources& r) {
            r.Draw();

            static std::mt19937 g;
            static std::uniform_int_distribution d { 0, 200 };
            if (d(g) == 0)
                std::shuffle(r.m_clearColor.begin(), r.m_clearColor.begin() + 3, g);
        });
    }

public:
    Scheduler(Token) { }

    SnakeGame::Synchronized<Resources> resources { token };

} scheduler(token);
}