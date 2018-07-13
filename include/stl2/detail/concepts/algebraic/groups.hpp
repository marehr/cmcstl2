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
#ifndef CMCSTL2_DETAIL_CONCEPTS_ALGEBRAIC_GROUP_HPP
#define CMCSTL2_DETAIL_CONCEPTS_ALGEBRAIC_GROUP_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/function.hpp>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE { namespace ext {
   template <class F, class T>
   concept bool Magma = RegularInvocable<F> &&
      Assignable<T&, invoke_result_t<F, T, T>> &&
      EqualityComparableWith<T, invoke_result_t<F, T, T>>;

   template <class F, class R, class T1, class T2>
   concept bool MagmaWith = (Same<T1, R> || Same<T2, R>) &&
      RegularInvocable<F, T1, T2> &&
      Assignable<R&, invoke_result_t<F, T1, T2>> &&
      Common<T1, T2> &&
      CommonReference<T1&, R&> &&
      CommonReference<T2&, R&> &&
      EqualityComparableWith<R, invoke_result_t<F, T1, T2>>;

   template <class F, class I1, class I2>
   concept bool IndirectMagma = IndirectRegularBinaryInvocable<F, I1, I2> &&
      Magma<F, reference_t<I1>, reference_t<I2>> &&
      Magma<F, reference_t<I1>, value_type_t<I2>> &&
      Magma<F, value_type_t<I1>, reference_t<I2>> &&
      Magma<F, value_type_t<I1>, value_type_t<I2>> &&
      Magma<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

   /// \brief A quasigroup is an algebraic structure resembling a group in the sense that 'division'
   ///        is always possible.
   ///
   /// Let `f1` and `f2` be callable objects of type `F`, and `t1`, `t2` be objects of type `T`.
   /// `Quasigroup<F, T>` is satisfied if, and only if:
   ///    -- `invoke(f1, t1, invoke(f2, t2, t1)) == t2`
   ///    -- `invoke(f2, invoke(f1, t1, t2), t1) == t2`
   ///    -- `invoke(f1, invoke(f2, t2, t1), t1) == t2`
   ///    -- `invoke(f2, invoke(f1, t2, t1), t1) == t2`
   ///
   /// Let t3 be an object of type T. If invoke(f1, t1, t2) == invoke(f1, t1, t3) and t1 is not an
   /// annihilating element for T, then t2 == t3.
   /// \tparam F
   /// \tparam T
   ///
   template <class F, class T>
   concept bool  Quasigroup = Magma<F, T>;

   template <class F, class I1, class I2>
   concept bool IndirectQuasigroup = IndirectRegularBinaryInvocable<F, I1, I2> &&
      Quasigroup<F, reference_t<I1>, reference_t<I2>> &&
      Quasigroup<F, reference_t<I1>, value_type_t<I2>> &&
      Quasigroup<F, value_type_t<I1>, reference_t<I2>> &&
      Quasigroup<F, value_type_t<I1>, value_type_t<I2>> &&
      Quasigroup<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

   /// \brief A quasigroup with an identity element is called a loop.
   ///
   /// There exists a unique value `id` of the type `T` that satisfies the _identity-element
   /// property_.
   ///
   /// Let `result1 = invoke(f1, t1, t2)` and `result2 = invoke(f2, t1, t2)`. When `t1 != id` and
   /// `t2 != id`:
   ///    -- `result1 != t1` and `result1 != t2`
   ///    -- `result2 != t1` and `result2 != t2`
   /// \tparam F
   /// \tparam T
   /// \note The distinction between `Quasigroup<F, T>` and `Loop<F, T>` is purely semantic.
   ///
   template <class F, class T>
   concept bool  Loop = Quasigroup<F, T>;

   template <class F, class I1, class I2>
   concept bool IndirectLoop = IndirectRegularBinaryInvocable<F, I1, I2> &&
      Loop<F, reference_t<I1>, reference_t<I2>> &&
      Loop<F, reference_t<I1>, value_type_t<I2>> &&
      Loop<F, value_type_t<I1>, reference_t<I2>> &&
      Loop<F, value_type_t<I1>, value_type_t<I2>> &&
      Loop<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

   /// \brief A semigroup is an associative magma.
   ///
   /// Let f be a callable object of type `F`, and `t1`, `t2`, and `t3` be objects of type `T`.
   /// `Semigroup<F, T>` is satisfied if, and only if, for all `t1`, `t2`, and `t3`,
   /// `invoke(f, invoke(f, t1, t2), t3) == invoke(f, t1, invoke(t2, t3)).`
   ///
   /// Semigroups introduce the notion of a zero the annhiliating element. The result of any
   /// operation with an annhiliating element is equivalent to 0.
   /// \tparam F
   /// \tparam T
   ///
   template <class F, class T>
   concept bool  Semigroup = Magma<F, T> &&
      requires {
         {T{0}} -> T; // NOTE: T must be constructible with the value `0`. While this is technically
                      // `Constructible<T, int>`, `T{i}` need not be valid, where `i` is a nonzero
                      // integer.
      };

   template <class F, class I1, class I2>
   concept bool IndirectSemigroup = IndirectRegularBinaryInvocable<F, I1, I2> &&
      Semigroup<F, reference_t<I1>, reference_t<I2>> &&
      Semigroup<F, reference_t<I1>, value_type_t<I2>> &&
      Semigroup<F, value_type_t<I1>, reference_t<I2>> &&
      Semigroup<F, value_type_t<I1>, value_type_t<I2>> &&
      Semigroup<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

   /// \brief A monoid is a semigroup with an identity element.
   ///
   /// There exists a unique value `id` of the type `T` that satisfies the _identity-element
   /// property_.
   ///
   /// Let `result1 = invoke(f1, t1, t2)` and `result2 = invoke(f2, t1, t2)`. When `t1 != id` and
   /// `t2 != id`:
   ///    -- `result1 != t1` and `result1 != t2`
   ///    -- `result2 != t1` and `result2 != t2`
   /// \tparam F
   /// \tparam T
   /// \note The distinction between `Semigroup<F, T>` and `Monoid<F, T>` is purely semantic.
   ///
   template <class F, class T>
   concept bool  Monoid = Semigroup<F, T>;

   template <class F, class I1, class I2>
   concept bool IndirectMonoid = IndirectRegularBinaryInvocable<F, I1, I2> &&
      Monoid<F, reference_t<I1>, reference_t<I2>> &&
      Monoid<F, reference_t<I1>, value_type_t<I2>> &&
      Monoid<F, value_type_t<I1>, reference_t<I2>> &&
      Monoid<F, value_type_t<I1>, value_type_t<I2>> &&
      Monoid<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

   /// \brief A group is a binary operation that has an identity element `id`, supports the notion
   ///        of associativity, the notion of invertiblity, and a zero annhiliating element.
   /// \tparam F
   /// \tparam T
   ///
   template <class F, class T>
   concept bool  Group = Monoid<F, T> &&
      Loop<F, T>;

   template <class F, class I1, class I2>
   concept bool IndirectGroup = IndirectRegularBinaryInvocable<F, I1, I2> &&
      Group<F, reference_t<I1>, reference_t<I2>> &&
      Group<F, reference_t<I1>, value_type_t<I2>> &&
      Group<F, value_type_t<I1>, reference_t<I2>> &&
      Group<F, value_type_t<I1>, value_type_t<I2>> &&
      Group<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

   /// \brief Refines group so that the binary operation supports commutativity.
   ///
   /// Let `f` be an object of type `F`, and `t1` and `t2` be objects of type `T`. Then
   ///       `invoke(f, t1, t2) == invoke(f, t2, t1)`.
   /// \tparam F
   /// \tparam T
   /// \note The inverse of `f` need not be commutative.
   /// \note The distinction between a `Group` and `CommutativeGroup` is purely semantic.
   ///
   template <class F, class T>
   concept bool  CommutativeGroup = Group<F, T>;

   template <class F, class I1, class I2>
   concept bool IndirectCommutativeGroup = IndirectRegularBinaryInvocable<F, I1, I2> &&
      CommutativeGroup<F, reference_t<I1>, reference_t<I2>> &&
      CommutativeGroup<F, reference_t<I1>, value_type_t<I2>> &&
      CommutativeGroup<F, value_type_t<I1>, reference_t<I2>> &&
      CommutativeGroup<F, value_type_t<I1>, value_type_t<I2>> &&
      CommutativeGroup<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;
}} STL2_CLOSE_NAMESPACE

#endif // CMCSTL2_DETAIL_CONCEPTS_ALGEBRAIC_GROUP_HPP
