#ifndef STL2_DETAIL_RANGE_COUNTED_RANGE_HPP
#define STL2_DETAIL_RANGE_COUNTED_RANGE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		///////////////////////////////////////////////////////////////////////////
		// __make_const_iterator [Exposition]
		//
		template <template <class...> class I, class V, class... Ts>
		struct __make_const_iterator<I<V, Ts...>> {
			using type = I<V const, Ts...>;
		};

		template <template <class...> class I, class V, class... Ts>
		struct __make_const_iterator<I<V*, Ts...>> {
			using type = I<std::remove_pointer_t<V> const*, Ts...>;
		};

		template <Iterator I>
		using __make_const_iterator_t = _t<__make_const_iterator<I>>;

		///////////////////////////////////////////////////////////////////////////
		// counted_range [Extension]
		//
		template <Iterator I>
		class counted_range {
		public:
			using value_type = value_type_t<I>;
			using reference = reference_t<I>;
			using const_reference = const_reference_t<I>;
			using difference_type = difference_type_t<I>;
			using iterator = counted_iterator<I>;
			using sentinel = default_sentinel;
			using const_iterator = counted_iterator<__make_const_iterator<I>>; // implementation-defined
			using const_sentinel = default_sentinel;

			constexpr counted_range() = default;

			constexpr counted_range(I x, difference_type n)
				: iterator_{std::move(x), n}
			{}

			template <ConvertibleTo<I> I2>
			constexpr counted_iterator(counted_iterator<I2> const& i)
				: iterator_{i}
			{}

			template <ConvertibleTo<I> I2>
			constexpr counted_iterator(counted_range<I2> const& rng)
				: iterator_{rng.iterator_}
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
				return {};
			}

			const_sentinel end() const
			{
				return {};
			}

			auto cend() const
			{
				return end();
			}
		private:
			counted_iterator<I> iterator_{}; // exposition only
		};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_RANGE_COUNTED_RANGE_HPP
