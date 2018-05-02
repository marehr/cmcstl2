// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2018
//  Copyright Eric Niebler 2018
//  Copyright Christopher Di Bella 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/range/ostream_range.hpp>

#include "../simple_test.hpp"
#include <sstream>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/iterator/ostream_iterator.hpp>
#include <string>
#include <utility>
#include <vector>

template <class charT = char, class traits = std::char_traits<charT>>
class streamer {
public:
	explicit streamer(int id, std::basic_string<charT, traits> name)
		: id_{id},
			name_{std::move(name)}
	{}

	friend std::basic_ostream<charT, traits> operator<<()
	{
		return o << "id: " << id_ << "\tstring: " << name_;
	}
private:
	int id_;
	std::basic_string<charT, traits> name_;
};

namespace ranges = __stl2;

namespace {
	template <class T>
	void test()
	{
		auto const control_ostream = []{
			auto result = std::ostringstream{};
			ranges::copy(input, ranges::ostream_iterator<T>{result});
			return result;
		}();

		auto independent_ostream = std::ostringstream{};
	}
} // namespace

int main()
{
	::test<int>();
	::test<double>();
	::test<streamer>();
	return ::test_result();
}
