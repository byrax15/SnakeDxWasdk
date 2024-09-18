module;
export module SnakeGame:Synchronized;

import std;
import <chrono>;

namespace SnakeGame {

template <typename F, typename... Args>
using call_result_t = decltype(std::declval<F>()(std::declval<Args>()...));

template <typename F, typename... Args>
constexpr bool void_function_v = std::is_invocable_v<F, Args...> && std::is_void_v<call_result_t<F, Args...>>;

template <typename F, typename... Args>
constexpr bool non_void_function_v = std::is_invocable_v<F, Args...> && !std::is_void_v<call_result_t<F, Args...>>;

export template <typename T>
class Synchronized {
    std::mutex m {};
    T owned;

public:
    using value_type = T;

    template <typename F>
    using bool_result = std::enable_if_t<void_function_v<F, T&>, bool>;

    template <typename F>
    using optional_result = std::enable_if_t<non_void_function_v<F, T&>, std::optional<call_result_t<F, T&>>>;

    template <typename... Args>
    Synchronized(Args&&... args)
        : owned(std::forward<Args>(args)...)
    {
    }

    T& operator*() { return owned; }
    T* operator->() { return &owned; }

    decltype(auto) Lock(std::invocable<T&> auto&& callable)
    {
        std::scoped_lock l(m);
        return callable(owned);
    }

    template <typename F>
    bool_result<F> TryLock(F&& callable)
    {
        std::unique_lock l(m, std::defer_lock);
        if (l.try_lock()) {
            callable(owned);
            return true;
        } else {
            return false;
        }
    }

    template <typename F>
    optional_result<F> TryLock(F&& callable)
    {
        std::unique_lock l(m, std::defer_lock);
        if (l.try_lock()) {
            return callable(owned);
        } else {
            return std::nullopt;
        }
    }

    decltype(auto) TryLock(std::chrono::milliseconds backoff, unsigned int max_attempts, auto&& callable)
    {
        auto next_backoff = backoff;
        for (unsigned int i = 0; i < max_attempts; ++i) {
            if (auto result = TryLock(callable); result)
                return result;
            next_backoff *= backoff.count();
            std::this_thread::sleep_for(next_backoff);
        }
        return decltype(TryLock(callable)) {};
    }

    decltype(auto) Lock(std::chrono::milliseconds backoff, unsigned int attempts_before_lock, auto&& callable)
    {
        if (auto result = TryLock(backoff, attempts_before_lock, callable); result) {
            if constexpr (requires { result.value(); })
                return result.value();
            else
                return;
        } else {
            return Lock(callable);
        }
    }
};

}