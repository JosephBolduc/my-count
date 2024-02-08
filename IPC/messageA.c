#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>

const int BUFFER_SIZE = 256;
const int PORT = 11000;

const char DESTINATION_IP[] = "127.0.0.1";

void errormsg( char *msg )
{
    perror( msg );
    exit( 1 );
}

int main( void )
{

   struct sockaddr_in si_other;

   int sockd; 

   char buffer[BUFFER_SIZE];
   
   if ( ( sockd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )  
      errormsg( "ERROR creating a socket" );
   
   bzero( (void *) &si_other, sizeof( si_other ) );
   
   si_other.sin_family = AF_INET;
   si_other.sin_port = htons( PORT );
   
   if ( inet_aton( DESTINATION_IP, &si_other.sin_addr ) == 0 ) 
      errormsg( "ERROR converting IP address" );

   // prompt the user to enter a message
   fprintf( stdout, "Process A: Enter a message to send\n" );
   fgets( buffer, BUFFER_SIZE, stdin );

   if ( sendto( sockd, buffer, strlen( buffer ) + 1, 0, ( const struct sockaddr* ) &si_other, sizeof( si_other ) ) < 0 )
      errormsg( "ERROR sending message" );    

   close( sockd ); 
   return 0; 
}
