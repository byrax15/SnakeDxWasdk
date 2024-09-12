export module SnakeDx:Synchronized;

import <mutex>;
import <optional>;


namespace SnakeDx {
export template <typename T>
class Synchronized {
    std::mutex mutex;
    T data;

public:
    template <typename... TArgs>
    Synchronized(TArgs&&... args)
        : data(std::forward<TArgs>(args)...)
    {
    }

    auto WithLock(std::invocable<T&> auto&& callable)
    {
        std::scoped_lock l { mutex };
        return callable(data);
    }

    auto TryLock(std::invocable<T&> auto&& callable)
    {
        std::scoped_lock l { mutex, std::defer_lock };
        if (std::try_lock(l)) {
            return std::make_optional(callable(data));
        }
        return std::nullopt;
    }
};
}
