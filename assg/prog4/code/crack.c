/*
2. Brute-force password decryption using multi-threading.
     [32] Encryption algorithms work by operating on a password to produce a
     hash.  Brute-force password cracking works by applying an encryption
     algorithm to a range of possible passwords, and checking if each of the
     resulting hashes turned up by the encryption algorithm is equal to an
     acquired hash. 

     Naturally, this takes a long time. Anything which is known about the
     password helps a brute-force attempt--such as knowing that the password
     has only lowercase characters, or that it is exactly eight characters in
     length, and so forth.

     A file called ``crack.c`` is provided which contains a serial decryption
     algorithm for use with Linux ``crypt(3)``, and a file called ``hash``
     which contains a password hash for the password ``abba``.  Run it and
     observe how it works.  It assumes the password is 4 lowercase characters
     and that the salt is also lowercase characters.  
     
     Write a multi-threaded version using pthread to run the serial algorithm
     on all possible four-character lowercase-letter passwords, and check to
     see if the hashes are equal.  If they are, output the password to stdout
     and terminate the program.  Use four threads.  Call it ``crack.c``, as the
     original is called.
     
     You will need to be careful with pointers; in particular, ``crypt`` uses
     only one pointer to reference its result, so you will need to avoid race
     conditions in the multi-threaded version.  Use a mutex lock during calls
     to ``crypt`` and data copies from the pointer returned by crypt.
*/

#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <crypt.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#define THREADS 26

void ith(char *password, int index, int n) {
  for (int i=1; i<=n; i++) {
    password[i-1] = (index % 26) + 'a';
    index /= 26;
  }
  password[n] = '\0';
}


int main() {
  char *hash;
  char target[16];
  char password[5];
  char salt[3];

  int N=pow(26, 4);
  int M=pow(26, 2);

  int fd  = open("hash", 'r');
  int num = read(fd, target, 16);

  if (num) target[num-1] = 0;
  else     exit(0);
  close(fd);

  for (int i=0; i<N; i++) {
	// lock
    ith(password, i, 4); // changes password
    for (int j=0; j<M; j++) {
      ith(salt,     j, 2); // changes salt
      hash = crypt(password, salt); // gets hash for password and salt
      if (0==strcmp(hash, target)) {
        printf("Password: %s\t\tSalt: %s\t\tHash: %s\t\tTarget: %s\n", password, salt, hash, target);
        exit(1);
      } else printf("Trying %s...\r", password);
    }
	// unlock
  }

}
