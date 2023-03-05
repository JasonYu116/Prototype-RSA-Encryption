#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "rsa.h"
#include "randstate.h"

#define OPTIONS "i:o:n:vh"

// print out usage options for command line
void usage(void) {
               
	printf ("%s\n", "Usage: ./decrypt [options]");
     	printf ("%s\n", "  ./decrypt-dist decrypts an input file using the specified private key file,");
       	printf ("%s\n", "  writing the result to the specified output file.");
       	printf ("%s\n", "    -i <infile> : Read input from <infile>. Default: standard input.");
      	printf ("%s\n", "    -o <outfile>: Write output to <outfile>. Default: standard output.");
    	printf ("%s\n", "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.");
       	printf ("%s\n", "    -v          : Enable verbose output.");
     	printf ("%s\n", "    -h          : Display program synopsis and usage.");
}

// main function for switch case
int main(int argc, char **argv) {
	// Declare and define needed variables
    	int opt = 0;
    	FILE *infile = NULL;
    	FILE *outfile = NULL;
    	FILE *pvfile = NULL;
    	char *priv_file = "rsa.priv";
    	char *input = NULL;
    	char *output = NULL;
    	int verb = 0;
    	mpz_t n, d;
    	mpz_inits(n, d, NULL);

    	// Cases for command line options
    	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        	switch (opt) {
        		case 'i': 
				input = optarg; 
				break;
        		case 'o': 
				output = optarg; 
				break;
        		case 'n': 
				priv_file = optarg; 
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

	// Check for nonexistent output file
    	if (output != NULL && (fopen(output, "w")) == NULL) {
        	printf("Error opening %s.\n", output);
        	exit(1);
    	}

	// Check for nonexistent input file
    	if (input != NULL && (fopen(input, "r")) == NULL) {
        	printf("Error opening %s.\n", input);
		// close and clear all used variables and files
        	fclose(pvfile);
        	fclose(infile);
        	fclose(outfile);
      	 	mpz_clears(n, d, NULL);
        	exit(1);
    	}

	// Check for existing input file
   	if (input != NULL && (fopen(input, "r")) != NULL) {
        	infile = fopen(input, "r");
    	}

	// Check for nonexistent private key file
    	if (fopen(priv_file, "r") == NULL) {
        	printf("Error opening %s.\n", priv_file);
		// Clear and close all used mpz_t variables and files
        	fclose(pvfile);
        	fclose(infile);
        	fclose(outfile);
        	mpz_clears(n, d, NULL);
        	exit(1);
    	}
	// Check for existing private key file
    	else if ((fopen(priv_file, "r")) != NULL) {
        	pvfile = fopen(priv_file, "r");
        	rsa_read_priv(n, d, pvfile);
    	}

	// Check for nonexistent output file
    	if (output != NULL && (fopen(output, "w")) != NULL) {
        	outfile = fopen(output, "w");
    	}

	// Check if user did not specify a output file
    	if (output == NULL) {
        	outfile = stdout;
    	}

	// Commands for if verbose is turned on
    	if (verb == 1) {
        	gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        	gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    	}

	// Check user did not specify a input file
   	if (input == NULL) {
        	infile = stdin;
    	}

	// decrypt the ciphertext in the input file
    	rsa_decrypt_file(infile, outfile, n, d);

	// Close and clear all used files and mpz_t variables
    	fclose(pvfile);
    	fclose(infile);
    	fclose(outfile);
    	mpz_clears(n, d, NULL);
}
