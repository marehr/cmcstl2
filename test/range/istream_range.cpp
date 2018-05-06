#include <stl2/detail/range/istream_range.hpp>

#include "../simple_test.hpp"
#include <limits>
#include <sstream>
#include <stl2/algorithm.hpp>
#include <string>
#include <vector>

namespace ranges = __stl2;

class fake_vector : public std::vector<float> {
	using vector<float>::vector;
};

std::istream& operator>>(std::istream& in, fake_vector& f)
{
	f = {ranges::istream_iterator<float>{in}, ranges::istream_iterator<float>{}};
	return in;
}


template <typename T, typename... Ts>
void test_istream_range(std::string const& s, Ts&&... t)
{
	auto const control = [&]{
		auto in = std::istringstream{s};
		(void)(in >> ... >> t);
		auto result = std::vector<T>{};

		ranges::copy(ranges::istream_iterator<T>{in}, ranges::default_sentinel{},
			ranges::back_inserter(result));
		return result;
	}();

	auto const independent = [&s]{
		auto in = std::istringstream{s};
		auto result = std::vector<T>{};

		ranges::copy(ranges::ext::istream_range<T>{in}, ranges::back_inserter(result));
		return result;
	}();

	CHECK((control == independent));
}

template <typename T>
void test_integer()
{
	test_istream_range<T>("1 2 3 4");
	test_istream_range<T>(R"(-1
2
	-3												4)");
	test_istream_range<T>("a b c d");
	test_istream_range<T>("a b c d", std::hex);
	test_istream_range<T>("0xa 0xb 0xc 0xd");
	test_istream_range<T>("0xa 0xb -0xc 0xd", std::hex);

	test_istream_range<T>("1.0 2.0 3.0 0.54");
	test_istream_range<T>("");
	test_istream_range<T>("1 2 3 4 meow");

	test_istream_range<T>("65536 65537 65538");
}

void test_vector_float()
{
	test_istream_range<fake_vector>("");
	test_istream_range<fake_vector>("1 2 3 4 5");
	test_istream_range<fake_vector>("1.1 2.2 3 4.4 5.");
	test_istream_range<fake_vector>("1.1f 2.2f 3f 4.4f 5.f");
	test_istream_range<fake_vector>("a b c d");
	test_istream_range<fake_vector>("0.123p-1 5.123p+4");
	test_istream_range<fake_vector>("0.123p-1 5.123p+4", std::hex);
	test_istream_range<fake_vector>("0x0.123p-1 0x5.123p+4");
	test_istream_range<fake_vector>("0x0.123p-1 0x5.123p+4", std::hex);
	test_istream_range<fake_vector>(
		std::to_string(std::numeric_limits<long long>::max()) + " 0");
}

int main()
{
	test_integer<int>();
	test_integer<unsigned int>();
	test_integer<signed char>();

	test_vector_float();
}
