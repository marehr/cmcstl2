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
#ifndef STL2_DETAIL_RANGE_CONCEPTS_HPP
#define STL2_DETAIL_RANGE_CONCEPTS_HPP

#include <initializer_list>

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>

namespace std {
#ifndef __GLIBCXX__
#pragma message "These forward declarations will likely only work with libstdc++."
#endif
	template <class, class, class> class set;
	template <class, class, class> class multiset;
	template <class, class, class, class> class unordered_set;
	template <class, class, class, class> class unordered_multiset;
}

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Range [ranges.range]
	//
	template <class T>
	using iterator_t = __begin_t<T&>;

	template <class T>
	using sentinel_t = __end_t<T&>;

	template <class T>
	STL2_CONCEPT_KEYWORD _RangeImpl =
		requires(T&& t) {
			__stl2::begin(static_cast<T&&>(t)); // not necessarily equality-preserving
			__stl2::end(static_cast<T&&>(t));
		};

	template <class T>
	STL2_CONCEPT_KEYWORD Range = _RangeImpl<T&>;

	template <class T>
	STL2_CONCEPT_KEYWORD _ForwardingRange = Range<T> && _RangeImpl<T>;

	template <class R>
	STL2_CONCEPT_KEYWORD SizedRange =
		Range<R> &&
		!disable_sized_range<__uncvref<R>> &&
		requires(const remove_reference_t<R>& r) {
			{ __stl2::size(r) } -> Integral;
			{ __stl2::size(r) } -> iter_difference_t<iterator_t<R>>;
		};

	///////////////////////////////////////////////////////////////////////////
	// View [ranges.view]
	//
	struct view_base {};

	template <class T>
	STL2_CONCEPT_KEYWORD _ContainerLike =
		Range<T> && Range<const T> &&
		!Same<iter_reference_t<iterator_t<T>>, iter_reference_t<iterator_t<const T>>>;

	template <class T>
	struct enable_view {};

	template <class T>
	struct __view_predicate3 : std::true_type {};

	template <_ContainerLike T>
	struct __view_predicate3<T> : std::false_type {};

	template <class T>
	struct __view_predicate2 : __view_predicate3<T> {};

	template <DerivedFrom<view_base> T>
	struct __view_predicate2<T> : std::true_type {};

	template <class T>
	struct __view_predicate : __view_predicate2<T> {};

	template <class T>
		requires requires() { typename enable_view<T>::type ;}
	struct __view_predicate<T> : enable_view<T> {};

	template <class T>
	struct __view_predicate<std::initializer_list<T>> : std::false_type {};
	template <class Key, class Compare, class Alloc>
	struct __view_predicate<std::set<Key, Compare, Alloc>> : std::false_type {};
	template <class Key, class Compare, class Alloc>
	struct __view_predicate<std::multiset<Key, Compare, Alloc>> : std::false_type {};
	template <class Key, class Hash, class Pred, class Alloc>
	struct __view_predicate<std::unordered_set<Key, Hash, Pred, Alloc>> : std::false_type {};
	template <class Key, class Hash, class Pred, class Alloc>
	struct __view_predicate<std::unordered_multiset<Key, Hash, Pred, Alloc>> : std::false_type {};

	template <class T>
	STL2_CONCEPT_KEYWORD View =
		Range<T> &&
		Semiregular<T> &&
		__view_predicate<__uncvref<T>>::value;

	///////////////////////////////////////////////////////////////////////////
	// CommonRange
	//
	template <class T>
	STL2_CONCEPT_KEYWORD CommonRange =
		Range<T> && Same<iterator_t<T>, sentinel_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// OutputRange [ranges.output]
	//
	template <class R, class T>
	STL2_CONCEPT_KEYWORD OutputRange =
		Range<R> && OutputIterator<iterator_t<R>, T>;

	///////////////////////////////////////////////////////////////////////////
	// InputRange [ranges.input]
	//
	template <class T>
	STL2_CONCEPT_KEYWORD InputRange =
		Range<T> && InputIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// ForwardRange [ranges.forward]
	//
	template <class T>
	STL2_CONCEPT_KEYWORD ForwardRange =
		Range<T> && ForwardIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// BidirectionalRange [ranges.bidirectional]
	//
	template <class T>
	STL2_CONCEPT_KEYWORD BidirectionalRange =
		Range<T> && BidirectionalIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// RandomAccessRange [ranges.random.access]
	//
	template <class T>
	STL2_CONCEPT_KEYWORD RandomAccessRange =
		Range<T> && RandomAccessIterator<iterator_t<T>>;

	namespace ext {
		template <class R>
		STL2_CONCEPT_KEYWORD ContiguousRange =
			_Is<iter_reference_t<iterator_t<R>>, is_reference> &&
			Same<iter_value_t<iterator_t<R>>, __uncvref<iter_reference_t<iterator_t<R>>>> &&
			requires(R& r) {
				{ __stl2::data(r) } -> Same<add_pointer_t<iter_reference_t<iterator_t<R>>>>&&;
			};
	}

	namespace ext {
		template <class R>
		STL2_CONCEPT_KEYWORD SimpleView =
			View<R> && Range<const R> &&
			Same<iterator_t<R>, iterator_t<const R>> &&
			Same<sentinel_t<R>, sentinel_t<const R>>;
	}
	namespace ext {
		template<class Rng>
		STL2_CONCEPT_KEYWORD ViewableRange =
			Range<Rng> &&
			(_RangeImpl<Rng> || View<__f<Rng>>);
	}
} STL2_CLOSE_NAMESPACE

#endif
