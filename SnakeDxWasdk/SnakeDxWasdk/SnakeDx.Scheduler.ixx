module;
#include "pch.h"
export module SnakeDx:Scheduler;

import SnakeGame;

import :Resources;
import :Token;

namespace SnakeDx {

export class Scheduler {
    SnakeGame::Synchronized<Resources> resources { token };

public:
    Scheduler(Token) {}

    auto& Resources() { return resources; }

} scheduler(token);

}