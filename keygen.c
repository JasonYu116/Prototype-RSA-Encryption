#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <gmp.h>
#include "rsa.h"
#include "randstate.h"

#define OPTIONS "b:i:n:d:s:vh"

void usage(void) {

	printf ("%s\n", "Usage: ./keygen-dist [options]");
        printf ("%s\n", "  ./keygen-dist generates a public / private key pair, placing the keys into the public and private");
       	printf ("%s\n", "  key files as specified below. The keys have a modulus (n) whose length is specified in");
      	printf ("%s\n", "  the program options.");
        printf ("%s\n", "    -s <seed>   : Use <seed> as the random number seed. Default: time()");
        printf ("%s\n", "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024");
        printf ("%s\n", "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50");
        printf ("%s\n", "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub");
        printf ("%s\n", "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv");
	printf ("%s\n", "    -v          : Enable verbose output.");
	printf ("%s\n", "    -h          : Display program synopsis and usage.");

}
 
// main function for switch cases
int main(int argc, char **argv) {
	// Declare and define needed variables
    	int opt = 0;
    	uint64_t bits = 1024;
    	uint64_t iters = 50;
    	bool check_seed = false;
    	int seed = 0;
    	int verb = 0;
    	char *pub = "rsa.pub";
    	char *priv = "rsa.priv";
   	FILE *pbfile = NULL;
    	FILE *pvfile = NULL;
    	char *user = getenv("USER");	
   	mpz_t use, signature, p, q, n, e, d;
	// Initilize needed variables
	mpz_inits(p, q, n, e, d, use, signature, NULL);
	// command line options ofr switch statement

    	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        	switch (opt) {
        		case 'b': 
				bits = (uint64_t) strtoul(optarg, NULL, 10); 
				break;
        		case 'i': 
				iters = (uint64_t) strtoul(optarg, NULL, 10); 
				break;
        		case 'n': 
				pub = optarg; 
				break;
        		case 'd': 
				priv = optarg; 
				break;
        		case 's':
            			seed = (uint64_t) strtoul(optarg, NULL, 10);
            			check_seed = true;
            			break;
        		case 'v': 
				verb = 1; 
				break;
        		case 'h': 
				usage(); 
				break;
			default:
				usage();
				exit(1);
				break;
        	}
    	}

	//open private and public key files for writing in terms of operator input
    	pbfile = fopen(pub, "w");
    	pvfile = fopen(priv, "w");

	// obtains username to verify who is using keys
    	uint64_t fi = fileno(pvfile);
    	fchmod(fi, S_IRUSR | S_IWUSR);

	// check seed
    	if (check_seed == false) {
		// Initilization of seed value
        	randstate_init(time(NULL));
    	} 
	else {
        	randstate_init(seed);
    	}
    	if (bits <= 0 || bits < 50 || bits > 4096) {
        	exit(1);
    	}

	// generates the public and private keys
    	rsa_make_pub(p, q, n, e, bits, iters);
    	rsa_make_priv(d, e, p, q);

	// convert username into a mpz_t with base 62
    	mpz_set_str(use, user, 62);
    	rsa_sign(signature, use, d, n);
   	rsa_write_pub(n, e, signature, user, pbfile);
    	rsa_write_priv(n, d, pvfile);

	// if verbose turn on, prints variables and bit sizes
    	if (verb == 1) {
        	printf("user = %s \n", user);
        	gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(signature, 2), signature);
        	gmp_printf("p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        	gmp_printf("q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        	gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        	gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        	gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    	}

	// clear and close all used files and mpz_t variables
    mpz_clears(p, q, n, e, d, use, signature, NULL);
    fclose(pvfile);
    fclose(pbfile);
    randstate_clear();
}
