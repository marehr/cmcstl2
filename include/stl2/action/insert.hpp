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
#ifndef STL2_ACTION_INSERT_HPP
#define STL2_ACTION_INSERT_HPP

#include <initializer_list>
#include <stl2/action/concepts.hpp>
#include <stl2/concepts.hpp>
#include <stl2/detail/algorithm/max.hpp>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/utility.hpp>

#include "meta_temp.hpp"

STL2_OPEN_NAMESPACE {
   namespace ext {
      namespace action {
         namespace __insert {
            template <LvalueContainerLike Cont, typename T>
            requires
               Constructible<value_type_t<decay_t<Cont>>, T>
            decltype(auto) insert(Cont&& cont, T&& t)
            {
               return unwrap_reference(cont).insert(std::forward<T>(t));
            }

            template <LvalueContainerLike Cont, InputIterator I, Sentinel<I> S>
            requires
               not Range<S> // should there be a constructible constraint here?
            decltype(auto) insert(Cont&& cont, I i, S j)
            {
               using C = common_iterator<I, S>;
               return unwrap_reference(cont).insert(C{i}, C{j});
            }

            template <LvalueContainerLike Cont, Range Rng>
            decltype(auto) insert(Cont&& cont, Rng&& rng)
            {
               using C = __range_common_iterator<Rng>;
               return unwrap_reference(cont).insert(C{__stl2::begin(rng)}, C{__stl2::end(rng)});
            }

            template <LvalueContainerLike Cont, Iterator I, class T>
            requires
               Constructible<value_type_t<decay_t<I>>, T>
            decltype(auto) insert(Cont&& cont, I p, T&& t)
            {
               return unwrap_reference(cont).insert(p, std::forward<T>(t));
            }

            template <LvalueContainerLike Cont, Iterator I, Integral N, typename T>
            requires
               Constructible<value_type_t<decay_t<Cont>>, T>
            decltype(auto) insert(Cont&& cont, I p, N n, T&& t)
            {
               return unwrap_reference(cont).insert(p, std::forward<T>(t));
            }

            namespace detail {
               template <Container C, Iterator P>
               requires
                  RandomAccessReservable<C>
               iterator_t<C> insert_reserve_helper(C& c, P const p, size_type_t<C> const delta)
               {
                  auto const old_size = ranges::size(c);
                  auto const max_size = c.max_size();
                  // add CMCSTL2_EXPECT here?
                  auto const new_size = old_size + delta;
                  auto const old_capacity = c.capcity();
                  auto const index = p - ranges::begin(c);

                  if (old_capacity < new_size) {
                     auto const new_capacity = (old_capacity <= max_size / 3 * 2)
                        ? __stl2::max(old_capacity + old_capacity / 2, new_size)
                        : max_size;
                     c.reserve(new_capacity);
                  }

                  return __stl2::begin(c) + index;
               }

               template <LvalueContainerLike Cont, Iterator P, Iterator I, Sentinel<I> S>
               requires
                  not Range<S>
               decltype(auto) insert_impl(Cont&& cont, P p, I i, S j) // false_type removed
               {
                  using C = common_iterator<I, S>;
                  return unwrap_reference(cont).insert(p, C{i}, C{j});
               }

               template <LvalueContainerLike Cont, Iterator P, Iterator I, SizedSentinel<I> S>
               requires
                  RandomAccessReservable<Cont> &&
                  not Range<S>
               decltype(auto) insert(Cont&& cont_, P p, I i, S j) // true_type removed
               {
                  auto&& cont = unwrap_reference(cont_);
                  auto const delta = static_cast<size_type_t<Cont>>(j - i);
                  auto pos = insert_reserve_helper(pos, std::move(p), delta);

                  using C = common_iterator<I, S>;
                  return cont.insert(pos, C{std::move(i)}, C{std::move(j)});
               }

               template <LvalueContainerLike Cont, Iterator I, Range Rng>
               decltype(auto) insert_impl(Cont&& cont, I p, Rng&& rng) // false_type removed
               {
                  using C = __range_common_iterator<Rng>;
                  return unwrap_reference(cont).insert(p, C{ranges::begin(rng)}, C{ranges::end(rng)});
               }

               template <LvalueContainerLike Cont, Iterator I, SizedRange Rng>
               requires
                  RandomAccessReservable<Cont>
               decltype(auto) insert_impl(Cont&& cont_, I p, Rng&& rng) // true_type removed
               {
                  auto&& cont = unwrap_reference(cont_);
                  auto const delta = static_cast<size_type_t<Cont>>(ranges::size(rng));
                  auto pos = insert_reserve_helper(cont, std::move(p), delta);

                  using C = __range_common_iterator<Rng>;
                  return cont.insert(pos, C{ranges::begin(rng)}, C{ranges::end(rng)});
               }
            } // namesapce detail

            template <LvalueContainerLike Cont, Iterator P, Iterator I, Sentinel<I> S>
            requires
               not Range<S>
            decltype(auto) insert(Cont&& cont, P p, I i, S j)
            {
               return detail::insert_impl(std::forward<Cont>(cont), std::move(p),
                  std::move(i), std::move(j));
            }

            template <LvalueContainerLike Cont, Iterator I, Range Rng>
            decltype(auto) insert(Cont&& cont, I p, Rng&& rng)
            {
               return detail::insert_impl(std::forward<Cont>(cont), std::move(p),
                  std::forward<Rng>(rng));
            }

            struct fn {
               template <Range Rng, typename T>
               requires
                  Constructible<value_type_t<decay_t<Rng>>, T>
               decltype(auto) operator()(Rng&& rng, T&& t) const
               {
                  return insert(std::forward<Rng>(rng), std::forward<T>(t));
               }

               template <Range Rng1, Range Rng2>
               requires
                  not is_infinite<Rng1>
               decltype(auto) operator()(Rng1&& rng1, Rng2&& rng2) const
               {
                  return insert(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2));
               }

               template <Range Rng, typename T>
               decltype(auto) operator()(Rng&& rng1, initializer_list<T> rng2) const
               {
                  return insert(std::forward<Rng>(rng1), rng2);
               }

               template <Range Rng, Iterator I, Sentinel<I> S>
               requires
                  not Range<S>
               decltype(auto) operator()(Rng&& rng, I i, S j) const
               {
                  return insert(std::forward<Rng>(rng), std::move(i), std::move(j));
               }

               template <Range Rng, Iterator I, typename T>
               decltype(auto) operator()(Rng&& rng, I p, T&& t) const
               {
                  return insert(std::forward<Rng>(rng), std::move(p), std::forward<T>(t));
               }

               template <Range Rng1, Iterator I, Range Rng2>
               requires
                  not is_infinite<Rng1>
               decltype(auto) operator()(Rng1&& rng1, I p, Rng2&& rng2) const
               {
                  return insert(std::forward<Rng1>(rng1), std::move(p), std::forward<Rng2>(rng2));
               }

               template <Range Rng, Iterator I, class T>
               decltype(auto) operator()(Rng&& rng1, I p, initializer_list<T> rng2) const
               {
                  return insert(std::forward<Rng>(rng1), std::move(p), rng2);
               }

               template <Range Rng, Iterator I, Integral N, class T>
               requires
                  Constructible<value_type_t<decay_t<Rng>>, T>
               decltype(auto) operator()(Rng&& rng, I p, N n, T&& t) const
               {
                  return insert(std::forward<Rng>(rng), std::move(p), n, std::forward<T>(t));
               }

               template <Range Rng1, Iterator P, Iterator I, Sentinel<I> S>
               requires
                  not Range<S>
               decltype(auto) operator()(Rng1&& rng, P p, I i, S j) const
               {
                  return insert(std::forward<Rng1>(rng), std::move(p), std::move(i), std::move(j));
               }
            };
         } // namespace __insert

         inline constexpr __insert::fn insert;

         template <class Rng, class... Rest>
         concept bool InsertableRange = Range<Rng> &&
            requires(Rng&& rng, Rest&&... rest) {
               ((void)__stl2::ext::action::__insert::insert(std::forward<Rng>(rng),
                  std::forward<Rest>(rest)..., 42));
            };
      } // namespace action
   } // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_ACTION_INSERT_HPP
