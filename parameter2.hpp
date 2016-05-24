#include "brigand\algorithms.hpp"


namespace named_parameters {
	template<typename...Ts>
	brigand::list<Ts...> list(Ts...) {
		return{};
	}

	template<typename T>
	struct make_default_value {
		T operator()() {
			return{};
		}
	};

	///index is the arguements position, use for disambiguation of types
	///T is the parameter type, must be copy or move constructable
	///DefaultValueMaker is a functor which returns the default value
	template<unsigned index, typename T, typename DefaultValueMaker>
	struct Tagged {
		T val_;
		using maker = DefaultValueMaker;
	};


	///index is the arguements position, use for disambiguation of types
	///T is the parameter type, must be copy or move constructable
	///DefaultValueMaker is a functor which returns the default value
	template<int index, typename T, typename DefaultValueMaker = make_default_value<T>>
	struct Tagger {
		using type = Tagged<index, T, DefaultValueMaker>;
		constexpr type operator=(T in) {
			return{ std::move(in) };
		}

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



	template<typename R, typename...Ps, typename...Ls, typename...Ts>
	void call(R(&f)(Ps...), brigand::list<Ls...>, Ts...args) {
		auto t = std::make_tuple(std::move(args)...);
		f(std::move(selector<typename Ls::type, brigand::list<Ts...>>{}(t))...);
	}
}


