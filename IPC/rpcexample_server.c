#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>

#include "rpcexample.h"


// implementation of the procedures


// add procedure

add_out *addproc_1_svc( add_in *numbers, struct svc_req *request )
{
   static add_out result;

   fprintf( stdout, "add procedure called with parameters %d and %d\n", numbers->left, numbers->right );

   result.value = numbers->left + numbers->right;

   // printf( "Numbers = %d and %d\n", numbers->dummy[0], numbers->dummy[1] );

   return ( &result );
}

// square procedure

square_out *squareproc_1_svc( square_in *number, struct svc_req *request )
{
   static square_out result;

   fprintf( stdout, "square procedure called with parameter %d\n", number->value );

   result.value = number->value * number->value;

   return ( &result );
}

