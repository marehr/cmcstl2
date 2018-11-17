/// \file meta_fwd.hpp Forward declarations
//
// Meta library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef META_FWD_HPP
#define META_FWD_HPP

#include <utility>

#define META_CXX_STD_14 201402L

#ifndef STL2_CONCEPT_KEYWORD
# if defined __clang__
#  define STL2_CONCEPT_KEYWORD concept
# elif defined __GNUC__
#  define STL2_CONCEPT_KEYWORD concept bool
# else
#  error "Please use either GCC 7+ or the clang-concepts experimental branch."
# endif // __GNUC__
#endif // STL2_CONCEPT_KEYWORD

#if defined(_MSC_VER) && defined(_MSVC_LANG)
#define META_CXX_VER _MSVC_LANG
#define META_HAS_MAKE_INTEGER_SEQ 1
#else
#define META_CXX_VER __cplusplus
#endif

#ifndef META_CXX_VARIABLE_TEMPLATES
#ifdef __cpp_variable_templates
#define META_CXX_VARIABLE_TEMPLATES __cpp_variable_templates
#else
#define META_CXX_VARIABLE_TEMPLATES (META_CXX_VER >= META_CXX_STD_14)
#endif
#endif

#ifndef META_CXX_INTEGER_SEQUENCE
#ifdef __cpp_lib_integer_sequence
#define META_CXX_INTEGER_SEQUENCE __cpp_lib_integer_sequence
#else
#define META_CXX_INTEGER_SEQUENCE (META_CXX_VER >= META_CXX_STD_14)
#endif
#endif

#ifndef META_HAS_MAKE_INTEGER_SEQ
#ifdef __has_builtin
#if __has_builtin(__make_integer_seq)
#define META_HAS_MAKE_INTEGER_SEQ 1
#endif
#endif
#endif
#ifndef META_HAS_MAKE_INTEGER_SEQ
#define META_HAS_MAKE_INTEGER_SEQ 0
#endif

#ifndef META_HAS_TYPE_PACK_ELEMENT
#ifdef __has_builtin
#if __has_builtin(__type_pack_element)
#define META_HAS_TYPE_PACK_ELEMENT 1
#endif
#endif
#endif
#ifndef META_HAS_TYPE_PACK_ELEMENT
#define META_HAS_TYPE_PACK_ELEMENT 0
#endif

#if !defined(META_DEPRECATED) && !defined(META_DISABLE_DEPRECATED_WARNINGS)
#if defined(__cpp_attribute_deprecated) || META_CXX_VER >= META_CXX_STD_14
#define META_DEPRECATED(...) [[deprecated(__VA_ARGS__)]]
#elif defined(__clang__) || defined(__GNUC__)
#define META_DEPRECATED(...) __attribute__((deprecated(__VA_ARGS__)))
#endif
#endif
#ifndef META_DEPRECATED
#define META_DEPRECATED(...)
#endif

namespace meta
{
    inline namespace v1
    {
        namespace detail
        {
            template <bool B>
            using bool_ = std::integral_constant<bool, B>;
        }

        template <typename... Ts>
        struct list;

        template <template <typename...> class>
        struct quote;

        template <typename T, template <T...> class F>
        struct quote_i;

        template <template <typename...> class C, typename... Ts>
        struct defer;

        template <typename T, template <T...> class C, T... Is>
        struct defer_i;

        template <typename T>
        constexpr bool is_list_v = false;

        template <typename... Ts>
        constexpr bool is_list_v<list<Ts...>> = true;

        template <typename...>
        STL2_CONCEPT_KEYWORD True = true;

        template <typename T, typename U>
        STL2_CONCEPT_KEYWORD Same = detail::bool_<std::is_same<T, U>::value>::value;

        template <template <typename...> class C, typename... Ts>
        STL2_CONCEPT_KEYWORD Valid = requires
        {
            typename C<Ts...>;
        };

        template <typename T, template <T...> class C, T... Is>
        STL2_CONCEPT_KEYWORD Valid_I = requires
        {
            typename C<Is...>;
        };

        template <typename T>
        STL2_CONCEPT_KEYWORD Trait = requires
        {
            typename T::type;
        };

        template <typename T>
        STL2_CONCEPT_KEYWORD Invocable = requires
        {
            typename quote<T::template invoke>;
        };

        template <typename T>
        STL2_CONCEPT_KEYWORD List = is_list_v<T>;

        // clang-format off
        template <typename T>
        STL2_CONCEPT_KEYWORD Integral = requires
        {
            typename T::type;
            typename T::value_type;
            typename T::type::value_type;
            requires Same<typename T::value_type, typename T::type::value_type>;
            // \begin{BUGBUG} https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68434
            //{ T::value } -> Same<typename T::value_type>;
            //{ T::type::value } -> Same<typename T::value_type>;
            T::value;
            T::type::value;
            // requires Same<decltype(T::value), typename T::value_type>;
            // requires Same<decltype(T::type::value), typename T::value_type>;
            // \end{BUGBUG}
            { T {} } -> typename T::value_type;
            // Again: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68434
            //{ T{}() } -> Same<typename T::value_type>;
            T{}();
            requires Same<decltype(T{}()), typename T::value_type>;
        }
        && std::is_integral<typename T::value_type>::value
        && T::value == T::type::value
        && T{}() == T::value;

        template <typename T>
        struct id;

        template <typename... Fs>
        struct compose;

        namespace extension
        {
            template <typename F, typename List>
            struct apply;
        }

        using std::integer_sequence;
    } // inline namespace v1
} // namespace meta

#endif
