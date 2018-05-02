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
#ifndef STL2_DETAIL_RANGE_OSTREAM_RANGE
#define STL2_DETAIL_RANGE_OSTREAM_RANGE

#include <stl2/detail/iterator/ostream_iterator.hpp>

STL2_NAMESPACE_OPEN {
   namespace ext {
		template <class T, class charT = char, class traits = char_traits<charT>>
		class ostream_range {
		public:
			using value_type = T;
			using reference = value_type&;
			using difference_type = ptrdiff_t;
			using iterator = ostream_iterator<T, charT, traits>;
			using sentinel = default_sentinel;
			using char_type = charT;
			using traits_type = traits;
			using ostream_type = basic_ostream<charT, traits>;

			constexpr ostream_range() noexcept = default;

			constexpr ostream_range(ostream_type& s) noexcept
				: iter_{s}
			{}

			ostream_range(ostream_type& s, const charT* delimiter) noexcept
				: iter_{s, delimiter}
			{}

			ostream_range(const ostream_range& x) noexcept
				: iter_{x.iter_}
			{}

			ostream_range& operator=(const ostream_range& other)
			{
				auto temp = other;
				ranges::swap(*this, temp);
				return *this;
			}

			constexpr iterator begin() noexcept
			{
				return iter_;
			}

			constexpr iterator begin() const noexcept
			{
				return iter_;
			}

			constexpr iterator cbegin() const noexcept
			{
				return iter_;
			}

			constexpr sentinel end() noexcept
			{
				return sentinel{};
			}

			constexpr sentinel end() const noexcept
			{
				return sentinel{};
			}

			constexpr sentinel cend() const noexcept
			{
				return sentinel{};
			}
		private:
			ostream_iterator<T, charT, traits> iter_;
		};
   } // namespace ext
} STL2_NAMESPACE_CLOSE

#endif // STL2_DETAIL_RANGE_OSTREAM_RANGE
