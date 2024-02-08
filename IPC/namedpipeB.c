#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int BUFFER_SIZE = 256;
const char pipeName[] = "/tmp/NP";


void errormsg( char *msg ) {
   perror( msg );
   exit( 1 );
}


int main( void ) {

   char buffer[BUFFER_SIZE];
   
   // open the pipe for reading
   int fd = open( pipeName, O_RDONLY );

   if ( fd < 0 ) errormsg( "ERROR opening the name pipe" );

   // read from the pipe
   fprintf( stdout, "Process B: Reading from the named pipe ...\n" );

   int numberRead = read( fd, buffer, BUFFER_SIZE );
   
   buffer[numberRead] = '\0';

   // write the message read from the pipe
   fprintf( stdout, "Process B: %s\n",  buffer );

   // close the pipe
   close( fd );

   return 0;
}

