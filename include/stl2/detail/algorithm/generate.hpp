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
#ifndef STL2_DETAIL_ALGORITHM_GENERATE_HPP
#define STL2_DETAIL_ALGORITHM_GENERATE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/function.hpp>

///////////////////////////////////////////////////////////////////////////
// generate [alg.generate]
//
STL2_OPEN_NAMESPACE {
	template <Invocable F, OutputIterator<result_of_t<F&()>> O, Sentinel<O> S>
	O generate(O first, S last, F gen)
	{
		for (; first != last; ++first) {
			*first = gen();
		}
		return first;
	}

	template <Invocable F, OutputRange<result_of_t<F&()>> Rng>
	safe_iterator_t<Rng>
	generate(Rng&& rng, F gen)
	{
		return __stl2::generate(__stl2::begin(rng), __stl2::end(rng),
			__stl2::ref(gen));
	}
} STL2_CLOSE_NAMESPACE

#endif
