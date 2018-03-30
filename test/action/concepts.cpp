// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include "stl2/action/concepts.hpp"
#include "stl2/concepts.hpp"
#include <array>
#include <memory>
#include <vector>

int main()
{
	namespace ranges = std::experimental::ranges;

	int rgi[6];
	static_assert(ranges::Range<decltype(rgi)>);
	static_assert(not ranges::ext::SemiContainer<decltype(rgi)>);

	static_assert(ranges::ext::SemiContainer<std::array<int, 6>>);
	static_assert(not ranges::ext::Container<std::array<int, 6>>);

	static_assert(ranges::ext::Container<std::vector<int>>);

	// TODO(@cjdb, CaseyCarter/cmcstl2/#142) uncomment when resolved
	//  static_assert(ranges::Container<std::vector<std::unique_ptr<int>>>);
}