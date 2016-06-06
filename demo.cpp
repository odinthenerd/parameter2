#include "parameter2.hpp"

namespace p2 = parameter2;
struct DepthMaker
{
	int operator()() {
		return 42;
	}
};
constexpr auto length = p2::make_tag<1>(4);							//we can capture default value as a constexpr
constexpr p2::tag<2, int, std::integral_constant<int, 9>> height{};	//we can also use "wrapped" default values as long as they are convertable
constexpr p2::tag<3, int, DepthMaker> depth{};						//if all else fails we can specify a functor which makes our default value


template<typename...Ts>
void draw(Ts&&...args) {
	//make tuple uses D style syntax, compile time values as first arg list and runtime in the second arg list
	//first arg list must contain all name tags in the desired positional order
	//second list must be the args passed by the user
	auto ta = p2::make_tuple(length, height, depth)(std::forward<Ts>(args)...);
	auto l = ta[height];	//input parameter indexing is trivial
}

int main(int argc, const char** argv)
{
	draw(length = 1, height = 8);
}