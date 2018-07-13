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
#ifndef STL2_DETAIL_NUMERIC_ACCUMULATE_HPP
#define STL2_DETAIL_NUMERIC_ACCUMULATE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algebraic/groups.hpp>

STL2_OPEN_NAMESPACE {
   template <class F, class T, class I>
   concept bool __Accumulable = IndirectMagma<F, T const*, I> &&
      Assignable<T&, indirect_invoke_result_t<F, rvalue_reference_t<T*>, reference_t<I>>> &&
      Common<T, value_type_t<I>> &&
      Common<T, indirect_invoke_result_t<F, T const*, I>> &&
      Common<value_type_t<I>, indirect_invoke_result_t<F, T const*, I>>;

   template <Copyable T, InputIterator I>
   requires CommonReference<T, reference_t<I>>
   struct accumulate_result {
      T result;
      I first;
   };

   template <InputIterator I, Copyable T, class Proj = identity,
      __Accumulable<projected<I, Proj>, T> BOp = std::plus<>>
   accumulate_result<I, T>
   accumulate(I first, S last, T init, BOp binary_op = BOp{}, Proj proj = Proj{})
   {
      for (; first != last; ++first) {
         init = __stl2::invoke(binary_op, std::move(init), __stl2::invoke(proj, *first));
      }
      return {std::move(first), std::move(init)};
   }

   template <InputRange Rng, Copyable T, class Proj = identity,
      __Accumulable<projected<iterator_t<Rng>, Proj>, T> BOp = std::plus<>>
   accumulate_result<safe_iterator_t<Rng>, T>
   accumulate(Rng&& rng, T init, BOp binary_op = BOp{}, Proj proj = Proj{})
   {
      return __stl2::accumulate(__stl2::begin(rng), __stl2::end(rng), std::move(init),
         binary_op, proj);
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_ACCUMULATE_HPP
