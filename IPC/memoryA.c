#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

const int MEMORY_SIZE = 4096;
const int BUFFER_SIZE = 256;

void errormsg( char *msg ) {
   perror( msg );
   exit( 1 );
}


int main( void ) {

   int segment_id;
   
   char buffer[BUFFER_SIZE];

   // create a memory segment to be shared
   segment_id = shmget( IPC_PRIVATE, MEMORY_SIZE, S_IRUSR | S_IWUSR );

   if ( segment_id < 0 ) errormsg( "ERROR in creating a shared memory segment\n" );

   fprintf( stdout, "Segment id = %d\n", segment_id );

   // attach an area of local memory to the shared memory segment
   char *shared_memory = ( char * ) shmat( segment_id, NULL, 0 );


   // prompt the user to enter a message
   fprintf( stdout, "Process A: Enter a message to write to the shared memory segment\n" );
   fgets( buffer, BUFFER_SIZE, stdin );

   // write the message to the shared memory segment
   sprintf( shared_memory, buffer );

   fprintf( stdout, "Process A: Written message to the shared memory\n" );

   // break the link between the local memory and the shared memory segment
   shmdt( shared_memory );

   // wait for a user input
   getchar( );

   // mark the shared memory segment for destruction
   shmctl( segment_id, IPC_RMID, NULL );


   return 0;
}

