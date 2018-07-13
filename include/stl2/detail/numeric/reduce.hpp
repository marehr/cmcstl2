// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_NUMERIC_REDUCE_HPP
#define STL2_DETAIL_NUMERIC_REDUCE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algebraic/groups.hpp>
#include <stl2/detail/numeric/reduce.hpp>

STL2_OPEN_NAMESPACE {
   template <class BOp, class T1, class T2>
   concept bool __Reducible = __Accumulable<BOp, T1, T2> &&
      requires(BOp& binary_op, T1 const& t1, T2 const& t2) {
         {__stl2::invoke(binary_op, t1, t1)} -> ConvertibleTo<T>;
         {__stl2::invoke(binary_op, t1, t2)} -> ConvertibleTo<T>;
         {__stl2::invoke(binary_op, t2, t1)} -> ConvertibleTo<T>;
         {__stl2::invoke(binary_op, t2, t2)} -> ConvertibleTo<T>;
      };

   template <Copyable T, InputIterator I>
   requires CommonReference<T, reference_t<I>>
   struct reduce_result {
      T result;
      I first;
   };

   template <InputIterator I, Copyable T, class Proj = identity,
      __Reducible<projected<I, Proj>, T> BOp = std::plus<>>
   reduce_result<I, T>
   reduce(I first, S last, T init, BOp binary_op = BOp{}, Proj proj = Proj{})
   {
      for (; first != last; ++first) {
         init = __stl2::invoke(binary_op, std::move(init), __stl2::invoke(proj, *first));
      }
      return {std::move(first), std::move(init)};
   }

   template <InputRange Rng, Copyable T, class Proj = identity,
      __Reducible<projected<iterator_t<Rng>, Proj>, T> BOp = std::plus<>>
   reduce_result<safe_iterator_t<Rng>, T>
   reduce(Rng&& rng, T init, BOp binary_op = BOp{}, Proj proj = Proj{})
   {
      return __stl2::reduce(__stl2::begin(rng), __stl2::end(rng), std::move(init),
         binary_op, proj);
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_REDUCE_HPP
