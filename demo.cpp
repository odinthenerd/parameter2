#include "parameter2.hpp"

namespace Octocat {
	void draw(std::unique_ptr<int> length, int height, int depth) {};
}

namespace NamedOctocat {
	namespace np = named_parameters;
	struct DepthMaker
	{
		int operator()() {
			return 42;
		}
	};
	struct unique_maker
	{
		std::unique_ptr<int> operator()() {
			return nullptr;
		}
	};
	constexpr np::Tagger<1, std::unique_ptr<int>, unique_maker> length{};
	constexpr np::Tagger<2, int> height{};
	constexpr np::Tagger<3, int, DepthMaker> depth{};
	template<typename...Ts>
	void draw(Ts...args) {
		auto a = list(length, height, depth);
		np::call(Octocat::draw, a, std::forward<Ts>(args)...);
	}

}

int main(int argc, const char** argv)
{
	{
		using namespace NamedOctocat;
		draw(length = std::make_unique<int>(), height = 8);
	}
}