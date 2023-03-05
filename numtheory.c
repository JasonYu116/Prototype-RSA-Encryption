/* implements the number theory functions */

// Include neccessary files
#include <stdio.h>
#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "numtheory.h"
#include "randstate.h"

// lines 73 to 85 were referenced from Lev Teytelman (Tutor) in section
// lines 156 to 167 were referenced from Lev Teytelman (Tutor) in section

// Computes the greatest common divisor of storing the value of the computed divisor in d in relation to Euclidean algorithm
void gcd(mpz_t d, mpz_t a, mpz_t b) {
		
	// Declare and define all needed variables
	mpz_t temp, temp1, temp2;
	mpz_inits(temp, temp1, temp2, NULL);

	// Set the needed temp variables
    	mpz_set(temp1, b); mpz_set(temp2, a);

    	while (mpz_cmp_ui(temp1, 0) != 0) {
        	mpz_set(temp, temp1);
        	mpz_mod(temp1, temp2, temp1); //the loop iterates and keeps using modulus until b is equal to 0
        	mpz_set(temp2, temp);
    	}
	// temporary mpz_t variables set to putput mpz_t variables
    	mpz_set(d, temp2);
	// clear all mpz_t variables used
	mpz_clears(temp2, temp1, temp, NULL);
}

// Computing base raised to the exponent power modulo modulus, and storing the computed result.
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {

  	// Declare and define all needed variables
	mpz_t p, exp, n, v;

       	// Initilize needed variables
    	mpz_inits(p, exp, n, v, NULL);

	// Set needed variables used for calculating out which is equal to base^exponent (mod modulus)
    	mpz_set(p, base);
	mpz_set(exp, exponent);
	mpz_set(n, modulus);
	mpz_set_ui(v, 1);

    	// Check to see if exponenet is not zero
    	while (mpz_cmp_ui(exp, 0) > 0) {
        	if (mpz_odd_p(exp) != 0) {
            		mpz_mul(v, v, p);
            		mpz_mod(v, v, n);
        	}
        	mpz_mul(p, p, p);
        	mpz_mod(p, p, n); //p^2mod n
        	mpz_fdiv_q_ui(exp, exp, 2); //floor division: exp = exp/2
    	}

    	mpz_set(out, v);

	// clear all used mpz_t variables
    	mpz_clears(exp, n, p, v, NULL);
}

// Using miller-rabin primality test to verify if integer is prime or not 
bool is_prime(mpz_t n, uint64_t iters) {

	// Declare and define needed variables
	mpz_t dividend, s, r, div, result, a, y, n_minus, s_minus, j;
	// Initilize needed variables
	mpz_inits(dividend, s, div, result, r, a, y, n_minus, s_minus, j, NULL);
	// Compute and set needed variables in order to not change original parameters
	mpz_set(dividend, n);
        mpz_sub_ui(dividend, dividend,1);
        mpz_set_ui(div,1);
        mpz_set(result, n);
        mpz_sub_ui(result, result, 3);
        mpz_sub_ui(n_minus, n, 1);

	// find s and r reapeadly dividing n-1 by 2 until r is odd
    	while (mpz_odd_p(r) == 0) {
        	mpz_add_ui(s, s, 1);
        	mpz_div(r, dividend, div);
        	mpz_mul_ui(div, div, 2);
    	}
	
    	mpz_sub_ui(s, s, 1);
    	mpz_sub_ui(s_minus, s, 1);

	// iterate form 1 to specified number by operator: Default: 50
    	for (uint64_t i = 1; i < iters; i++) {
		
		// call u_ramdomm to generate numbers from 0 to n-1 inclusive
     		mpz_urandomm(a, state, result);
	        // increment a by 2 to set interval from 2 to n-2	
        	mpz_add_ui(a, a, 2); 
		// call pow_mod to set y to a^r (mod n)
        	pow_mod(y, a, r, n);
		
		// check if y is not 1 and y is not n-1
        	if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n_minus) != 0) {
			// set j to 1
            		mpz_set_ui(j, 1); //sets j to 1

			// check that j is less than or equal to s-1 and y is not equal to n-1
            		while (mpz_cmp(j, s_minus) <= 0 && mpz_cmp(y, n_minus) != 0) {
                		mpz_mul(y, y, y);
                		mpz_mod(y, y, n); // calculates y^2 mod n
						  // 
                		if (mpz_cmp_ui(y, 1) == 0) {
                    			mpz_clears(dividend, s, r, div, y, a, s_minus, n_minus, j, result, NULL);
					// return false if y equal to 1
					return false;
                		}
                		// increment j by 1	
				mpz_add_ui(j, j, 1);
            		}
			
            		if (mpz_cmp(y, n_minus) != 0) {
                		mpz_clears(dividend, s, r, div, y, a, s_minus, n_minus, j, result, NULL);
				// return false if y is not equal to n-1
				return false;
            		}
        	}
    	}
    	// clear all used mpz_t variables
	mpz_clears(dividend, s, r, div, y, a, s_minus, n_minus, j, result, NULL);
	// return true if all prime tests pass
	return true;
}

