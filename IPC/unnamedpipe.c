#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1


int main( void ) {


   char write_msg[BUFFER_SIZE] = "Hello World!";
   char read_msg[BUFFER_SIZE];

   int fd[2];

   pid_t pid;

   if ( pipe( fd ) == -1 ) {
      fprintf( stderr, "Pipe failed.\n" );
      return 1;
   }


   /* spawn a child process */
   pid = fork( );


   if ( pid < 0 ) {
      fprintf( stderr, "Fork failed.\n" );
      return 1;
   }


   if ( pid > 0 ) {

      // parent process

      // close the read end of the pipe
      close( fd[READ_END] );

      // fprintf( stdout, "PARENT: Writing to pipe...\n" );

      // prompt the user for a message
      fprintf( stdout, "PARENT: Enter message to send to child\n" );
      fgets( write_msg, BUFFER_SIZE, stdin ); 

      // write message to the pipe
      write( fd[WRITE_END], write_msg, strlen( write_msg ) + 1 );

      fprintf( stdout, "PARENT: Written to pipe...\n" );

      // wait for the child process to terminate
      wait( NULL );
      
    
      // wait for the user input
      fprintf( stdout, "Type any key to continue:" );
      getchar( );

      // close the write end of the pipe
      close( fd[WRITE_END] );

   } else {

      // close the write end of the pipe
      close( fd[WRITE_END] );

      // fprintf( stdout, "CHILD: Reading from pipe...\n" );

      // get the message from the pipe 
      read( fd[READ_END], read_msg, BUFFER_SIZE );
      
      // fprintf( stdout, "CHILD: Read from pipe...\n" );

      // write the message received from the parent on the screen
      fprintf( stdout, "CHILD: %s\n", read_msg );

      // close the write end of the pipe
      close( fd[READ_END] );

   }


   return 0;

}
