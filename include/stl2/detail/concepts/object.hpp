#ifndef STL2_DETAIL_CONCEPTS_OBJECT_HPP
#define STL2_DETAIL_CONCEPTS_OBJECT_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/type_traits.hpp>

////////////////////////////////////////
// Object Concepts [concepts.lib.object]
//
namespace stl2 { inline namespace v1 {

// 20150712: not to spec. is_object and accepting the
// parameters by reference are necessary to prevent hard
// errors in the requires clause with odd types.
template <class T>
concept bool Destructible() {
  return _Is<T, std::is_object> &&
    requires (T& t, const T& ct, T* const p) {
      { t.~T() } noexcept;
      STL2_EXACT_TYPE_CONSTRAINT(&t, T*); // not required to be equality preserving
      STL2_EXACT_TYPE_CONSTRAINT(&ct, const T*); // not required to be equality preserving
      delete p;
      delete[] p;
    };
}

namespace ext {
template <class T, class...Args>
concept bool ConstructibleObject =
  Destructible<T>() && requires (Args&&...args) {
    T{ (Args&&)args... }; // not required to be equality preserving
    new T{ (Args&&)args... }; // not required to be equality preserving
  };

// 20150718: Not to spec: spec is broken.
// FIXME: Correct wording.
template <class T, class...Args>
concept bool BindableReference =
  _Is<T, std::is_reference> &&
  // requires (Args&&...args) { T{ (Args&&)args... }; };
  // requires (Args&&...args) { T( (Args&&)args... ); };
  _Is<T, std::is_constructible, Args...>;
}

namespace detail {
template <class T, class...Args>
constexpr bool constructible_object_or_ref = false;

ext::ConstructibleObject{T, ...Args}
constexpr bool constructible_object_or_ref<T, Args...> = true;

ext::BindableReference{T, ...Args}
constexpr bool constructible_object_or_ref<T, Args...> = true;
}

template <class T, class...Args>
concept bool Constructible() {
  return detail::constructible_object_or_ref<T, Args...>;
}

// There's implementation variance around DR1518, this may not
// enforce the requirement that the default constructor be
// non-explicit.
template <class T>
concept bool DefaultConstructible() {
  return Constructible<T>() &&
    requires (const std::size_t n) {
      new T[n]{}; // not required to be equality preserving
    };
}

template <class T>
concept bool MoveConstructible() {
  return Constructible<T, std::remove_cv_t<T>&&>() &&
    ext::ImplicitlyConvertibleTo<std::remove_cv_t<T>&&, T>();
}

template <class T>
concept bool CopyConstructible() {
  return MoveConstructible<T>() &&
    Constructible<T, const std::remove_cv_t<T>&>() &&
    ext::ImplicitlyConvertibleTo<std::remove_cv_t<T>&, T>() &&
    ext::ImplicitlyConvertibleTo<const std::remove_cv_t<T>&, T>() &&
    ext::ImplicitlyConvertibleTo<const std::remove_cv_t<T>&&, T>();
}

template <class T>
concept bool Movable() {
  return MoveConstructible<T>() &&
    Assignable<T&, T&&>();
}

template <class T>
concept bool Copyable() {
  return CopyConstructible<T>() &&
    Movable<T>() &&
    Assignable<T&, const T&>();
}

template <class T>
concept bool Semiregular() {
  return Copyable<T>() &&
    DefaultConstructible<T>();
}

template <class T>
concept bool Regular() {
  return Semiregular<T>() &&
    EqualityComparable<T>();
}

namespace ext {
template <class T>
concept bool TriviallyDestructible() {
  return Destructible<T>() &&
    _Is<T, is_trivially_destructible>;
}

template <class T, class...Args>
concept bool TriviallyConstructible() {
  return Constructible<T, Args...>() &&
    _Is<T, is_trivially_constructible, Args...>;
}

template <class T>
concept bool TriviallyDefaultConstructible() {
  return DefaultConstructible<T>() &&
    _Is<T, is_trivially_default_constructible>;
}

template <class T>
concept bool TriviallyMoveConstructible() {
  return MoveConstructible<T>() &&
    _Is<T, is_trivially_move_constructible>;
}

template <class T>
concept bool TriviallyCopyConstructible() {
  return CopyConstructible<T>() &&
    TriviallyMoveConstructible<T>() &&
    _Is<T, is_trivially_copy_constructible>;
}

template <class T>
concept bool TriviallyMovable() {
  return Movable<T>() &&
    TriviallyMoveConstructible<T>() &&
    _Is<T, is_trivially_move_assignable>;
}

template <class T>
concept bool TriviallyCopyable() {
  return Copyable<T>() &&
    TriviallyMovable<T>() &&
    TriviallyCopyConstructible<T>() &&
    _Is<T, is_trivially_copy_assignable>;
}

namespace models {
template <class>
constexpr bool destructible() { return false; }
Destructible{T}
constexpr bool destructible() { return true; }

template <class T, class...Args>
constexpr bool constructible_object() { return false; }
ConstructibleObject{T, ...Args}
constexpr bool constructible_object() { return true; }

template <class T, class...Args>
constexpr bool bindable_reference() { return false; }
BindableReference{T, ...Args}
constexpr bool bindable_reference() { return true; }

template <class, class...>
constexpr bool constructible() { return false; }
Constructible{T, ...Args}
constexpr bool constructible() { return true; }

template <class>
constexpr bool default_constructible() { return false; }
DefaultConstructible{T}
constexpr bool default_constructible() { return true; }

template <class>
constexpr bool move_constructible() { return false; }
MoveConstructible{T}
constexpr bool move_constructible() { return true; }

template <class>
constexpr bool copy_constructible() { return false; }
CopyConstructible{T}
constexpr bool copy_constructible() { return true; }

template <class>
constexpr bool movable() { return false; }
Movable{T}
constexpr bool movable() { return true; }

template <class>
constexpr bool copyable() { return false; }
Copyable{T}
constexpr bool copyable() { return true; }

template <class>
constexpr bool semiregular() { return false; }
Semiregular{T}
constexpr bool semiregular() { return true; }

template <class>
constexpr bool regular() { return false; }
Regular{T}
constexpr bool regular() { return true; }
}}}} // namespace stl2::v1::ext::models

#endif
