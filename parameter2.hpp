/*==================================================================================================
Copyright (c) 2016 Odin Holmes and Carlos van Rooijen
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
=================================================================================================**/

#pragma once
#define BRIGAND_NO_BOOST_SUPPORT
#include "brigand\brigand.hpp"


namespace parameter2 {
	template<unsigned N, typename T, typename DefaultValueMaker>
	struct tag;
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
			using parameter_type = T;
			T val_;
		};

		template<typename T>
		struct make_default;
		template<unsigned N, typename T, typename DefaultValueMaker>
		struct make_default<tag<N, T, DefaultValueMaker>> {
			template<typename D>
			auto operator()(const D& defaults) { 
				return DefaultValueMaker{}();
			}
		};
		template<unsigned N, typename T>
		struct make_default<tag<N, T, use_constexpr_value>> {
			template<typename D>
			T operator()(const D& defaults) { 
				return std::get<brigand::index_of<brigand::wrap<D,brigand::list>, tag<N, T, use_constexpr_value>>::value>(defaults).default_;
			}
		};

		template<unsigned N, typename T, T V>
		struct make_default<tag<N, T, std::integral_constant<T,V>>> {
			template<typename D>
			T operator()(const D& ) {
				return V;
			}
		};

		template<typename T>
		struct make_defaults;
		template<typename...Ts>
		struct make_defaults<brigand::list<Ts...>>{
			template<typename D>
			std::tuple<typename Ts::parameter_type...> operator()(const D& defaults) {
				return std::tuple<typename Ts::parameter_type...>{ make_default<Ts>{}(defaults)... };
			}
		};

		template<typename T>
		struct get_value {
			using type = typename T::parameter_type;
		};

		template<typename T, typename...Ts>
		struct results_in_one_of {
			using type = brigand::any<brigand::list<Ts...>, std::is_same<brigand::_1, typename T::tagged_parameter_type>>;
		};

		template<typename T, typename...Ts>
		struct not_results_in_one_of {
			using type = brigand::none<brigand::list<Ts...>, std::is_same<brigand::_1, typename T::tagged_parameter_type>>;
		};


		//PL is a list of taks for which we have parameters
		//DL is a list of tags from which we need the defaults
		template<typename PL, typename DL, typename...Ts>
		struct parameter_tuple {
			
			std::tuple<typename Ts::parameter_type...> parameters;
			brigand::wrap<brigand::transform<DL,get_value<brigand::_1>>, std::tuple> defaults;
			//if we have to parameter
			template<typename T>
			decltype(std::get<brigand::index_of<PL, T>::value>(parameters)) operator[](T) {
				return std::get<brigand::index_of<PL, T>::value>(parameters);
			}
			//if we do not have the parameter use default
			template<typename T>
			decltype(std::get<brigand::index_of<DL, T>::value>(defaults)) operator[](T) {
				return std::get<brigand::index_of<DL, T>::value>(defaults);
			}
		};

		
		template<typename...Ps>
		struct tuple_maker {
			std::tuple<Ps...> defaults;
			template<typename...Ts>
			auto operator()(Ts...args) {
				using all_tags = brigand::list<Ps...>;
				using defaults_needed = brigand::remove_if<all_tags, results_in_one_of<brigand::_1, Ts...>>;
				using tags_with_parameters = brigand::remove_if<all_tags, not_results_in_one_of<brigand::_1, Ts...>>;
				return parameter_tuple<tags_with_parameters, defaults_needed, Ts...>{ std::tuple<typename Ts::parameter_type...>{args.val_...}, make_defaults<defaults_needed>{}(defaults) };
			}
		};
	}

	///index is the arguements position, use for disambiguation of types
	///T is the parameter type, must be copy or move constructable
	///DefaultValueMaker is a functor which returns the default value
	template<unsigned N, typename T, typename DefaultValueMaker = detail::make_default_value<T>>
	struct tag {
		using tagged_parameter_type = detail::tagged_parameter<N, T>;
		using parameter_type = T;
		constexpr tagged_parameter_type operator=(T in) const {
			return{ std::move(in) };
		}

	};

	template<unsigned N, typename T>
	struct tag<N, T, detail::use_constexpr_value> {
		T default_;
		using parameter_type = T;
		using tagged_parameter_type = detail::tagged_parameter<N, T>;
		constexpr tagged_parameter_type operator=(T in) const {
			return{ std::move(in) };
		}
	};


	template<unsigned N, typename T>
	constexpr tag<N,T, detail::use_constexpr_value> make_tag(T t) {
		return{ t };
	}

	template<typename...Ts>
	detail::tuple_maker<Ts...> make_tuple(const Ts...args) {
		return{ std::tuple<const Ts...>{args...} };
	}
}


