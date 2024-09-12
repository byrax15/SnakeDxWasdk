module;
#include "pch.h"
export module SnakeDx:Scheduler;
import :Token;
import :Resources;
import :Synchronized;

namespace SnakeDx {

export class Scheduler {
public:
    Synchronized<Resources> dxResources { token };

    Scheduler(token_t) { }
    Scheduler(Scheduler const&) = delete;
    Scheduler& operator=(Scheduler const&) = delete;
} scheduler(token);

}