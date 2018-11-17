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
#ifndef STL2_DETAIL_RANGE_PRIMITIVES_HPP
#define STL2_DETAIL_RANGE_PRIMITIVES_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/iterator/dangling.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// Range primitives [range.primitives]
//
STL2_OPEN_NAMESPACE {
	// enumerate
	namespace ext {
		template<Range R>
		constexpr auto enumerate(R&& r)
		STL2_NOEXCEPT_RETURN(
			__stl2::ext::enumerate(__stl2::begin(r), __stl2::end(r))
		)

		template<SizedRange R>
		constexpr auto enumerate(R&& r)
		STL2_NOEXCEPT_RETURN(
			tagged_pair<tag::count(iter_difference_t<iterator_t<R>>),
				tag::end(safe_iterator_t<R>)>{
				__stl2::size(r),
				__stl2::next(__stl2::begin(r), __stl2::end(r))
			}
		)
	}

	template<Range R>
	constexpr iter_difference_t<iterator_t<R>> distance(R&& r)
	noexcept(noexcept(__stl2::distance(__stl2::begin(r), __stl2::end(r))))
	{
		const iter_difference_t<iterator_t<R>> n =
			__stl2::distance(__stl2::begin(r), __stl2::end(r));
		STL2_EXPECT(n >= 0);
		return n;
	}

	template<SizedRange R>
	constexpr iter_difference_t<iterator_t<R>> distance(R&& r)
	noexcept(noexcept(__stl2::size(r)))
	{
		const auto n =
			static_cast<iter_difference_t<iterator_t<R>>>(__stl2::size(r));
		STL2_EXPECT(n >= 0);
		return n;
	}
} STL2_CLOSE_NAMESPACE

#endif
