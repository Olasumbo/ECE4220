
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>

#define CHAR_DEV "/dev/Lab6"

#define MSG_SIZE 40 // message size

char IP_save[16];
socklen_t fromlen;
int sock;
struct sockaddr_in addr;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void btn_activated( void * ptr )
{
        //Open the character device
        int cdev_id, devi;
        char buffer[MSG_SIZE];

        if( (cdev_id = open( CHAR_DEV, O_RDONLY )) == -1 )
        {
                printf( "Cannot open device for reading %d\n", CHAR_DEV );
        }

        while(1)
        {
                devi = read( cdev_id, buffer, sizeof(buffer) );
                if( devi != sizeof(buffer) )
                {
                        printf( "Read Failed" );
                        break;
                }
                else if( buffer[0] != '\0' )
                {	
                        printf( "Recieved Button pressed: %s\n", buffer );
                        buffer[0] = '\0';

                        printf( "Sending Command: @%c to %s\n", buffer[1], IP_save );
        
                        char send[MSG_SIZE];
                        
                        sprintf( send, "@%c", buffer[1] );

                        addr.sin_addr.s_addr = inet_addr( IP_save );
                        
                        sendto( sock, send, MSG_SIZE, 0, (const struct sockaddr *)&addr, fromlen );
                        
                        //sendto( sock, ReturnVote, MSG_SIZE, 0, (const struct sockaddr *)&addr, fromlen );

                }
        
        	
        }

}    

