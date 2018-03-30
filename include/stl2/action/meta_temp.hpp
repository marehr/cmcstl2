#ifndef STL2_ACTION_META_TEMP_HPP
#define STL2_ACTION_META_TEMP_HPP

#include <meta/meta.hpp>
#include <stl2/concepts.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/view/view_closure.hpp>

STL2_OPEN_NAMESPACE {
   namespace ext {
      enum cardinality : std::ptrdiff_t {
         infinite = -3,
         unknown = -2,
         finite = -1
      };

      namespace detail {
         template <cardinality> struct basic_view : view_base {};

         std::integral_constant<cardinality, finite> test_cardinality(void *);

         template <cardinality Card>
         std::integral_constant<cardinality, Card> test_cardinality(basic_view<Card> *);

         template <typename T, std::size_t N>
         std::integral_constant<cardinality, static_cast<cardinality>(N)>
         test_cardinality(T(*)[N]);

         template <typename T, std::size_t N>
         std::integral_constant<cardinality, static_cast<cardinality>(N)>
         test_cardinality(std::array<T, N>*);
      }

      // User customization point for specifying the cardinality of ranges:
      template <Range Rng, typename Void = void>
      struct range_cardinality
         : meta::if_<std::is_same<Rng, __uncvref<Rng>>,
               decltype(detail::test_cardinality(static_cast<__uncvref<Rng>*>(nullptr))),
               range_cardinality<__uncvref<Rng>>>
      {};

      template <Range Rng>
      constexpr bool is_finite = meta::bool_<(range_cardinality<Rng>::value >= finite)>::value;

      template <Range Rng>
      constexpr bool is_infinite = meta::bool_<range_cardinality<Rng>::value == infinite>::value;

      namespace __unwrap_reference {
         struct fn : __stl2::detail::__pipeable<fn> {
            template <typename T>
            T&& operator()(T &&t) const noexcept
            {
               return static_cast<T&&>(t);
            }

            template<typename T>
            T& operator()(std::reference_wrapper<T> t) const noexcept
            {
               return t.get();
            }
         };
      } // namespace __unwrap_reference

      inline constexpr auto unwrap_reference = __unwrap_reference::fn{};
   } // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_ACTION_META_TEMP_HPP
