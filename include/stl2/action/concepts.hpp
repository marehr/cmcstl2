#include <stl2/concepts.hpp>
#include <stl2/functional.hpp>
#include <meta/meta.hpp>

STL2_OPEN_NAMESPACE {
	template <class T>
	concept bool SemiContainer = ForwardRange<T> &&
		DefaultConstructible<__uncvref<T>> &&
		Movable<__uncvref<T>> &&
		not View<T>;

	template <class T>
	concept bool Container = SemiContainer<T> &&
		InputIterator<iterator_t<T>> &&
		Sentinel<sentinel_t<T>, iterator_t<T>> &&
		Constructible<T, iterator_t<T>, sentinel_t<T>>;

	template <class T>
	using size_type_t = class T::size_type;

	template <class C>
	concept bool Reservable = Container<C> &&
		SizedRange<C> &&
		UnsignedIntegral<size_type_t<C>> &&
		requires(C&& c, C const&& cc, size_type_t<C>&& s) {
			{cc.capacity()} -> SizeT;
			{cc.max_size()} -> SizeT;
			{c.reserve(s)};
		};

	template <class C>
	concept bool ReverseableAndAssignable = Reservable<C> &&
		Reservable<C> &&
		requires(C&& c, iterator_t<C> i, sentinel_t<C>&& s) {
			{c.assign(i, s)};
		};

	template <class C>
	concept bool RandomAccessReservable = Reservable<C> &&
		RandomAccessRange<C>;
} STL2_CLOSE_NAMESPACE
