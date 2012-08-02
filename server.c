/* 
 * Onyedi Anyansi, WPI
 * Fall 2011
 * 
 * server.c - server for distribuited shell program.
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

char message[1024];
int bytes;
int sock, serv_host_port, clilen, newsock;
char hostname[128];
struct sockaddr_in cli_addr, serv_addr;

int run (int asock, int pbool) {
   if (asock < 0) {
      perror("accepting connection");
      return 0;
   }
   else {
      int pid, status, closeStatus;
      closeStatus = 0;
      pid = fork();

      if (pid == 0) {
         if (pbool) puts("forked child");
         /* read data until no more */
         bytes = read(asock, message, 1024);
         message[bytes] = '\0'; /* do this just so we can print as string */
         if (pbool) printf("password received: '%s'\n", message);

         if (bytes == -1) perror("error in read");
         else if (strcmp(message, "pass") == 0) {
            if (pbool) puts("correct password");
            if (write(asock, "passok", strlen("passok")) == -1) {
               perror("write failed");
            }
         }
         else {
            if (pbool) puts("incorrect password");
            if (write(asock, "passsucks", strlen("passsucks")) == -1) {
               perror("write failed");
            }
            close(asock);
         }


         bytes = read(asock, message, 8182);
         message[bytes] = '\0'; /* do this just so we can print as string */
         if (pbool) printf("given command: '%s'\n", message);

         if (bytes == -1) perror("error in read");
         else if (strcmp(message, "test") == 0) {
            if (pbool) puts("test");
            if (write(asock, "test", strlen("test")) == -1) {
               perror("write failed");
            }
            close(asock);
         }
         else if (strcmp(message, "close") == 0) {
            if (pbool) puts("closing");
            if (write(asock, "close", strlen("close")) == -1) {
               perror("write failed");
            }
            close(asock);
            closeStatus = 1;
         }
         else {
            char *pch;
            char *cmdargs[64];
            int i = 1;
            cmdargs[0] = strtok (message," ");

            //parse the command arguments
            while (1)
            {
               pch = strtok (NULL, " ");
               if (!(pch)) {
                  cmdargs[i] = NULL;
                  break;
               }
               strcpy(cmdargs[i], pch);
               if (pbool) printf ("cmdargs[%d] is %s\n",i,cmdargs[i]);
               i++;
            }
            if (pbool) printf("executing command: '%s'\n", cmdargs[0]);
            dup2(asock, STDOUT_FILENO);
            dup2(asock, STDERR_FILENO);
            execvp(cmdargs[0], cmdargs);
         }
      } 

      wait(&status);
      if (pbool) puts("closing asock");
      close(asock);
      return closeStatus;
      //int newersock = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
      //run(newersock);
   }
}

int main(int argc, char *argv[]) {
   int c, hflag, kflag;
   char *pfile = "4444";
   hflag = 0;
   kflag = 0;
   while ((c = getopt (argc, argv, "hkp:")) != EOF) {
      switch (c) {
      case 'p':
         pfile = optarg;
         break;
      case 'k':
         kflag++;
         break;
      case 'h':
         hflag++;
         break;
      }
   }
   if (hflag) {
      printf("Distributed shell server\n");
      printf("usage: server\n");
      printf("[-p #]\tport server is on (default is 4444)\n");
      printf("[-k #]\tto keep the server accepting connections indefinitely\n");
      printf("[-h]\tthis help message and for printing messages pertaining to commands\n");
      gethostname(hostname, sizeof hostname);
      printf("Server hostname: %s\n", hostname);
   }


   serv_host_port = atoi(pfile);

   printf("Listen activating.\n");
   printf("Trying to create socket at port %d...\n", serv_host_port);   

   /* Create socket from which to read */
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("can't open stream socket");
      exit(1);
   }
   
   /* bind our local address so client can connect to us */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   serv_addr.sin_port = htons(serv_host_port);
   if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("can't bind to local address");
      exit(1);
   }

   printf("Socket created! Accepting connections.\n\n");

   listen(sock, 5); /* mark the socket as passive, and sets backlog num */
   clilen = sizeof(cli_addr);
   int closeStatus;
   /* wait here for connections */
   if (kflag) {
      while (closeStatus != 1) {
         newsock = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
         if (hflag) closeStatus = run(newsock, 1);
         else closeStatus = run(newsock, 0);
         //("%d\n",closeStatus);
         if (closeStatus == 1) break;
      }
   }
   else {
      newsock = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
      if (hflag) run(newsock, 1);
      else run (newsock, 0);
   }
   /*
   int pid;
   while (pid = waitpid(-1, NULL, 0)) {
      if (errno == ECHILD) {
         break;
      }
   }
   */
   close(sock);
}
