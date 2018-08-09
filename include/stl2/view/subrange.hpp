// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_SUBRANGE_HPP
#define STL2_VIEW_SUBRANGE_HPP

#include <tuple>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/dangling.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/primitives.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	template <class T>
	STL2_CONCEPT_KEYWORD _PairLike =
		meta::Integral<std::tuple_size<T>> &&
		std::tuple_size<T>::value == 2 &&
		meta::Trait<std::tuple_element<0, T>> &&
		meta::Trait<std::tuple_element<1, T>> &&
		requires(T t) {
			{ std::get<0>(t) } -> const std::tuple_element_t<0, T>&;
			{ std::get<1>(t) } -> const std::tuple_element_t<1, T>&;
		};

	template <class T, class U, class V>
	STL2_CONCEPT_KEYWORD _PairLikeConvertibleTo =
		!Range<T> && _PairLike<__uncvref<T>> &&
		requires(T&& t) {
			{ std::get<0>(static_cast<T&&>(t)) } -> ConvertibleTo<U>;
			{ std::get<1>(static_cast<T&&>(t)) } -> ConvertibleTo<V>;
		};

	template <class T, class U, class V>
	STL2_CONCEPT_KEYWORD _PairLikeConvertibleFrom =
		!Range<T> && Same<T, __uncvref<T>> && _PairLike<T> &&
		Constructible<T, U, V>;

	template <class T>
	STL2_CONCEPT_KEYWORD _IteratorSentinelPair =
		!Range<T> && Same<T, __uncvref<T>> && _PairLike<T> &&
		Sentinel<std::tuple_element_t<1, T>, std::tuple_element_t<0, T>>;

	namespace ext {
		enum class subrange_kind : bool { unsized, sized };

		template <Iterator I, Sentinel<I> S = I,
			subrange_kind K = static_cast<subrange_kind>(SizedSentinel<S, I>)>
		requires K == subrange_kind::sized || !SizedSentinel<S, I>
		class subrange
		: public view_interface<subrange<I, S, K>>
		{
			static constexpr bool StoreSize =
				K == subrange_kind::sized && !SizedSentinel<S, I>;

			meta::if_c<StoreSize,
				std::tuple<I, S, iter_difference_t<I>>,
				std::tuple<I, S>> data_;

			constexpr I& first_() noexcept {
				return std::get<0>(data_);
			}
			constexpr const I& first_() const noexcept {
				return std::get<0>(data_);
			}
			constexpr S& last_() noexcept {
				return std::get<1>(data_);
			}
			constexpr const S& last_() const noexcept {
				return std::get<1>(data_);
			}
			constexpr iter_difference_t<I>& size_() noexcept requires StoreSize {
				return std::get<2>(data_);
			}
			constexpr const iter_difference_t<I>& size_() const noexcept requires StoreSize {
				return std::get<2>(data_);
			}
		public:
			using iterator = I;
			using sentinel = S;

			subrange() = default;

			constexpr subrange(I i, S s)
				requires !StoreSize
			: data_{std::move(i), std::move(s)} {}

			constexpr subrange(I i, S s, iter_difference_t<I> n)
				requires StoreSize
			: data_{std::move(i), std::move(s), n} {
				if constexpr (RandomAccessIterator<I>) {
					STL2_EXPECT(first_() + n == last_());
				}
			}
			constexpr subrange(I i, S s, iter_difference_t<I> n)
			requires SizedSentinel<S, I>
			: data_{std::move(i), std::move(s)} {
				STL2_EXPECT(last_() - first_() == n);
			}

			template <_NotSameAs<subrange> R>
			requires _ForwardingRange<R> &&
				ConvertibleTo<iterator_t<R>, I> && ConvertibleTo<sentinel_t<R>, S>
			constexpr subrange(R&& r) requires (!StoreSize)
			: subrange{__stl2::begin(r), __stl2::end(r)} {}

			template <_NotSameAs<subrange> R>
			requires _ForwardingRange<R> &&
				ConvertibleTo<iterator_t<R>, I> && ConvertibleTo<sentinel_t<R>, S>
			constexpr subrange(R&& r) requires StoreSize && SizedRange<R>
			: subrange{__stl2::begin(r), __stl2::end(r), __stl2::distance(r)} {}

			template <_ForwardingRange R>
			requires ConvertibleTo<iterator_t<R>, I> && ConvertibleTo<sentinel_t<R>, S>
			constexpr subrange(R&& r, iter_difference_t<I> n)
				requires (K == subrange_kind::sized)
			: subrange{__stl2::begin(r), __stl2::end(r), n} {
				if constexpr (SizedRange<R>) {
					STL2_EXPECT(n == __stl2::distance(r));
				}
			}

			template <_NotSameAs<subrange> PairLike>
			requires _PairLikeConvertibleTo<PairLike, I, S>
			constexpr subrange(PairLike&& r) requires (!StoreSize)
			: subrange{std::get<0>(static_cast<PairLike&&>(r)),
				std::get<1>(static_cast<PairLike&&>(r))}
			{}

			template <_PairLikeConvertibleTo<I, S> PairLike>
			constexpr subrange(PairLike&& r, iter_difference_t<I> n)
				requires (K == subrange_kind::sized)
			: subrange{std::get<0>(static_cast<PairLike&&>(r)),
				std::get<1>(static_cast<PairLike&&>(r)), n}
			{}

			template <_NotSameAs<subrange> PairLike>
				requires _PairLikeConvertibleFrom<PairLike, const I&, const S&>
			constexpr operator PairLike() const {
				return PairLike(first_(), last_());
			}

			constexpr I begin() const noexcept(std::is_nothrow_copy_constructible_v<I>) {
				return first_();
			}
			constexpr S end() const noexcept(std::is_nothrow_copy_constructible_v<S>) {
				return last_();
			}
			constexpr bool empty() const {
				return first_() == last_();
			}

			constexpr iter_difference_t<I> size() const requires K == subrange_kind::sized {
				if constexpr (StoreSize) {
					return size_();
				} else {
					return last_() - first_();
				}
			}

			[[nodiscard]] constexpr subrange next(iter_difference_t<I> n = 1) const {
				auto tmp = *this;
				tmp.advance(n);
				return tmp;
			}
			[[nodiscard]] constexpr subrange prev(iter_difference_t<I> n = 1) const
			requires BidirectionalIterator<I> {
				auto tmp = *this;
				tmp.advance(-n);
				return tmp;
			}
			constexpr subrange& advance(iter_difference_t<I> n) {
				auto remainder = __stl2::advance(first_(), n, last_());
				(void)remainder;
				if constexpr (StoreSize) {
					size_() -= n - remainder;
				}
				return *this;
			}
		};

		template <Iterator I, Sentinel<I> S>
		subrange(I, S, iter_difference_t<I>) -> subrange<I, S, subrange_kind::sized>;

		template <_IteratorSentinelPair P>
		subrange(P) ->
			subrange<std::tuple_element_t<0, P>, std::tuple_element_t<1, P>>;

		template <_IteratorSentinelPair P>
		subrange(P, iter_difference_t<std::tuple_element_t<0, P>>) ->
			subrange<std::tuple_element_t<0, P>, std::tuple_element_t<1, P>, subrange_kind::sized>;

		template <_ForwardingRange R>
		subrange(R&&) -> subrange<iterator_t<R>, sentinel_t<R>>;

		template <_ForwardingRange R>
		requires SizedRange<R>
		subrange(R&&) -> subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>;

		template <_ForwardingRange R>
		subrange(R&&, iter_difference_t<iterator_t<R>>) ->
			subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>;

		// Not to spec: Extension
		template <Iterator I, Sentinel<I> S = I>
		using sized_subrange = subrange<I, S, subrange_kind::sized>;

		template <std::size_t N, class I, class S, subrange_kind K>
		requires N < 2
		constexpr auto get(const subrange<I, S, K>& r) {
			if constexpr (N == 0) {
				return r.begin();
			} else {
				return r.end();
			}
		}

		// Not to spec: these should be hidden friends
		template <class I, class S, subrange_kind K>
		constexpr I begin(subrange<I, S, K>&& r)
		noexcept(std::is_nothrow_copy_constructible_v<I>) {
			return r.begin();
		}
		template <class I, class S, subrange_kind K>
		constexpr S end(subrange<I, S, K>&& r)
		noexcept(std::is_nothrow_copy_constructible_v<S>) {
			return r.end();
		}

		// Not to spec: should be constrained with _ForwardingRange, and never dangle.
		template <Range R>
		using safe_subrange_t =	__maybe_dangling<R, subrange<iterator_t<R>>>;
	} // namespace ext
} STL2_CLOSE_NAMESPACE

namespace std {
	template <class I, class S, ::__stl2::ext::subrange_kind K>
	struct tuple_size<::__stl2::ext::subrange<I, S, K>>
	  : std::integral_constant<size_t, 2> {};
	template <class I, class S, ::__stl2::ext::subrange_kind K>
	struct tuple_element<0, ::__stl2::ext::subrange<I, S, K>>
	{ using type = I; };
	template <class I, class S, ::__stl2::ext::subrange_kind K>
	struct tuple_element<1, ::__stl2::ext::subrange<I, S, K>>
	{ using type = S; };
}

#endif
