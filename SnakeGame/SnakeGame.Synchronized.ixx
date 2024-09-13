export module SnakeGame:Synchronized;

import std;

namespace SnakeGame {

export template <typename T>
class Synchronized {
    mutable std::mutex m {};
    T owned;

public:
    template <typename... Args>
    Synchronized(Args&&... args)
        : owned(std::forward<Args>(args)...)
    {
    }

    auto Lock(std::invocable<T&> auto&& callable)
    {
        std::scoped_lock l(m);
        return callable(owned);
    }

    auto Lock(std::invocable<T const&> auto&& callable) const
    {
        std::scoped_lock l(m);
        return callable(owned);
    }

    auto TryLock(std::invocable<T&> auto&& callable)
    {
        std::scoped_lock l(m, std::defer_lock);
        if (std::try_lock(l)) {
            return callable(owned);
        } else {
            return std::nullopt;
        }
    }

    auto TryLock(std::invocable<T const&> auto&& callable) const
    {
        std::scoped_lock l(m, std::defer_lock);
        if (std::try_lock(l)) {
            return callable(owned);
        } else {
            return std::nullopt;
        }
    }
};

}