// generate random prime numbers
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
	
	// Declare and define needed variables
	mpz_t min;
	// Initialize needed variables
	mpz_init(min);
	// Generate random prime number between 0 and 2^bits-1 long
    	mpz_ui_pow_ui(min, 2, bits);
    	mpz_urandomb(p, state, bits);
	// add 2^bits to the result in order for the number is at least bits number of bits long
    	mpz_add(p, p, min);

	// check to see if the random number is prime number
	while (is_prime(p, iters) == false) {
		// make random numbers bits long until number is prime
        	mpz_urandomb(p, state, bits);
        	mpz_add(p, p, min);
    	}
	// clear all mpz_t variables
   	mpz_clear(min);
}

// function to calculate the modular inverse
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {

	// Declare and define needed variables
	mpz_t r, r_prime, t, t_prime, q, q_product_r, q_product_t, r_sub, t_sub, temp_r, temp_i;
	// Initilize needed variables
	mpz_inits(r, r_prime, t, t_prime, q, q_product_r, q_product_t, r_sub, t_sub, temp_r, temp_i, NULL);
	// Set needed variables and 
    	mpz_set(r, n);
    	mpz_set(r_prime, a);
    	mpz_set_ui(t, 0);
    	mpz_set_ui(t_prime, 1);

	// check if r prime is zero
    	while (mpz_cmp_ui(r_prime, 0) != 0) {
		// calculate q to be equal to floor (r/r_prime), and switch between r and r_prime
        	mpz_fdiv_q(q, r, r_prime);
        	mpz_mul(q_product_r, q, r_prime);
        	mpz_mul(q_product_t, q, t_prime);
        	mpz_sub(r_sub, r, q_product_r);
        	mpz_sub(t_sub, t, q_product_t);
        	mpz_set(temp_r, r_prime);
        	mpz_set(temp_i, t_prime);
        	mpz_set(r, temp_r);
        	mpz_set(r_prime, r_sub);
        	mpz_set(t, temp_i);
        	mpz_set(t_prime, t_sub);
    	}
	
	// return no inverse if r is greater than 1 
    	if (mpz_cmp_ui(r, 1) > 0) {
        	mpz_clears(r, r_prime, q, t_prime, q_product_r, q_product_t, r_sub, t_sub, temp_r, temp_i, t, NULL);
        	return;
    	}

	// increment t by n if less than zero
    	if (mpz_cmp_ui(t, 0) < 0) { 
        	mpz_add(t, t, n);
    	}

    	mpz_set(i, t);

	// clear all used mpz_t variables
    	mpz_clears(r, r_prime, q, t_prime, q_product_r, q_product_t, r_sub, temp_r, temp_i, t, NULL);
}
