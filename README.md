# parameter2
Modern version of boost.parameter enabling named parameters without macros and better compile time. 
*it should be noted that this lib is in a very early stage and should not be used for anything but fun. Input is however very much appreciated*

```C++
namespace p2 = parameter2;
struct DepthMaker
{
	int operator()() {
		return 42;
	}
};

//in order to use named parameters the user must first define names:

//we can capture default value as a constexpr
constexpr auto length = p2::make_tag<1>(4);				

//we can also use "wrapped" default values as long as they are convertable
constexpr p2::tag<2, int, std::integral_constant<int,9>> height{};	

//if all else fails we can specify a functor which makes our default value
constexpr p2::tag<3, int, DepthMaker> depth{};						


template<typename...Ts>
void draw(Ts&&...args) {
	//make tuple uses D style syntax, compile time values as first arg list and runtime in the second arg list
	//first arg list must contain all name tags in the desired positional order
	//second list must be the args passed by the user
	auto ta = p2::make_tuple(length, height, depth)(std::forward<Ts>(args)...);
	auto h = ta[height];	//input parameter indexing is trivial
	doSomething(ta[length]);
}

int main(int argc, const char** argv)
{
	draw(length = 1, height = 8);
}
```

In our opinion Boost.parameter is slightly too premissive when it comes to convertability, parameter2 follows the following strict rules:
 - positional arguements are only accepted if no named arguements are present, all arguements are convertable to their respective postitions and the remaining arguements all have defaults.
 - deduced parameters are only allowed if they are convertable to exactly one candidate

## goals:
 - at least 10x faster compile time
 - make implementing a function which takes named parameters much easier
 - don't use macros (we hate them)

## non goals:
 - no support for named template parameters (implementation is trivial and boost.hana style will make this obsolete in the long run)
 - no support for "predicate requirements" (at least in version 1)

## open questions:
 - should we force users to wrap out parameters with 'std::ref'? (out parameters should be getting more and more unpopular due to compiler improvements and their related coding style improvments. Also chances are that if a user wants the added clairity of named parameters that they also want to visibly state the a parameter is an out parameter.)
 - should we support the capture default values as a constexpr parameter? (the compiler should be able eliminate any unneeded overhead, however in the embedded domain the size of debug builds also matter.)
 
