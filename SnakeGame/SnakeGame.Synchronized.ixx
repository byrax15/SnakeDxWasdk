module;
export module SnakeGame:Synchronized;

import std;
import <chrono>;

namespace SnakeGame {

export template <typename T>
class Synchronized {
    std::mutex m {};
    T value;

public:
    template <typename... Args>
    Synchronized(Args&&... args)
        : value(std::forward<Args>(args)...)
    {
    }

    auto& Mutex() { return m; }

    T& operator*() { return value; }
    T* operator->() { return &value; }
    auto& Value() { }

    struct Ref {
        std::mutex& m;
        T& value;
    };

    Ref ToRef()
    {
        return Ref { m, value };
    }
};

}