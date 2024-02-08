#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> 
#include <stdlib.h>
#include <strings.h>

const int BUFFER_SIZE = 256;
const int PORT = 11000;

void errormsg( char *msg )
{
    perror( msg );
    exit( 1 );
}



int main( void )
{
   struct sockaddr_in si_me, si_other;

   int sockd;
   socklen_t length = sizeof( si_other );

   char buffer[BUFFER_SIZE];
   
   // create a communication end point
   if ( ( sockd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )  
      errormsg( "ERROR creating a socket" );
   
   // attach the communication end point (socket) to an address (port)
   bzero( (void *) &si_me, sizeof( si_me ) );
   si_me.sin_family = AF_INET;
   si_me.sin_port = htons( PORT );
   si_me.sin_addr.s_addr = htonl( INADDR_ANY );

   if ( bind( sockd, ( const struct sockaddr * ) &si_me, sizeof( si_me ) ) < 0 )  
      errormsg( "ERROR binding to the socket");


   // attempt to receive a message
   if ( recvfrom( sockd, buffer, BUFFER_SIZE, 0, ( struct sockaddr * ) &si_other, (socklen_t *) &length ) < 0 )
      errormsg( "ERROR reading from the socket" );

   // print the message on the screen
   fprintf( stdout, "Process B: received message from %s:%d\n\tData: %s\n",
      inet_ntoa( si_other.sin_addr ), ntohs( si_other.sin_port ), buffer );  

   close( sockd );

   return 0;
}
