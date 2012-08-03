Distributed Shell Program
=============

Usage
-------------
To compile: 

    ~$ make clean
    ~$ make
  
To run the server: 

    ~$ ./server
    
To run the client: 

    ~$ ./dsh -s <proper server name> -c <command you want to run>
    
-h for more help


Example Usage
-------------

### Server 

    -bash-3.2$ ./server -h
    Distributed shell server
    usage: server
    [-p #]	port server is on (default is 4444)
    [-k #]	to keep the server accepting connections indefinitely
    [-h]	this help message and for printing messages pertaining to commands
    Server hostname: CCCWORK2.WPI.EDU
    Listen activating.
    Trying to create socket at port 4444...
    Socket created! Accepting connections.
    
    forked child
    password received: 'pass'
    correct password
    given command: 'cat makefile'
    cmdargs[1] is makefile
    executing command: 'cat'
    closing asock
    
    
### Client

    ./dsh -s cccwork2.wpi.edu -c "cat makefile"
    Talk activated.
    
    Trying to connect to server cccwork2.wpi.edu at port 4444...
    Looking up cccwork2.wpi.edu...
    Found it.  Setting port connection to 4444...
    Done. Opening socket...
    Open. Trying connection to server...
    Connection established!
    password status: 'passok'
    running cat makefile
    cmd output: 
    # 
    # Makefile for distributed shell program
    #
    
    CC = gcc
    CFLAGS = -Wall
    LIBFLAGS =
    
    all: dsh server
    
    server: server.c 
    	$(CC) $(CFLAGS) server.c -o server $(LIBFLAGS)
    
    dsh: dsh.c 
    	$(CC) $(CFLAGS) dsh.c -o dsh $(LIBFLAGS)
    
    clean:
    	/bin/rm -rf *.o core dsh server
