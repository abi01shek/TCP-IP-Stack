# TCP-IP-Stack
My implementation of TCP IP stack


## Simulating communication between nodes
To simulate communication between the nodes, on one side we have
the CLI that is used to initiate transactions and on the other side
we have a thread that will simulate reception of messages by corresponding
nodes.

To simulate the communication, each node will be bound to a socket and be
assigned a loopback address and unique port. When a node wants to send a message to another node, the sender node will open a UDP socket and send a message to the destination's address. A thread that runs the command line interface will be used to initiate these send operations.


The receive operations are handled by another thread. This thread has an epoll with all the socket FDs in it. Whenever the socket FDs become readable, it will process the data. This is akin to the receiver node processing the data. This is the underlying communication infrastructure to simulate communication between nodes.

Steps
1. Each node has a socket FD as parameter
2. when node is created, a new listen UDP socket must be opened for that node with a unique port ID and bound to the socket. This socket FD is stored into the node data structure
3. Next we want to create the thread that handles reading of data from these sockets.
4. For this in main we create a detached thread.
5. This thread adds all the socket FDs into an epoll and waits for any of them to become readable. When ready it just reads the data and processes it. 