int main(int argc, char *argv[])
{

    int boolval = 1; // for a socket option

    if (argc != 3)
    {
        printf("usage:: myfile port name\n");
        exit(1);
    }
    
    int portno = atoi(argv[1]);//saving the port number from user input
    
    char Name[14];

    strcpy(Name , argv[2]); // Destination, source
    
    struct ifaddrs *ifaddr = NULL, *ifa = NULL;
    
    void *AddrPtr = NULL;
    char host[INET6_ADDRSTRLEN];
    
    
    if (getifaddrs(&ifaddr) == -1) 
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
            
    /* Accesing the link list */
    for(ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if(ifa->ifa_addr->sa_family == AF_INET)
        {
            AddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
                        printf("Interface: <%s>\n",ifa->ifa_name ); 
                        printf("Internet Address: %s\n", inet_ntop(ifa->ifa_addr->sa_family, AddrPtr, host, sizeof(host)));
                                            
                    strcpy( IP_save, inet_ntop(ifa->ifa_addr->sa_family, AddrPtr, host, sizeof(host)) ); 
        }
    }
        
int length;

printf( "\nMy IP is: %s\n", IP_save );
    /* printf( "\nMy IP is: %s\n", IP1 );
    printf( "\nMy IP is: %s\n", IP2 );
    printf( "\nMy IP is: %s\n", IP3 ); */
        
sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionbased.
if (sock < 0)
{
        error("Opening socket");
}

struct ifreq ifreak;
ifreak.ifr_addr.sa_family = AF_INET;

    struct sockaddr_in server;
    length = sizeof(server);
    bzero( &server, length );
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( portno );
    
/* length = sizeof(server);			// length of structure
bzero(&server,length);			// sets all values to zero. memset() could be used
server.sin_family = AF_INET;		// symbol constant for Internet domain
server.sin_addr.s_addr = INADDR_ANY;		// IP address of the machine on which
                                                                                        // the server is running
server.sin_port = htons(atoi(argv[1]));	// port number */

    fromlen = sizeof( struct sockaddr_in );
    snprintf( ifreak.ifr_name, IFNAMSIZ, "wlan0" );
            
            //strncpy(ifreak.ifr_name, "wifi1", IFNAMSIZ-1);
            //snprintf( ifreak.ifr_name, IFNAMSIZ, "wifi0" );
            //printf("\n%s\n \n%s\n  ", ifreak.ifr_name,ifreak.ifr_addr );
            
    /*Accessing network interface information by passing address using ioctl.*/
    ioctl( sock, SIOCGIFADDR, &ifreak );
    
// binds the socket to the address of the host and the port number
if (bind(sock, (struct sockaddr *)&server, length) < 0)
{
    error("binding");
}

// change socket permissions to allow broadcast
if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
{
    printf("error setting socket options\n");
    exit(-1);
}
        
    //set up broadcast
    addr.sin_addr.s_addr = inet_addr( "128.206.19.255" );//broadcast address
    addr.sin_family = AF_INET;
    addr.sin_port = htons( portno ); // port number
    
    printf( "\nOpen for broadcasting socket at: 128.206.19.255:%d\n",  portno );
        
    fromlen = sizeof(struct sockaddr_in);	// size of structure
    char buffer[MSG_SIZE];
    int n;
    int master = 0;
    unsigned int random = 0;
    srand((unsigned)time(NULL));
    
    pthread_t buttonThread;
    pthread_create( &buttonThread, NULL, (void *) btn_activated, (void *) &sock );

    while (1)
    {
        // bzero: to "clean up" the buffer. The messages aren't always the same length...
        bzero(buffer,MSG_SIZE);		// sets all values to zero. memset() could be used
        
        // receive from a client
        n = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&addr, &fromlen);
        if (n < 0)
        {
                error("recvfrom"); 
        }
        //else
        //{
            printf("Received a datagram. It says: %s\n", buffer);
    // }
        fflush(stdout);
        char Headmaster[MSG_SIZE];
        
        if( !strcmp( buffer, "WHOIS\n") )
        {
            //Master is a yes
            //printf("\nDoing Mastery Stuff\n");
            if(master == 1)
            {
                    printf("I am MASTER\n");
                    sprintf( Headmaster, "%s on board %s is master!", Name, IP_save ); //printing in server
                    sendto( sock, Headmaster, MSG_SIZE, 0, (const struct sockaddr *)&addr, fromlen ); // Sending to client
            }
            
        }
        else if(!strcmp( buffer, "VOTE\n") )
        {
            
            //printf("\nNot master Implementation\n");
            char ReturnVote[18];
            random = 1 + rand()%10;
            
            sprintf( ReturnVote, "\n#%s %u\n", IP_save, random);
           // printf("\nTesting random: %u\n",random);
            
            //unsigned int testnum = random;
            
            addr.sin_addr.s_addr = inet_addr( "128.206.19.255" );
            
            
            n = sendto( sock, ReturnVote, MSG_SIZE, 0, (const struct sockaddr *)&addr, fromlen );
            if( n < 0 )
            {
                error( "Sending Vote" );
            }
        
        }
        
        else if( buffer[0] == '#' )
        {
           // printf("\nTesting to make sure random value is accurate: %u\n",random);
            
            unsigned int Num; // Assigned newrandom number
            unsigned int Ip; // last digit of my IP address
            unsigned int LastAddr;
            
            sscanf( IP_save, "%*u.%*u.%*u.%u %u", &LastAddr );  // u is for Unsigned decimal integer.it normally a pointer.
                        
            sscanf( buffer, "# %*u.%*u.%*u.%u %u",  &Ip, &Num );
            
            printf( "\nVote recieved from : NewIp: %u | NewRandNum: %u | OldIp: %u | RandNum: %u \n", Ip, Num, LastAddr, random);
            
            // If the random value assigned is greater than the current randomvalue
            //then the new random value becomes the master, else no master
                if( ( random > Num )) // my number is greater than friend, then I am master
                {
                    master = 1;
                   // printf("I am Master\n");
                }
                
                else if( random == Num )  //my number is equal to friend, check ips
                {
                        if( Ip >= LastAddr )
                        {
                            master = 1;
                            //printf("I am Master..\n");
                        } 
                        else 
                        {                  
                            master = 0;
                            //printf("I am NOT THE master\n");
                        }
                }
                
                else
                {
                    master = 0;
                   // printf("I am NOT THE master\n");
                }
        }
        else if( buffer[0] == '@' )
        {
            
            int cdev_id, devi;
            static char L_Char;
            
            if( L_Char != buffer[1] )
            {
    
                    if( ( cdev_id = open( CHAR_DEV, O_WRONLY ) ) == -1 )
                    {
                            printf( "Cannot open device for writing %s\n", CHAR_DEV );
                    }
            
                    if( buffer[1] != '\0' )
                    {
                            devi = write( cdev_id, buffer, sizeof( buffer ) );
                    }

                    close( cdev_id );

                    if( master == 1 )
                    {
                            addr.sin_addr.s_addr = inet_addr( "128.206.19.255" );
                    }
            }
            
            L_Char = buffer[1];
        
        }
        
    }
    
    close(sock);

return 0;

}
