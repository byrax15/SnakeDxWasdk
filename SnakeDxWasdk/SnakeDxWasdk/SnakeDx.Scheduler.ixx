module;
#include "pch.h"
export module SnakeDx:Scheduler;

import std;
using namespace std::literals;

import SnakeGame;
import :Resources;
import :Token;

namespace SnakeDx {

export class Scheduler {
    SnakeGame::Synchronized<Resources> resources { token };

    std::jthread event_loop;

public:
    ~Scheduler() { event_loop.request_stop(); }
    Scheduler(Scheduler const&) = delete;
    Scheduler& operator=(Scheduler const&) = delete;

public:
    Scheduler(Token)
        : event_loop([&](std::stop_token const& t) {
            while (!t.stop_requested()) {
                if (!(*resources).Ready())
                    continue;
                resources.TryLock([&](SnakeDx::Resources& r) {
                    r.Draw();

                    static std::mt19937 g;
                    static std::uniform_int_distribution d { 0, 200 };
                    if (d(g) == 0)
                        std::shuffle(r.m_clearColor.begin(), r.m_clearColor.begin() + 3, g);
                });
                std::this_thread::sleep_for(1ms);
            }
        })
    {
    }

    auto& Resources() { return resources; }

private:
} scheduler(token);
}