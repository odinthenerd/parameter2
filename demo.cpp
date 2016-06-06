#include "parameter2.hpp"

namespace p2 = parameter2;
struct DepthMaker
{
	int operator()() {
		return 42;
	}
};
constexpr auto length = p2::make_tag<1>(4);
constexpr p2::tag<2, int, std::integral_constant<int, 9>> height{};
constexpr p2::tag<3, int, DepthMaker> depth{};


template<typename...Ts>
void draw(Ts&&...args) {
	auto ta = p2::make_tuple(length, height, depth)(std::forward<Ts>(args)...);
	auto l = ta[height];
}

int main(int argc, const char** argv)
{
	draw(length = 1, height = 8);
}