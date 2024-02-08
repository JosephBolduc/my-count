#include <stdio.h> 
#include <signal.h> 
  
// a signal handler
void catchsignal(int type) 
{ 

   // print a message
   printf( "Caught signal %d\n", type ); 

   return;
} 
  
int main( void )
{ 

   // install the signal handler
   signal( SIGINT, catchsignal ); 

   // wait forever
   while (1) ;

   return 0; 
} 

