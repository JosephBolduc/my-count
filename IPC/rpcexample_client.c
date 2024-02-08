#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>

#include "rpcexample.h"

// name of the machine where the server is running
const char hostname[] = "localhost.utdallas.edu";


void errormsg( char *msg ) {
   perror( msg );
   exit( 1 );
}
 
int main( void ) {


   CLIENT*  client;

   add_in numbers_to_add = { 40, 50 };
   add_out *sum;

   square_in number_to_square = { -100 };
   square_out *squared;


   // create a connection to the server 
   client = clnt_create( hostname, RPCEXAMPLE_PROG, RPCEXAMPLE_VERS, "tcp" );

   if ( client == NULL ) errormsg( "ERROR creating client" );

   // call add procedure
   sum = addproc_1( &numbers_to_add, client );

   if ( sum == NULL ) errormsg( "ERROR invoking add procedure" );

   fprintf( stdout, "Adding numbers: %d + %d = %d\n", numbers_to_add.left, numbers_to_add.right, sum->value );

   // call square procedure
   squared = squareproc_1( &number_to_square, client );

   if ( squared == NULL ) errormsg( "ERROR invoking square procedure" );

   fprintf( stdout, "Squaring number: %d * %d = %d\n", number_to_square.value, number_to_square.value, squared->value );
  
   return 0;

}
