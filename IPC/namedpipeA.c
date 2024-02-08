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


   int value = mkfifo( pipeName, S_IRUSR | S_IWUSR ); 

   if ( value < 0 ) errormsg( "ERROR creating the named pipe" );


   // prompt the user to enter a message
   fprintf( stdout, "Process A: Enter a message to write to the named pipe\n" );
   fflush( stdout );
   fgets( buffer, BUFFER_SIZE, stdin );

   // open the pipe for writing
   int fd = open( pipeName, O_WRONLY );

   if ( fd < 0 ) errormsg( "ERROR opening the named pipe" );

   fprintf( stdout, "Process A: Writing to the named pipe ...\n" );

   // write the message to the pipe
   write( fd, buffer, strlen( buffer ) + 1 );   

   fprintf( stdout, "Process A: Written message to the pipe\n" );

   // close the pipe
   close( fd );

   // delete the pipe
   // unlink( pipeName );
 
   return 0;
}

