// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_TYPE_TRAITS
#define STL2_TYPE_TRAITS

#include <type_traits>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// common_type
	//
	template <class T>
	struct __unary {
		template <class U>
		using apply = meta::invoke<T, U>;
	};

	template <class T>
	using __ref = std::add_lvalue_reference_t<T>;

	template <class T, class X = std::remove_reference_t<T>>
	using __cref = __ref<std::add_const_t<X>>;

	template <class T, class U>
	using __cond = decltype(true ? declval<T(&)()>()() : declval<U(&)()>()());

	template <class From, class To>
	struct __copy_cv_ : meta::id<To> {};
	template <class From, class To>
	struct __copy_cv_<From const, To> : std::add_const<To> {};
	template <class From, class To>
	struct __copy_cv_<From volatile, To> : std::add_volatile<To> {};
	template <class From, class To>
	struct __copy_cv_<From const volatile, To> : std::add_cv<To> {};
	template <class From, class To>
	using __copy_cv = meta::_t<__copy_cv_<From, To>>;

	template <class T, class U>
	struct __builtin_common {};
	template <class T, class U>
	using __builtin_common_t = meta::_t<__builtin_common<T, U>>;
	template <class T, class U>
	requires _Valid<__cond, __cref<T>, __cref<U>>
	struct __builtin_common<T, U> : std::decay<__cond<__cref<T>, __cref<U>>> {};

	template <class T, class U, class R = __builtin_common_t<T &, U &>>
	using __rref_res = std::conditional_t<meta::_v<std::is_reference<R>>,
		std::remove_reference_t<R> &&, R>;

	template <class T, class U>
	requires
		_Valid<__builtin_common_t, T &, U &> &&
		ConvertibleTo<T &&, __rref_res<T, U>> &&
		ConvertibleTo<U &&, __rref_res<T, U>>
	struct __builtin_common<T &&, U &&> : meta::id<__rref_res<T, U>> {};

	template <class T, class U>
	using __lref_res = __cond<__copy_cv<T, U> &, __copy_cv<U, T> &>;

	template <class T, class U>
	struct __builtin_common<T &, U &> : meta::defer<__lref_res, T, U> {};

	template <class T, class U>
	requires
		_Valid<__builtin_common_t, T &, U const &> &&
		ConvertibleTo<U &&, __builtin_common_t<T &, U const &>>
	struct __builtin_common<T &, U &&> : __builtin_common<T &, U const &> {};
	template <class T, class U>
	struct __builtin_common<T &&, U &> : __builtin_common<U &, T &&> {};

	// common_type
	template <class...>
	struct common_type {};

	template <class... Ts>
	using common_type_t = meta::_t<common_type<Ts...>>;

	template <class T>
	struct common_type<T> : std::decay<T> {};

	template <class T, class U>
	struct __common_type2 : common_type<std::decay_t<T>, std::decay_t<U>> {};

	template <_Decayed T, _Decayed U>
	struct __common_type2<T, U> : __builtin_common<T, U> {};

	template <_Decayed T, _Decayed U>
		requires requires { typename std::common_type_t<T, U>; }
	struct __common_type2<T, U> : std::common_type<T, U> {};

	template <class T, class U>
	struct common_type<T, U> : __common_type2<T, U> {};

	template <class T, class U, class V, class...  W>
	requires _Valid<common_type_t, T, U>
	struct common_type<T, U, V, W...>
	: common_type<common_type_t<T, U>, V, W...> {};

	///////////////////////////////////////////////////////////////////////////
	// common_reference machinery
	//
	namespace __qual {
		using __rref = meta::quote<std::add_rvalue_reference_t>;
		using __lref = meta::quote<std::add_lvalue_reference_t>;
		template <class>
		struct __xref : meta::id<meta::compose<meta::quote<meta::_t>, meta::quote<meta::id>>> {};
		template <class T>
		struct __xref<T&> : meta::id<meta::compose<__lref, meta::_t<__xref<T>>>> {};
		template <class T>
		struct __xref<T&&> : meta::id<meta::compose<__rref, meta::_t<__xref<T>>>> {};
		template <class T>
		struct __xref<const T> : meta::id<meta::quote<std::add_const_t>> {};
		template <class T>
		struct __xref<volatile T> : meta::id<meta::quote<std::add_volatile_t>> {};
		template <class T>
		struct __xref<const volatile T> : meta::id<meta::quote<std::add_cv_t>> {};
	}

	template <class T, class U, template <class> class TQual, template <class> class UQual>
	struct basic_common_reference {};

	template <class T, class U>
	using __basic_common_reference =
		basic_common_reference<__uncvref<T>, __uncvref<U>,
			__unary<meta::_t<__qual::__xref<T>>>::template apply,
			__unary<meta::_t<__qual::__xref<U>>>::template apply>;

	// common_reference
	template <class...>
	struct common_reference {};

	template <class... Ts>
	using common_reference_t = meta::_t<common_reference<Ts...>>;

	template <class T>
	struct common_reference<T> : meta::id<T> {};

	template <class T, class U>
	struct __common_reference2_2_ : common_type<T, U> {};

	template <class T, class U>
	requires requires { typename __cond<T, U>; }
	struct __common_reference2_2_<T, U> : meta::id<__cond<T, U>> {};

	template <class T, class U>
	struct __common_reference2_1_ : __common_reference2_2_<T, U> {};

	template <class T, class U>
	requires _Valid<meta::_t, __basic_common_reference<T, U>>
	struct __common_reference2_1_<T, U> : __basic_common_reference<T, U> {};

	template <class T, class U>
	struct __common_reference2 : __common_reference2_1_<T, U> {};

	template <_Is<std::is_reference> T, _Is<std::is_reference> U>
	requires
		_Valid<__builtin_common_t, T, U> &&
		_Is<__builtin_common_t<T, U>, std::is_reference>
	struct __common_reference2<T, U> : __builtin_common<T, U> {};

	template <class T, class U>
	struct common_reference<T, U> : __common_reference2<T, U> {};

	template <class T, class U, class V, class... W>
	requires _Valid<common_reference_t, T, U>
	struct common_reference<T, U, V, W...>
	: common_reference<common_reference_t<T, U>, V, W...> {};

	///////////////////////////////////////////////////////////////////////////
	// CommonReference [concept.commonref]
	//
	template <class T, class U>
	STL2_CONCEPT_KEYWORD CommonReference =
		requires {
			typename common_reference_t<T, U>;
			typename common_reference_t<U, T>;
		} &&
		Same<common_reference_t<T, U>, common_reference_t<U, T>> &&
		ConvertibleTo<T, common_reference_t<T, U>> &&
		ConvertibleTo<U, common_reference_t<T, U>>;

	///////////////////////////////////////////////////////////////////////////
	// Common [concept.common]
	//
	template <class T, class U>
	STL2_CONCEPT_KEYWORD Common =
		requires {
			typename common_type_t<T, U>;
			typename common_type_t<U, T>;
		} &&
		Same<common_type_t<T, U>, common_type_t<U, T>> &&
		ConvertibleTo<T, common_type_t<T, U>> &&
		ConvertibleTo<U, common_type_t<T, U>> &&
		CommonReference<std::add_lvalue_reference_t<const T>,
			std::add_lvalue_reference_t<const U>> &&
		CommonReference<std::add_lvalue_reference_t<common_type_t<T, U>>,
			common_reference_t<std::add_lvalue_reference_t<const T>,
				std::add_lvalue_reference_t<const U>>>;
} STL2_CLOSE_NAMESPACE

#endif
