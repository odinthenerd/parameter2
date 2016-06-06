/*==================================================================================================
Copyright (c) 2016 Odin Holmes and Carlos van Rooijen
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
=================================================================================================**/

#pragma once
#include "brigand\algorithms.hpp"


namespace parameter2 {
	namespace detail {
		template<typename T>
		struct make_default_value {
			T operator()() {
				return{};
			}
		};
		struct use_constexpr_value {};
		///index is the arguements position, use for disambiguation of types
		///T is the parameter type, must be copy or move constructable
		///DefaultValueMaker is a functor which returns the default value
		template<unsigned N, typename T>
		struct tagged_parameter {
			T val_;
		};

		template<int index>
		struct tuple_getter {
			template<typename T>
			auto operator()(T& t) {
				return std::move(std::get<index>(t).val_);
			}
		};

		template<typename T>
		struct call_default_factory_functor {
			template<typename U>
			auto operator()(U&) {
				return T{}();
			}
		};

		template<typename Index, typename Maker>
		struct selector_impl {
			using type = tuple_getter <Index::value >;
		};
		template<typename Maker>
		struct selector_impl<brigand::no_such_type_, Maker> {
			using type = call_default_factory_functor<Maker>;
		};

		template<typename T, typename TupleContents>
		using selector = typename selector_impl<brigand::index_of<TupleContents, T>, typename T::maker>::type;

		template<typename PL,typename...Ts>
		struct parameter_tuple {
			std::tuple<Ts...> t;
			template<typename T>
			decltype(std::get<brigand::index_of<brigand::list<Ts...>, typename T::type>::value>(t)) operator[](T) {
				return std::get<brigand::index_of<brigand::list<Ts...>, typename T::type>::value>(t);
			}
		};


		template<typename...Ps>
		struct tuple_maker {
			template<typename...Ts>
			parameter_tuple<brigand::list<Ps...>, Ts...> operator()(Ts...args) {
				return{ std::tuple<Ts...>{args...} };
			}
		};
	}

	///index is the arguements position, use for disambiguation of types
	///T is the parameter type, must be copy or move constructable
	///DefaultValueMaker is a functor which returns the default value
	template<unsigned N, typename T, typename DefaultValueMaker = detail::make_default_value<T>>
	struct tag {
		using type = detail::tagged_parameter<N, T>;
		constexpr type operator=(T in) const {
			return{ std::move(in) };
		}

	};

	template<unsigned N, typename T>
	struct tag<N, T, detail::use_constexpr_value> {
		T default_;
		using type = detail::tagged_parameter<N, T>;
		constexpr type operator=(T in) const {
			return{ std::move(in) };
		}
	};


	template<unsigned N, typename T>
	constexpr tag<N,T, detail::use_constexpr_value> make_tag(T t) {
		return{ t };
	}

	template<typename...Ts>
	detail::tuple_maker<Ts...> make_tuple(Ts...) {
		return{};
	}
}


