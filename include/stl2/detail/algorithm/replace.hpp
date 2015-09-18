// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// replace [alg.replace]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I, Sentinel<I> S, class T1, Semiregular T2, class Proj = identity>
    requires Writable<I, T2>() &&
      IndirectCallableRelation<equal_to<>, Projected<I, Proj>, const T1 *>()
  I replace(I first, S last, const T1& old_value, const T2& new_value, Proj proj_ = Proj{}) {
    auto&& proj = __stl2::as_function(proj_);
    for (; first != last; ++first) {
      if (proj(*first) == old_value) {
        *first = new_value;
      }
    }
    return first;
  }

  template <ForwardRange Rng, class T1, Semiregular T2, class Proj = identity>
    requires Writable<IteratorType<Rng>, T2>() &&
      IndirectCallableRelation<equal_to<>, Projected<IteratorType<Rng>, Proj>, const T1 *>()
  safe_iterator_t<Rng>
  replace(Rng&& rng, const T1& old_value, const T2& new_value, Proj&& proj = Proj{}) {
    return __stl2::replace(
      __stl2::begin(rng), __stl2::end(rng),
      old_value, new_value, __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif