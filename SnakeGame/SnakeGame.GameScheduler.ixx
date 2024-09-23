export module SnakeGame:GameScheduler;
import :Scheduler;

import std;

namespace SnakeGame {
export class GameScheduler : public IScheduler {
protected:
    ~GameScheduler() = default;

    void StepFixed()
    {
        static std::mt19937 g;
        static std::uniform_int_distribution d(0, 200);
        if (d(g) == 0)
            gameState++;
    }

private:
    int gameState = 0;
};
}