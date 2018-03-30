// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter, Eric Niebler 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_ACTION_ACTION_HPP
#define STL2_ACTION_ACTION_HPP

#include <stl2/action/concepts.hpp>
#include <stl2/concepts.hpp>
#include <meta/meta.hpp>

STL2_OPEN_NAMESPACE {
   namespace action {
      struct action_access {
         template <class Action>
         struct impl {
            template <class... Ts, class A = Action>
            static decltype(auto) bind(Ts&&... ts)
            {
               return A::bind(std::forward<Ts>(ts)...); // static_cast<Ts&&>(ts)... in range-v3
            }
         };
      };

      struct make_action_fn {
         template <class Fun>
         action<Fun> operator()(Fun fun) const
         {
            return {detail::move(fun)};
         }
      };

      inline constexpr auto make_action = make_action_fn{};

      namespace detail {
         template <class A, class Rng, class... Rest>
         concept bool __Action = // exposition only
            Range<Rng> &&
            Invocable<A const&, Rng, Rest...>;

         template <class A, class Rng>
         concept bool __ActionPipe = // exposition only
            Invocable<A&, Rng> &&
            Range<Rng> &&
            meta::not_<std::is_reference<Rng>>;
      } // namespace detail

      template <detail::__Action A>
      class action : detail::__pipeable<action<A>> {
      public:
         action() = default;

         action(A a)
            : action_(detail::move(a))
         {}

         // Calling directly requires things are passed by reference.
         template <class Rng, class... Rest>
         requires
            detail::__ActionPipe<Rng&, Rest...>
         decltype(auto) operator()(Rng& rng, Rest&&... rest) const
         {
            return __stl2::invoke(action_, rng, std::forward<Rest>());
         }

         // currying overload
         template <typename T, typename... Rest, detail::__Action Actn = A>
         decltype(auto) operator()(T&& t, Rest&&... rest)
         {
            return make_action(action_access::impl<Actn>::bind(action_, std::forward<T>(t), std::forward<Rest>(rest)...));
         }
      private:
         template <detail::__ActionPipe<A> Rng, typename Act>
         static decltype(auto) pipe(Rng&& rng, Act&& act)
         {
            __stl2::invoke(act.action_, detail::move(act));
         }

         A action_;
         friend pipeable_action;
      };

      template <typename Rng, detail::__Action A>
      requires
         Pipeable<A> &&
         Range<Rng&> &&
         Invocable<bitwise_or, ref_t<Rng&>, A&> &&
         Same<ref_t<Rng&>, invoke_result_t<bitwise_or, ref_t<Rng&>&&, A&>>
      Rng& operator|=(Rng& rng, A&& action)
      {
         __stl2::ref(rng) | action;
         return rng;
      }
   } // namespace action
} STL2_CLOSE_NAMESPACE

#endif // STL2_ACTION_ACTION_HPP
