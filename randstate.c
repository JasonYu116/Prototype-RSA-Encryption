/* implement a small random state module*/

// Include neccessary files
#include <stdio.h>
#include <stdlib.h>
#include "randstate.h"

// Define and declare global variables 
gmp_randstate_t state; // state variable

// Initializes the global random state named state with a Mersenne Twister algorithm, using seed as the random seed.
void randstate_init(uint64_t seed) {
	// calls to gmp_randinit_mt()
	gmp_randinit_mt(state);
	
	// calls to gmp_randseed_ui()
	gmp_randseed_ui(state, seed);

    	//calls to srandom() using this seed
    	srandom(seed);
}

// Clears and frees all memory used by the initialized global random state named state.
void randstate_clear(void) {
	// calls to gmp_randclear()
	gmp_randclear(state);
}
