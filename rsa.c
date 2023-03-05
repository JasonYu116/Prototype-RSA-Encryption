#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
//#include <stdlib.h>
//#include <gmp.h>

void lcm(mpz_t d, mpz_t a, mpz_t b);

// makes public key
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
	
	// Declare and define needed variables
    	mpz_t p_minus, q_minus, least, tester;
	// Initilize needed variables
	mpz_inits(q_minus, p_minus, tester, least, NULL);

	// calculates respective bit sizes for p and q
    	uint64_t size = (random() % (3 * nbits / 4 - nbits / 4 + 1)) + nbits / 4;
    	uint64_t remain = nbits - size;
	//calculates the respective bit sizes for p and q
    	make_prime(p, size, iters);
    	make_prime(q, remain, iters);
    	mpz_mul(n, p, q);

    	//generates p and q and then calculates n by taking the product of p and q
    	mpz_sub_ui(q_minus, q, 1);
    	mpz_sub_ui(p_minus, p, 1);

	// find lcm p-1 and q-1 to find coprimes of p and q
    	lcm(least, p_minus, q_minus);
	
	// generate public key between 0 and 2^bits-1 long
    	do {
		// loop if not satisfy gcd (e, lcm(p-1, q-1)) not zero
        	mpz_urandomb(e, state, nbits);
        	gcd(tester, e, least);

    	} while (mpz_cmp_ui(tester, 1) != 0);

	// clear all used mpz_t variables
   	mpz_clears(q_minus, p_minus, tester, least, NULL);
}

// writes to the public key using rsa
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	// use gmp_fprintf and fprintf to print out n, e, s, as hexstrings and username as regular string
    	gmp_fprintf(pbfile, "%Zx \n", n);
    	gmp_fprintf(pbfile, "%Zx \n", e);
    	gmp_fprintf(pbfile, "%Zx \n", s);
    	fprintf(pbfile, "%s \n", username);
}

// reads the public key using rsa 
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	// reads n, e, s as numbers and username as string through gmp,fscaf, and fscanf respectively
    	gmp_fscanf(pbfile, "%Zx \n", n);
    	gmp_fscanf(pbfile, "%Zx \n", e);
    	gmp_fscanf(pbfile, "%Zx \n", s);
    	fscanf(pbfile, "%s \n", username);
}

// makes private key using rsa algorithm
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
	// Declare and define all needed variables
    	mpz_t lambda, p_minus, q_minus;
	// Initilize all needed variables
	mpz_inits(lambda, p_minus, q_minus, NULL);

	// Generate private key through lcm(p-1, q-1) and finding modular inverse e of public key
    	mpz_sub_ui(p_minus, p, 1);
    	mpz_sub_ui(q_minus, q, 1);
    	lcm(lambda, p_minus, q_minus);
    	mod_inverse(d, e, lambda);
    	mpz_clear(lambda);
	// clear all used mpz_t variables
    	mpz_clears(p_minus, q_minus, NULL);
}

// writes to private key using rsa algorithm
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
	// write n and d to the private key file using command of gmp_fprintf
    	gmp_fprintf(pvfile, "%Zx \n", n);
    	gmp_fprintf(pvfile, "%Zx \n", d);
}

// read from the private key using rsa algorithm
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
	// read n and d from private key using command of gmp_fscanf
    	gmp_fscanf(pvfile, "%Zx \n", n);
    	gmp_fscanf(pvfile, "%Zx \n", d);
}

// rsa encryption algorithm
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
	// data encryption through C = M^e mod n
    	pow_mod(c, m, e, n);
}

// encrypts file using rsa algorithm
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
	
	// Declare and define needed variables
    	uint64_t k;
    	size_t j;
    	mpz_t m, c;
	// calculate interval for bytes to be read in file
    	k = (mpz_sizeinbase(n, 2) - 1) / 8;
	// Initilize needed variables: plaintext and ciphertext, respectively
    	mpz_inits(m, c, NULL);

	// declaration of array and prepending 0xFF to fit range of encryption
    	uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));
    	arr[0] = 0xFF;

	// check to see if there are remaining bytes to be read in file using j
    	while ((j = fread(&arr[1], sizeof(uint8_t), k - 1, infile)) > 0) {
	        // convert elements in arr to m	
		mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, arr);
		// use rsa_encrypt to obtain c: ciphertext
        	rsa_encrypt(c, m, e, n); 
		// print c to the outfile: Default: stdout
        	gmp_fprintf(outfile, "%Zx \n", c);
    	}
	// clear all used mpz_t variables and other elements
    	free(arr);
    	mpz_clears(m, c, NULL);
}

// decyption using rsa algorithm
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
	// data decryption through M = C^d mod n
   	pow_mod(m, c, d, n);
}

// decryption of file using rsa algorithm
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
	// Declare and define all needed variables
    	uint64_t k;
    	size_t j;
    	mpz_t c, m;

	// Interval for the reading of the plaintext and ciphertext in the file
    	k = (mpz_sizeinbase(n, 2) - 1) / 8;
    	// Initilize needed variables in relation to ciphertext and plaintext respectively
    	mpz_inits(c, m, NULL);

    	uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));

    	while (gmp_fscanf(infile, "%Zx \n", c) != EOF) {
        	rsa_decrypt(m, c, d, n);
        	mpz_export(arr, &j, 1, sizeof(uint8_t), 1, 0, m);
        	fwrite(&arr[1], sizeof(uint8_t), j - 1, outfile);
    	}
	// clear all used mpz_t variables and elements
    	free(arr);
    	mpz_clears(c, m, NULL);
}

// signs the file with a signature using rsa algorithm
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
	// using pow_mod to get signature
    	pow_mod(s, m, d, n);
}

// verifies the validity of username from file using rsa algorithm
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
	// Declare and define needed variables
    	mpz_t t;
	// Initilize needed variables
    	mpz_init(t);

	// verify signature through T = S^e mod n if it is valid or not
    	pow_mod(t, s, e, n);
    	if (mpz_cmp(m, t) == 0) {
        	mpz_clear(t);
        	return true;
   	}	 
	else {
        	mpz_clear(t);
        	return false;
    	}
	// clear all used mpz_t variables
    	mpz_clear(t);
}

// lowest common denominator helper function
void lcm(mpz_t d, mpz_t a, mpz_t b) {
	// Declare all needed variables
    	mpz_t product, x, temp, temp2;
	// Initilize needed variables
	mpz_inits(x, product, temp, temp2, NULL);
	// Set needed variables
    	mpz_set(temp, a);
    	mpz_set(temp2, b);
	// use gcd function to simulate abs (a*b)/ gcd (a,b)
    	mpz_mul(product, temp, temp2);
    	mpz_abs(product, product);
    	gcd(x, a, b);
    	mpz_div(d, product, x);
	// Clear all used mpz_t variables
    	mpz_clears(x, product, temp, temp2, NULL);
}
