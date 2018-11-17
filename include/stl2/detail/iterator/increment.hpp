// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015-2016
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_INCREMENT_HPP
#define STL2_DETAIL_ITERATOR_INCREMENT_HPP

#include <cstddef>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/fundamental.hpp>

STL2_OPEN_NAMESPACE {
	////////////////////////////////////////////////////////////////////////////
	// iter_difference_t [iterator.assoc]
	// Not to spec:
	// * Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78173; it is
	//   necessary to guard the requires clause of the "fallback" specialization
	//   to prevent hard errors for pointers to incomplete types.
	//
	namespace detail {
		template <class T>
		STL2_CONCEPT_KEYWORD MemberDifferenceType =
			requires { typename T::difference_type; };
	}

	template <class> struct incrementable_traits {};

	template <ext::Object T>
	struct incrementable_traits<T*> {
		using type = std::ptrdiff_t;
	};

	template <class T>
	struct incrementable_traits<const T>
	: incrementable_traits<std::decay_t<T>> {};

	template <detail::MemberDifferenceType T>
	struct incrementable_traits<T> {
		using type = typename T::difference_type;
	};

	template <class T>
		requires !detail::MemberDifferenceType<T> &&
			_IsNot<T, is_pointer> && // Avoid GCC PR 78173 (See above)
			requires(const T& a, const T& b) {
				// Avoid gcc ICE (TODO file bug):
				a - b;
				requires Integral<decltype(a - b)>;
			 	// { a - b } -> Integral;
			}
	struct incrementable_traits<T>
	: make_signed<decltype(declval<const T>() - declval<const T>())> {};

	template <class T>
	using iter_difference_t = meta::_t<incrementable_traits<T>>;

	///////////////////////////////////////////////////////////////////////////
	// WeaklyIncrementable [weaklyincrementable.iterators]
	//
	template <class I>
	STL2_CONCEPT_KEYWORD WeaklyIncrementable =
		Semiregular<I> &&
		requires(I& i) {
			typename iter_difference_t<I>;
			{ ++i } -> Same<I&>&&;
			i++;
		};

	///////////////////////////////////////////////////////////////////////////
	// Incrementable [incrementable.iterators]
	//
	template <class I>
	STL2_CONCEPT_KEYWORD Incrementable =
		WeaklyIncrementable<I> &&
		EqualityComparable<I> &&
		requires(I& i) {
			{ i++ } -> Same<I>&&;
		};

	///////////////////////////////////////////////////////////////////////////
	// Decrementable [Extension]
	//
	namespace ext {
		template <class I>
		STL2_CONCEPT_KEYWORD Decrementable =
			Incrementable<I> &&
			requires(I& i) {
				{ --i } -> Same<I&>&&;
				{ i-- } -> Same<I>&&;
			};
			// Let a and b be objects of type I.
			// Axiom: &--a == &a
			// Axiom: bool(a == b) implies bool(a-- == b)
			// Axiom: bool(a == b) implies bool((a--, a) == --b)
			// Axiom: bool(a == b) implies bool(--(++a) == b)
			// Axiom: bool(a == b) implies bool(++(--a) == b)
	}

	///////////////////////////////////////////////////////////////////////////
	// RandomAccessIncrementable [Extension]
	//
	namespace ext {
		template <class I>
		STL2_CONCEPT_KEYWORD RandomAccessIncrementable =
			Decrementable<I> &&
			requires(I& i, const I& ci, const iter_difference_t<I> n) {
				{ i += n } -> Same<I&>&&;
				{ i -= n } -> Same<I&>&&;
				{ ci + n } -> Same<I>&&;
				{ n + ci } -> Same<I>&&;
				{ ci - n } -> Same<I>&&;
				{ ci - ci } -> iter_difference_t<I>;
			};
			// FIXME: Axioms
	}
} STL2_CLOSE_NAMESPACE

#endif
