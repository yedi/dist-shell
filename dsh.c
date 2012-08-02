/* 
 * Onyedi Anyansi, WPI
 * Fall 2011
 *
 * dsh.c - Client portion of distributed shell program
 */ 

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff /* should be in <netinet/in.h> */
#endif

int main(int argc, char *argv[]) {
  char buf[80]; 
  unsigned long int inaddr;
  int sock, serv_host_port, c;
  struct sockaddr_in serv_addr;
  char *serv_host_addr;
  struct hostent *hp;

  char *cfile = NULL, *sfile = "ccc.wpi.edu", *pfile = "4444";
  int hflag = 0, cflag = 0;

  while ((c = getopt (argc, argv, "hc:s:p:")) != EOF) {
    switch (c) {
    case 'c':
      cflag++;
      cfile = optarg;
      break;
    case 's':
      sfile = optarg;
      break;
    case 'p':
      pfile = optarg;
      break;
    case 'h':
      hflag++;
      break;
    }
  }

    if (hflag) {
      printf("Distributed shell client\n");
      printf("usage: dsh\n");
      printf("{-c command}\tcommand to execute remotely\n");
      printf("[-s host]\thost server is on (default is ccc.wpi.edu)\n");
      printf("[-p #]\tport server is on (default is 4444)\n");
      printf("[-h]\tthis help message\n");
   }
  if(!(cflag)) {
    printf("Please specify a command\n");
    exit(1);
  }
   serv_host_addr = sfile;
   serv_host_port = atoi(pfile);

   printf("Talk activated.\n\n");
   printf("Trying to connect to server %s at port %d...\n", 
	  serv_host_addr, serv_host_port);

   /*
    * First try to convert the host name as a dotted-decimal number.
    * Only if that fails do we call gethostbyname().
    */
   bzero((void *) &serv_addr, sizeof(serv_addr));
   printf("Looking up %s...\n", serv_host_addr);
   if ((hp = gethostbyname(serv_host_addr)) == NULL) {
     perror("host name error");
     exit(1);
   }
   bcopy(hp->h_addr, (char *) &serv_addr.sin_addr, hp->h_length);

   printf("Found it.  Setting port connection to %d...\n", serv_host_port);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(serv_host_port);
   /* serv_addr.sin_addr.s_addr = inet_addr(serv_host_addr); */

   /* open a TCP socket (an Internet stream socket). */
   puts("Done. Opening socket...");
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
     perror("opening socket");
     exit(1);
   }

   /* socket open, so connect to the server */
   puts("Open. Trying connection to server...");
   if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
     perror("can't connect");
     exit(1);
   }

   printf("Connection established!\n");
   //printf("Type in messages to send to server.\n");

   
   /* read from stdin, sending to server, print responses, until quit */
   /*
   printf("Type in the password:\n");
   fgets(buf, 80, stdin);
   buf[strlen(buf)-1] = '\0'; /* remove last \n */
   
   if (write(sock, "pass", strlen("pass")) == -1) {
     perror("write failed");
   }
    int bytes;
    char message[1024];
    bytes = read(sock, message, 1024);
    message[bytes] = '\0'; /* do this just so we can print as string */
    printf("password status: '%s'\n", message);

    if (bytes == -1) perror("error in read");
    else if (strcmp(message, "passok") == 0) {
      printf("running %s\n", cfile);
      if (write(sock, cfile, strlen(cfile)) == -1) {
        perror("write failed");
      }
    }

    bytes = read(sock, message, 8096);
    message[bytes] = '\0'; /* do this just so we can print as string */
    printf("cmd output: \n%s\n", message);

   close(sock);

   exit(0);
}
