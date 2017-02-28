// cmcstl2 - A concept-enabled C++ standard library
//
// Algorithm adapted (stolen) from Range v3 library
//
//  Copyright Andrew Sutton 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_ALL_OF_HPP
#define STL2_DETAIL_ALGORITHM_ALL_OF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// all_of [alg.all_of]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, class Proj = identity,
		IndirectPredicate<projected<I, Proj>> Pred>
	bool all_of(I first, S last, Pred pred, Proj proj = Proj{})
	{
		if (first != last) {
			do {
				if (!__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
					return false;
				}
			} while (++first != last);
		}
		return true;
	}

	template <InputRange Rng, class Proj = identity,
		IndirectPredicate<projected<iterator_t<Rng>, Proj>> Pred>
	bool all_of(Rng&& rng, Pred pred, Proj proj = Proj{})
	{
		return __stl2::all_of(__stl2::begin(rng), __stl2::end(rng),
			__stl2::ref(pred), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
