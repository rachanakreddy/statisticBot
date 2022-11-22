#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 8192 /* Max text line length */

int open_clientfd(char *hostname, int port) {
  // The client's socket file descriptor.
  int clientfd;

  // The hostent struct is used to get the IP address of the server
  // using DNS.
  //
  // struct hostent {
  //   char *h_name;        // official domain name of host
  //   char **h_aliases;    // null-terminated array of domain names
  //   int  h_addrtype;     // host address type (AF_INET)
  //   int  h_length;       // length of an address, in bytes
  //   char **h_addr_list;  // null-terminated array of in_addr structs
  // };
  struct hostent *hp;

  // serveraddr is used to record the server information (IP address
  // and port number).
  //
  // struct sockaddr_in {
  //   short            sin_family;   // e.g. AF_INET
  //   unsigned short   sin_port;     // e.g. htons(3490)
  //   struct in_addr   sin_addr;     // see struct in_addr, below
  //   char             sin_zero[8];  // zero this if you want to
  // };
  struct sockaddr_in serveraddr;

  printf("Echo Client is creating a socket.\n");

  // First, we create the socket file descriptor with the given
  // protocol and protocol family.
  if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

  // Query DNS for the host (server) information.
  if ((hp = gethostbyname(hostname)) == NULL) return -2;

  // The socket API requires that you zero out the bytes!
  bzero((char *)&serveraddr, sizeof(serveraddr));
 
// Record the protocol family we are using to connect.
  serveraddr.sin_family = AF_INET;

  // Copy the IP address provided by DNS to our server address
  // structure.
  bcopy((char *)hp->h_addr_list[0], (char *)&serveraddr.sin_addr.s_addr,
        hp->h_length);

  // Convert the port from host byte order to network byte order and
  // store this in the server address structure.
  serveraddr.sin_port = htons(port);

  printf("Echo Client is trying to connect to %s (%s:%d).\n", hostname,
         inet_ntoa(serveraddr.sin_addr), port);

  // Establish a connection with the server.
  if (connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    return -1;

  printf("Echo Client connected.\n");

  // Return the connected file descriptor.
  return clientfd;
}

int main(int argc, char **argv) {
  // The client socket file descriptor.
  int clientfd;

 // The port number.
  int port;

  // Variable to store the host/server domain name.
  char *host;

  // A buffer to receive data from the server.
  char buf[MAXLINE];

  // First, we check the program arguments:
  if (argc != 4) {
    fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
    exit(0);
  }

  // First argument is host, second is port:
  host = argv[3];
  port = atoi(argv[2]);

  // Open the client socket file descriptor given the host and port:
  clientfd = open_clientfd(host, port);

  char *netid = argv[1];
  
  char init[1000];
  strcpy(init, "cs230 HELLO ");
  strcat(init, netid);
  strcat(init, "\n");
  
  printf("%s", init);
  ssize_t ns = send(clientfd, init, strlen(init), 0);


  int i = 0;
  ssize_t nr;

  while ((nr = recv(clientfd, buf, MAXLINE, 0)) > 0) {
     char rand[230];
     char status[1000];
     char op[10];
     int num1, num2, num3, num4, num5;

     int answer = -1;

     
     // Remove the newline from fgets:
    buf[strcspn(buf, "\n")] = '\0';

    sscanf(buf, "%s %s", rand, status);
   if(strlen(status) == 64){
	printf("Captured the flag!\n");
	break;
   } 

   else
    {
      sscanf(buf, "%s %s %s %d %d %d %d %d", rand, status, op, &num1, &num2, &num3, &num4, &num5);

     int numArr[5] = {num1, num2, num3, num4, num5};
     int temp;

     //sorts array to make answers easier to calculate
     for(int j = 0; j < 5; j++){
	for(int k = j+1; k < 5; k++){
         if(numArr[j] > numArr[k]){
           temp = numArr[j];
           numArr[j] = numArr[k];
           numArr[k] = temp;
         }
	}
    }
     
    char mid = op[1];
      switch (mid)
      {
      case 'i':
        answer = numArr[0];
        break;
      case 'a':
        answer = numArr[4];
        break;
      case 'e':
        answer = numArr[2];
        break;
      default:
        break;
      }
    }
    char reply[1000];
    sprintf(reply, "cs230 %d\n", answer);
    ns = send(clientfd, reply, strlen(reply), 0);
  }

  // Close the file descriptor:
  close(clientfd);
  exit(0);
}
