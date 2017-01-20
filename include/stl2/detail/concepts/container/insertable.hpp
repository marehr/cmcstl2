// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015 -- 2017
//  Copyright Eric Niebler 2015 -- 2017
//  Copyright Christopher Di Bella 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONCEPTS_INSERTABLE_HPP
#define STL2_DETAIL_CONCEPTS_INSERTABLE_HPP
#include <stl2/memory.hpp>

STL2_OPEN_NAMESPACE {
   template <class, class, class>
   constexpr bool __minimal_insertable = false;
   template <class X, class T, class A>
   requires
      models::Same<value_type_t<X>, T> &&
      models::Same<allocator_type_t<X>, typename allocator_traits<A>::rebind_alloc<T>>
   constexpr bool __minimal_insertable = true;

   template <class X, class T, class A>
   concept bool DefaultInsertable() {
      return __minimal_insertable<X, T, A> &&
         requires(A m, T* p) {
            allocator_traits<A>::construct(m, p);
      };
   }

   namespace models {
      template <class, class, class>
      constexpr bool DefaultInsertable = false;
      __stl2::DefaultInsertable{X, T, A}
      constexpr bool DefaultInsertable<X, T, A> = true;
   } // namespace models

   template <class X, class T, class A>
   concept bool MoveInsertable() {
      return __minimal_insertable<X, T, A> &&
         requires(A m, T* p, T&& rv) {
            allocator_traits<A>::construct(m, p, rv);
      };
      // axiom: *p == previous_value(rv) && *p != rv && rv.~T()
   }

   namespace models {
      template <class, class, class>
      constexpr bool MoveInsertable = false;
      __stl2::MoveInsertable{X, T, A}
      constexpr bool MoveInsertable<X, T, A> = true;
   } // namespace models

   template <class X, class T, class A>
   concept bool CopyInsertable() {
      return models::MoveInsertable<X, T, A> &&
         requires(A m, T* p, const T& v) {
            allocator_traits<A>::construct(m, p, v);
      };
      // axiom: *p == v
   }

   namespace models {
      template <class, class, class>
      constexpr bool CopyInsertable = false;
      __stl2::CopyInsertable{X, T, A}
      constexpr bool CopyInsertable<X, T, A> = true;
   } // namespace modles

   template <class X, class T, class A, class... Args>
   concept bool EmplaceConstructible() {
      return __minimal_insertable<X, T, A> &&
         requires(A m, T* p, Args&&... args) {
            allocator_traits<A>::construct(m, p, __stl2::forward<Args>(args)...);
      };
   }

   namespace models {
      template <class, class, class, class...>
      constexpr bool EmplaceConstructible = false;
      __stl2::EmplaceConstructible{X, T, A, Args...}
      constexpr bool EmplaceConstructible<X, T, A, Args...> = true;
   } // namespace models

   template <class X, class T, class A>
   concept bool Erasable() {
      return __minimal_insertable<X, T, A> &&
         requires(A m, T* p) {
            allocator_traits<A>::destroy(m, p);
      };
   }
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_CONCEPTS_INSERTABLE_HPP