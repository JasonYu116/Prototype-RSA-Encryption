# Assignment 5 - Public Key Cryptography
This program encrypts and decrypts ciphertext using a RSA algorithm  
decrypt.c: This contains the implementation and main() function for the decrypt program.  
encrypt.c: This contains the implementation and main() function for the encrypt program.  
keygen.c: This contains the implementation and main() function for the keygen program.  
numtheory.c: This contains the implementations of the number theory functions.  
numtheory.h: This specifies the interface for the number theory functions.  
randstate.c: This contains the implementation of the random state interface for the RSA library and number theory functions.  
randstate.h: This specifies the interface for initializing and clearing the random state.  
rsa.c: This contains the implementation of the RSA library.  
rsa.h: This specifies the interface for the RSA library.  
Makefile: builds and cleans the files needed to run in the assignment.  
README.md: contains the program documentation and information regarding file specification. This also lists any potencial bugs as well as program options and instructions to run the program.  
DESIGN.pdf: describes and contains the pseudocode as well as blueprint in regards to planning and thinking process for how the progrogram was appraoched and designed. It allows for others to understand and follow how to program the assignment as well.  
WRITEUP.pdf: describes the commands used in the assigment and an explanation for the thinkign and planning process of the assignment as well as discussion on the steps used for each file  

## Building
Run the following to build the encrypt, decrypt, and keygen executables
make  

## Running 
./keygen

-b : specifies the minimum bits needed for the public modulus n (default: 1024).  
-i : specifies the number of Miller-Rabin iterations for testing primes (default: 50).  
-n pbfile : specifies the public key file (default: rsa.pub).  
-d pvfile : specifies the private key file (default: rsa.priv).  
-s : specifies the random seed for the random state initialization (default: the seconds since the UNIX epoch, given by time(NULL)).  
-v : enables verbose output.  
-h : displays program synopsis and usage.  

echo "[plaintext]" | ./encrypt  

-i : specifies the input file to encrypt (default: stdin).  
-o : specifies the output file to encrypt (default: stdout).  
-n : specifies the file containing the public key (default: rsa.pub).  
-v : enables verbose output.  
-h : displays program synopsis and usage.  

echo "[ciphertext]" | ./decrypt  

-i : specifies the input file to decrypt (default: stdin).  
-o : specifies the output file to decrypt (default: stdout).  
-n : specifies the file containing the private key (default: rsa.priv).  
-v : enables verbose output.  
-h : displays program synopsis and usage.  


## Sources Consulted
The following sources were consulted:  
TA and Tutors in section  
Man files in bash through terminal  
gmp library: https://gmplib.org/
See individual assignment files for code references
