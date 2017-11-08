#include <stdio.h>      // Default System Calls
#include <stdlib.h>     // Needed for OS X
#include <string.h>     // Needed for Strlen
#include <string>
#include <sys/socket.h> // Needed for socket creating and binding
#include <netinet/in.h> // Needed to use struct sockaddr_in
#include <time.h>       // To control the timeout mechanism
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <ios>
#include <sys/time.h>
#define EXPR_SIZE   1024
#define BUFLEN      512
#define TRUE        1
#define SERVERLEN   1024
#define PORTNO 42069


int main( void )
{
    printf("Program started.\r\n");
    int fd;
    if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        printf( "socket failed" );
        return 1;
    }

    srand(time(NULL));

    struct sockaddr_in serveraddr;
    memset( &serveraddr, 0, sizeof(serveraddr) );
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons( PORTNO  );
    serveraddr.sin_addr.s_addr = htonl( INADDR_ANY );

    if ( bind(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0 ) {
        printf( "bind failed\r\n" );
        return 1;
    }

    printf("Server started.\r\n");
    fflush(stdout);
    int sz = 48;
    char buffer[sz];
    while(true){
        int length = recvfrom( fd, buffer, sz, 0, NULL, 0 );
	//int length = recv(fd, buffer, sz-1, NULL); TCP
        if ( length < 0 ) {
            printf( "recvfrom failed\r\n" );
            break;
        }

	// Reverse ""endianness""
 	unsigned char hurg[32];
	for (int i = 0; i < 32; i++)
	{
		hurg[i] = buffer[31 - i];
	}
	double* d = reinterpret_cast<double*>(hurg);
	// Reverse buffer itself
	double dd[4];
	for (int i = 0; i < 4; i++)
	{
		dd[i] = d[3 - i];
	}

	unsigned char nickname[17];
	memcpy(nickname, buffer + 32, 16);
	nickname[16] = 0;

	std::ofstream file;
        std::stringstream s;
	// Seconds
	std::time_t t = std::time(0);
	// Add salt.
	int r = rand();
	int time = (int)t;

        s << "./pings/" << std::to_string(t) << "_" << r;
        file.open(s.str(), std::ios::out | std::ios::binary); // Opens the file by the constructed filename
	std::cout << "!!!-------------------------------!!!" << std::endl;
	fflush(stdout);
	std::cout << "Location in bytes: ";
	for (int i = 0; i < 32; i++)
	{
		std::cout << hurg[i];
		file << hurg[i];
		//std::cout << (reinterpret_cast<unsigned char*>(dd))[i];
		// prints the bytes into the file
		//file << (reinterpret_cast<unsigned char*>(dd))[i];
	} std::cout << std::endl;
	std::cout << std::fixed << std::setprecision(16) << length << " bytes: " << dd[0] << " " << dd[1] << " " << dd[2] << " " << dd[3] << std::endl;
	fflush(stdout);
	std::cout << "Nickname: " << nickname << std::endl;
	file << nickname;
	fflush(stdout);
	std::cout << "Time: " << t << std::endl;	
	fflush(stdout);
	std::cout << "!!!-------------------------------!!!" << std::endl;
	file.close();

	int sz = length + sizeof(int);
	unsigned char pingdata[sz];

	//printf("%.3f %.3f %.3f %.3f", d[0], d[1], d[2], d[3]);
	fflush(stdout);
    }

    close( fd );
}
