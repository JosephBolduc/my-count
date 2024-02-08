CC = gcc
CCFLAGS = -g -Wall
INCLUDES = 
LIBRARIES = 
EXECUTABLES = memoryA  memoryB  unnamedpipe  namedpipeA  namedpipeB  messageA  messageB signal rpcexample_server  rpcexample_client
RPCGENERATED = rpcexample.h rpcexample_xdr.c rpcexample_svc.c rpcexample_clnt.c


# Command for creating an executable file
.c:
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ $@.c $(LIBRARIES)

# Rule for generating .o file from .c file
%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) -c $^ 

# All files to be generated
all: $(EXECUTABLES) 


rpcexample_server: rpcexample_xdr.o rpcexample_svc.o rpcexample_server.o
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ $^ $(LIBRARIES) 

rpcexample_client: rpcexample_xdr.o rpcexample_clnt.o rpcexample_client.o
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@  $^ $(LIBRARIES) 

rpcexample_xdr.c: rpcexample.x
	rpcgen rpcexample.x

rpcexample_svc.c: rpcexample.x
	rpcgen rpcexample.x

rpcexample_clnt.c: rpcexample.x
	rpcgen rpcexample.x

# Clean the directory
clean: 
	rm -rf $(EXECUTABLES) $(RPCGENERATED) *.o *.dSYM

