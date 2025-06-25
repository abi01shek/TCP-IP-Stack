# TCP-IP-Stack
My implementation of TCP IP stack

## Adding a command line interface
I am using an open sourced [Command Parser library](https://github.com/sachinites/CommandParser) to inegrate a CLI for user to interact with the network. The CLI supports the following commands
 * `show topo`: prints all nodes in the topology along with their connection details
 * `run node <node-name> resolve-arp <ip-address>`: IP to MAC address ARP resolution.


## Simulating communication between nodes
To simulate communication between the nodes, on one side we have
the CLI that is used to initiate transactions and on the other side
we have a thread that will simulate reception of messages by corresponding
nodes. Each node will be bound to a socket and be assigned a loopback address and unique port. When a node wants to send a message to another node, the sender node will open a UDP socket and send a message to the destination's address. A thread that runs the command line interface will be used to initiate these send operations.


The receive operations are handled by another thread. This thread has an epoll with all the socket FDs in it. Whenever the socket FDs become readable, it will process the data. This is akin to the receiver node processing the data. This is the underlying communication infrastructure to simulate communication between nodes.

### Steps
1. Each node has a socket FD as parameter
2. When node is created, a new listen UDP socket must be opened for that node with a unique port ID and bound to the socket. This socket FD is stored into the node data structure (`comm.c`)
3. Next we want to create the thread that handles reception of data from these sockets. This is done in as soon as the topology is created (`topologies.c`)
4. This thread adds all the socket FDs into an epoll and waits for any of them to become readable. When ready it just reads the data and processes it. For now the received data is printed to the screen

### Testing
To recap, each node in the topology is assigned a port number (incrementally starting from 40000 in our case) and a UDP socket is bound to it. When the program is running, a thread is spawned that waits for messages on these sockets to print them.

First to check if the UDP connections are open we can use netstat
```bash
netstat -u -a # show all UDP sockets
```

It shows there are 3 UDP sockets with assigned port numbers
```
udp        0      0 0.0.0.0:40000           0.0.0.0:*
udp        0      0 0.0.0.0:40001           0.0.0.0:*
udp        0      0 0.0.0.0:40002           0.0.0.0:*
```

To communicate with these open sockets we can use netcat (nc). Since the port is open on the current machine, we just use local host as destination IP and the socket's port as destination port
```bash
echo "hi" | nc -u 127.0.0.1 40000
```

This data is received by the running thread and printed on screen.



### Sending data from one node to another
A data is sent on a link which connects one interface to another. Each interface is connected to one other interface through a link. Thus given an interface and a port number we can identify the node to send the data to. Then as in the test case above, we can write the data using a UDP socket.

Thus given an interface to send packet via, the link of that interface is got and the destination interface is got from the link. From the destination interface we get the node attached to it and get its port number before we create a UDP socket and send data to this port. Inorder to identify the interface on which a node receives this packet, we encapuste the RX interface name as the header followed by the data as the payload

The thread that epolls on these sockets will receive the data, parse the header and call the appropriate handler of that message at that node.
