#include <iostream>
#include <cstdlib>
#include <utility>

template <typename>
struct head;

template <typename T, T I, T... J>
struct head<std::integer_sequence<T, I, J...>> {
    static constexpr T value = I;
};

template <typename T>
struct head<std::integer_sequence<T>>{};

template <typename Seq>
static constexpr auto head_v = head<Seq>::value;

template <typename>
struct tail;

template <typename T, T I, T... J>
struct tail<std::integer_sequence<T, I, J...>> {
    using type = std::integer_sequence<T, J...>;
};

template <typename T, T I>
struct tail<std::integer_sequence<T, I>> {
    using type = std::integer_sequence<T>;
};

template <typename T>
struct tail<std::integer_sequence<T>> {};

template <typename Seq>
using tail_t = typename tail<Seq>::type;

template <typename Seq, auto I> 
struct push_back;

template <typename T, T... I, T J>
struct push_back<std::integer_sequence<T, I...>, J> {
    using type = std::integer_sequence<T, I..., J>;
};

template <typename Seq, auto I>
using push_back_t = typename push_back<Seq, I>::type;

template <typename, auto>
struct push_front;

template <typename T, T... I, T J>
struct push_front<std::integer_sequence<T, I...>, J> {
    using type = std::integer_sequence<T, J, I...>;
};

template <typename Seq, auto I>
using push_front_t = typename push_front<Seq, I>::type;

namespace detail {
    template <typename T>
    struct identity {
        using type = T;
    };

    template <typename Seq, auto I, template <typename Seq::value_type, typename Seq::value_type> class Cmp>
    struct insert;

    template <typename T, T... J, T I, template <T, T> class Cmp>
    struct insert<std::integer_sequence<T, J...>, I, Cmp> {
    private:
        using target = std::integer_sequence<T, J...>;
        static constexpr T target_head = head_v<target>;
        // the reason to use identity is in order not to instantiate false branch recursively.
        using new_tail =
            typename std::conditional_t<
                Cmp<I, target_head>::value,
                identity<target>,
                insert<tail_t<target>, I, Cmp>
            >::type;
        static constexpr T new_head = Cmp<I, target_head>::value ? I : target_head;
    public:
        using type = push_front_t<new_tail, new_head>;
    };

    template <typename T, T I, template <T, T> class Cmp>
    struct insert<std::integer_sequence<T>, I, Cmp> {
        using type = std::integer_sequence<T, I>;
    };
}

template <typename Seq, template <typename Seq::value_type, typename Seq::value_type> class>
struct sort;

template <typename T, T I, T... J, template <T, T> class Compare>
struct sort<std::integer_sequence<T, I, J...>, Compare> {
private:
    using sorted_tail = typename sort<std::integer_sequence<T, J...>, Compare>::type;
public:
    using type = typename detail::insert<sorted_tail, I, Compare>::type;
};

template <typename T, T I, template <T, T> class Compare>
struct sort<std::integer_sequence<T, I>, Compare> {
    using type = std::integer_sequence<T, I>;
};

template <typename Seq, template <typename Seq::value_type, typename Seq::value_type> class Compare>
using sort_t = typename sort<Seq, Compare>::type;

template <int X, int Y>
struct less_meta {
    static constexpr auto value = X < Y;
};

template <int X, int Y>
struct greater_meta {
    static constexpr auto value = X > Y;
};

int main()
{
    static_assert(
        std::is_same_v<
            sort_t<std::integer_sequence<int, 2, 3, 1, 4, 6, 1>, less_meta>,
            std::integer_sequence<int, 1, 1, 2, 3, 4, 6>
        >
    );
    
    static_assert(
        std::is_same_v<
            sort_t<std::integer_sequence<int, 2, 3, 1, 4, 6, 1>, greater_meta>,
            std::integer_sequence<int, 6, 4, 3, 2, 1, 1>
        >
    );
}

