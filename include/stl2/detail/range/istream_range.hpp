#ifndef STL2_DETAIL_RANGE_ISTREAM_RANGE_HPP
#define STL2_DETAIL_RANGE_ISTREAM_RANGE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/istream_iterator.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <class T, class charT = char, class traits = char_traits<charT>,
			class Distance = ptrdiff_t>
		class istream_range {
		public:
			using value_type = T;
			using reference = value_type&;
			using const_reference = value_type const&;
			using pointer = value_type*;
			using const_pointer = value_type const*;
			using difference_type = Distance;
			using iterator = istream_iterator<value_type, charT, traits, Distance>;
			using sentinel = iterator; // or default_sentinel
			using const_iterator = istream_iterator<value_type, charT, traits, Distance>;
			using const_sentinel = const_iterator; // or default_sentinel

			istream_range() = default;

			/// @brief Constructs an istream_range out of a previously constructed
			///        istream_iterator.
			/// @note This constructor is useful for constructing from an istream_iterator
			///       that has not yet reached the _end-of-stream_ value (e.g. double-input
			///       transform can return a useful iterator for this constructor).
			///
			istream_range(iterator i)
				: iterator_{i}
			{}

			/// @brief Initialises the range with an input stream.
			/// @param s The input stream to be abstracted.
			/// @postcondition iterator_.in_stream == &s
			///
			istream_range(basic_istream<charT, traits>& s)
				: iterator_{s}
			{}

			iterator begin()
			{
				return iterator_;
			}

			const_iterator begin() const
			{
				return iterator_;
			}

			auto cbegin() const
			{
				return begin();
			}

			sentinel end()
			{
				return sentinel{};
			}

			const_sentinel end() const
			{
				return sentinel{};
			}

			auto cend() const
			{
				return end();
			}
		private:
			istream_iterator<T, charT, traits, Distance> iterator_; // exposition only
		};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_RANGE_ISTREAM_RANGE_HPP
