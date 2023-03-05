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

// help function to print avaiable options to use on the command line
void usage(void) {
               
	printf ("%s\n", "Usage: ./encrypt [options]");
        printf ("%s\n", "  ./encrypt ./encrypt-dist encrypts an input file using the specified public key file,");
       	printf ("%s\n", "  writing the result to the specified output file.");
      	printf ("%s\n", "    -i <infile> : Read input from <infile>. Default: standard input.");
       	printf ("%s\n", "    -o <outfile>: Write output to <outfile>. Default: standard output.");
       	printf ("%s\n", "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.");
     	printf ("%s\n", "    -n <pbfile> : Public key file is <pbfile>. Default rsa.pub");
   	printf ("%s\n", "    -v          : Enable verbose output.");
      	printf ("%s\n", "    -h          : Display program synopsis and usage.");

}

// main function for switch cases and commands
int main(int argc, char **argv) {
	// Declare and define needed variables
    	int opt = 0;
    	FILE *infile = NULL;
    	FILE *outfile = NULL;
    	FILE *pbfile = NULL;
    	char *pub_file = "rsa.pub";
    	char *input = NULL;
    	char *output = NULL;
    	int verb = 0;
    	mpz_t n, e, s, user, m;
	// Initilize needed variables
    	mpz_inits(n, e, s, user, m, NULL);
 
	// Initilization of string being used to get username as 128 allocation is limit of username in unix
   	char *test = (char *) malloc(128 * sizeof(char));

	// switch statement for command line options
    	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        	switch (opt) {
        		case 'i': 
				input = optarg; 
				break;
        		case 'o': 
				output = optarg; 
				break;
        		case 'n': 
				pub_file = optarg; 
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
	// Check if user specified a nonexistent file
    	if (output != NULL && (fopen(output, "w")) == NULL) {
        		printf("Error opening %s.\n", output);
        		fclose(outfile);
        		exit(1);
    	}

	// Check if user specified nonexistent input file 
    	if (input != NULL && (fopen(input, "r")) == NULL) {
        	printf("Error opening %s.\n", input);
        	fclose(infile);
        	exit(1);
    	}

	// Check if user specified an existing input file
    	if (input != NULL && (fopen(input, "r")) != NULL) {
        	infile = fopen(input, "r");
    	}

	// Check if user specified nonexistent public key file
    	if ((fopen(pub_file, "r")) == NULL) {
        	printf("Error opening %s.\n", pub_file);
        	fclose(pbfile);
        	exit(1);
    	}
	// Check if user specified a existing public key file

   	else if (fopen(pub_file, "r") != NULL) {
        	pbfile = fopen(pub_file, "r");
        	rsa_read_pub(n, e, s, test, pbfile);
    	}

	// Check if user specified existing output file
    	if (output != NULL && (fopen(output, "w"))!= NULL) {
       	 	outfile = fopen(output, "w");
    	}
	
	// Check if user specified no output file
    	if (output == NULL) {
        	outfile = stdout;
    	}

	// Command to run if verbose is turned on
    	if (verb == 1) {
        	printf("user =%s \n", test);
      	 	gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        	gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        	gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    	}

	// Check to see if user specified no input file
    	if (input == NULL) {
        	infile = stdin;
    	}
	
	// Convert username to mpz_t
    	mpz_set_str(user, test, 62);

	// Check validicy of signature
    	if (rsa_verify(user, s, e, n) == false) {
        	printf("Error, username was not verified.\n");
        	exit(1);
    	}

	// Encrypt the input file
    	rsa_encrypt_file(infile, outfile, n, e);

	// Close and clear all used files and mpz_t variables
    	fclose(pbfile);
    	fclose(infile);
    	fclose(outfile);
    	mpz_clears(n, e, s, user, m, NULL);
    	free(test);
    	test = NULL;
}
