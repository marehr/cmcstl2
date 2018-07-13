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
   template <class BOp1, class BOp2, class T, class I1, class I2>
   concept __InnerProductable = // exposition only
      IndirectMagma<BOp1, T const*, I1> &&
      IndirectMagma<BOp2, I1, I2>

   template <InputIterator I1, InputIterator I2, MoveConstructible T>
   requires CommonReference<T, reference_t<I1>> &&
   CommonReference<T, reference_t<I2>>
   struct inner_product_result {
      I1 first1;
      I2 first2;
      T result;
   };

   template <InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2, Copyable T,
      class Proj1 = identity, class Proj2 = identity,
      class BOp1 = plus<>, class BOp2 = multiplies<>>
   requires
      __InnerProductable<BOp1, BOp2, T, projected<I1, Proj1>, projected<I2, Proj2>>
   inner_product_result<I1, I2, T>
   inner_product(I1 first1, S1 last1, I2 first2, S2 last2, T init, BOp1 bop1 = BOp1{},
      BOp2 bop2 = BOp2{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{});
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_REDUCE_HPP
