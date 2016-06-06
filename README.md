# parameter2
Modern version of boost.parameter enabling named parameters without macros and better compile time

## goals:
 - at least 10x faster compile time
 - make implementing a function which takes named parameters much easier
 - don't use macros (we hate them)

## non goals:
 - no support for named template parameters (implementation is trivial and boost.hana style will make this obsolete in the long run)
 - no support for "predicate requirements" (at least in version 1)
 
