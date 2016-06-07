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
//(numbers are needed in order to disambiguate between two tags with the same type. numbers need not be in asscending 
//order or contiguous but must be unique within a call to make_tuple)

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
	auto &l = ta[length];
	doSomething(l);
	doSomething(ta[length]); //indexing is resolved at compile time so this is not as inefficient as it looks
}

int main(int argc, const char** argv)
{
	draw(length = 1, height = 8);
}
```

In our opinion Boost.parameter is slightly too premissive when it comes to convertability, parameter2 follows the following strict rules:
 - all positional arguements must be alligned left of one or zero named arguements. If a named arguement is provided all arguements to its right must be named or uniquly convertable.
 - deduced parameters are only allowed if they are convertable to exactly one candidate, they can be mixed with named arguements but only with positional arguements if seperated by a named arguement.

## goals:
 - at least 10x faster compile time
 - make implementing a function which takes named parameters much easier
 - don't use macros (we hate them)
 - allow return type to be dependant on input types (allows boost.hana style coding)
 - no `#define BOOST_PARAMETER_MAX_ARITY` and other hackery

## non goals:
 - no support for named template parameters (implementation is trivial and boost.hana style will make this obsolete in the long run)
 - no support for "predicate requirements" (at least in version 1)

## open questions:
 - should we force users to wrap out parameters with 'std::ref'? (out parameters should be getting more and more unpopular due to compiler improvements and their related coding style improvments. Also chances are that if a user wants the added clairity of named parameters that they also want to visibly state the a parameter is an out parameter.)
 - should we support the capture default values as a constexpr parameter? (the compiler should be able eliminate any unneeded overhead, however in the embedded domain the size of debug builds also matter.)
 - should we support non constexpr defaults? This is really only critical if we want to default out parameters.
 
## requirements:
 - c++14 or higher compiler
 - since our goal is speed we use brigand (the only dependancy), however use with any other MPL library possible to support if needed
