#include_next <altivec.h>

// The altivec headers redefine vector and bool which conflict
// with C++'s bool type and std::vector. Undefine them here.
#undef vector
#undef bool
