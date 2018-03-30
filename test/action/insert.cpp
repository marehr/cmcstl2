// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <set>
#include <vector>
#include <stl2/view/iota.hpp>
#include <stl2/view/take.hpp>
// #include <stl2/view/for_each.hpp>
#include <stl2/view/repeat_n.hpp>
#include <stl2/view/repeat.hpp>
#include <stl2/view/transform.hpp>
#include <stl2/action/insert.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

template<typename T>
struct vector_like : std::vector<T> {
    using std::vector<T>::vector;

    using typename std::vector<T>::size_type;

    size_type last_reservation{};
    size_type reservation_count{};

    void reserve(size_type n) {
      std::vector<T>::reserve(n);
      last_reservation = n;
      ++reservation_count;
    }
};

int main()
{
    namespace ranges = std::experimental::ranges;
    namespace view = ranges::view;
    namespace action = ranges::ext::action;

    {
        std::vector<int> v;
        auto i = action::insert(v, v.begin(), 42);
        CHECK(i == v.begin());
        ::check_equal(v, {42});
        action::insert(v, v.end(), {1,2,3});
        ::check_equal(v, {42,1,2,3});

#if 0
        action::insert(v, v.begin(), view::ints | view::take(3));
        ::check_equal(v, {0,1,2,42,1,2,3});
#else
        action::insert(v, v.begin(), view::iota(0) | view::take(3));
#endif // 0
        int rg[] = {9,8,7};
        action::insert(v, v.begin()+3, rg);
        ::check_equal(v, {0,1,2,9,8,7,42,1,2,3});
        action::insert(v, v.begin()+1, rg);
        ::check_equal(v, {0,9,8,7,1,2,9,8,7,42,1,2,3});
    }

    {
        std::set<int> s;
#if 0
        auto temporary_view = view::ints
                            |view::take(10)
                            |view::for_each([](int i){return yield_if(i%2==0,i);});
#else
        auto temporary_view = view::iota(0)
                            | view::take(10)
                            | view::transform([](int i) { return i % 2 == 0 ? i : i - 1; });
#endif // 0
        action::insert(s, temporary_view);
        ::check_equal(s, {0,2,4,6,8});
        auto j = action::insert(s, 10);
        CHECK(j.first == prev(s.end()));
        CHECK(j.second == true);
        ::check_equal(s, {0,2,4,6,8,10});

        action::insert(std::ref(s), 12);
        ::check_equal(s, {0,2,4,6,8,10,12});

      //   action::insert(ranges::ref(s), 14);
      //   ::check_equal(s, {0,2,4,6,8,10,12,14});
    }

   //  {
   //      const std::size_t N = 1024;
   //      vector_like<int> vl;
   //      insert(vl, vl.end(), view::iota(0, int{N}));
   //      CHECK(vl.reservation_count == 1u);
   //      CHECK(vl.last_reservation == N);
   //      auto r = view::iota(0, int{2 * N});
   //      insert(vl, vl.begin() + 42, begin(r), end(r));
   //      CHECK(vl.reservation_count == 2u);
   //      CHECK(vl.last_reservation == 3 * N);
   //      int i = 42;
   //      insert(vl, vl.end(), &i, &i + 1);
   //      CHECK(vl.reservation_count == 3u);
   //      CHECK(vl.last_reservation > 3 * N + 1);
   //  }

    return ::test_result();
}
