// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter, Eric Niebler 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_ACTION_CONCEPTS_HPP
#define STL2_ACTION_CONCEPTS_HPP

#include <stl2/concepts.hpp>
#include <stl2/functional.hpp>
#include <meta/meta.hpp>

STL2_OPEN_NAMESPACE {
   namespace ext {
      template <class T>
      concept bool SemiContainer = ForwardRange<T> &&
         DefaultConstructible<__uncvref<T>> &&
         Movable<__uncvref<T>> &&
         not View<T>;

      template <class T>
      concept bool Container = SemiContainer<T> &&
         InputIterator<iterator_t<T>> &&
         Sentinel<sentinel_t<T>, iterator_t<T>> &&
         Constructible<T, iterator_t<T>, sentinel_t<T>>;

      template <class T>
      using size_type_t = typename T::size_type;

      template <class C>
      concept bool Reservable = Container<C> &&
         SizedRange<C> &&
         UnsignedIntegral<size_type_t<C>> &&
         requires(C&& c, C const&& cc, size_type_t<C>&& s) {
            {cc.capacity()} -> size_type_t<C>;
            {cc.max_size()} -> size_type_t<C>;
            {c.reserve(s)};
         };

      template <class C>
      concept bool ReverseableAndAssignable = Reservable<C> &&
         requires(C&& c, iterator_t<C> i, sentinel_t<C>&& s) {
            {c.assign(i, s)};
         };

      template <class C>
      concept bool RandomAccessReservable = Reservable<C> &&
         RandomAccessRange<C>;

      template <class C> constexpr bool __is_lvalue_container_like = Container<C>;
      Container{C} constexpr bool __is_lvalue_container_like<C&> = true;
      Container{C} constexpr bool __is_lvalue_container_like<std::reference_wrapper<C>> = true;

      template <class C>
      concept bool LvalueContainerLike = __is_lvalue_container_like<C>;
   } // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_ACTION_CONCEPTS_HPP
