export module SnakeGame:Synchronized;

import std;

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

    T const& operator*() const { return owned; }

    auto Lock(std::invocable<T&> auto&& callable)
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
            return false;
        }
    }
};

}