
export module ref_cast;

export template <typename T>
constexpr decltype(auto) ref_cast(T&& o)
{
    return static_cast<T&>(o);
}