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
#ifndef STL2_DETAIL_ALGORITHM_EQUAL_RANGE_HPP
#define STL2_DETAIL_ALGORITHM_EQUAL_RANGE_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/lower_bound.hpp>
#include <stl2/detail/algorithm/upper_bound.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/range.hpp>

///////////////////////////////////////////////////////////////////////////
// equal_range [equal.range]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		template <ForwardIterator I, class T, class Proj = identity,
			IndirectStrictWeakOrder<const T*, projected<I, Proj>> Comp = less<>>
		ext::range<I> equal_range_n(I first, difference_type_t<I> dist, const T& value,
			Comp comp = Comp{}, Proj proj = Proj{})
		{
			if (0 < dist) {
				do {
					auto half = dist / 2;
					auto middle = __stl2::next(first, half);
					auto&& v = *middle;
					auto&& pv = __stl2::invoke(proj, __stl2::forward<decltype(v)>(v));
					if (__stl2::invoke(comp, pv, value)) {
						first = __stl2::move(middle);
						++first;
						dist -= half + 1;
					} else if (__stl2::invoke(comp, value, pv)) {
						dist = half;
					} else {
						return {
							ext::lower_bound_n(
								__stl2::move(first), half, value,
								__stl2::ref(comp), __stl2::ref(proj)),
							ext::upper_bound_n(__stl2::next(middle),
								dist - (half + 1), value,
								__stl2::ref(comp), __stl2::ref(proj))
						};
					}
				} while (0 != dist);
			}
			return {first, first};
		}
	}

	template <ForwardIterator I, SizedSentinel<I> S, class T, class Proj = identity,
		IndirectStrictWeakOrder<const T*, projected<I, Proj>> Comp = less<>>
	ext::range<I> equal_range(I first, S last, const T& value,
		Comp comp = Comp{}, Proj proj = Proj{})
	{
		auto len = __stl2::distance(first, __stl2::move(last));
		return ext::equal_range_n(__stl2::move(first), len, value,
			__stl2::ref(comp), __stl2::ref(proj));
	}

	template <ForwardRange Rng, class T, class Proj = identity,
		IndirectStrictWeakOrder<const T*, projected<iterator_t<Rng>, Proj>> Comp = less<>>
	ext::range<safe_iterator_t<Rng>> equal_range(Rng&& rng, const T& value,
		Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::equal_range(
			__stl2::begin(rng), __stl2::end(rng), value,
			__stl2::ref(comp), __stl2::ref(proj));
	}

	template <ForwardRange Rng, class T, class Proj = identity,
		IndirectStrictWeakOrder<const T*, projected<iterator_t<Rng>, Proj>> Comp = less<>>
   requires
		SizedRange<Rng>()
	ext::range<safe_iterator_t<Rng>> equal_range(Rng&& rng, const T& value,
		Comp comp = Comp{}, Proj proj = Proj{})
	{
		return ext::equal_range_n(
			__stl2::begin(rng), __stl2::size(rng), value,
			__stl2::ref(comp), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
