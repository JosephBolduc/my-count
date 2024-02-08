#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>


int main( void ) {

   int segment_id = 0;
   char *shared_memory;

   // prompt the user to enter shared memory segment identifier
   fprintf( stdout, "Process B: Enter segment id: " );

   // read the identifier
   fscanf( stdin, "%d", &segment_id );

   
   // fprintf( stdout, "Read segment id = %d\n", segment_id );


   // attach an area of local memory to the shared memory segment
   shared_memory = ( char * ) shmat( segment_id, NULL, 0 );

   // write the message in the shared memory segment
   fprintf( stdout, "Process B: %s\n", shared_memory );
   // fprintf( stdout, "Process B: %c\n", shared_memory[0] );

   // break the link between the local memory and the shared memory segment
   shmdt( shared_memory );

   return 0;
}

