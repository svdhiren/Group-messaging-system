# Group-messaging-system

The entities in the system are a server and multiple clients in the form of groups.

The clients in a particular group can communicate with each other with the help of the server using pipes and message queues.

The clients in a particular group are identified using their unique Process IDs by the server.

Since each client can send and receive the messages, and server can receive messages from multiple groups, all process are multithreaded for efficiency.

I additionally added a separated thread to accept broadcasting requests from clients. So the messages sent by clients will shared across all the groups!


Played around a lot and learnt a ton about Interprocess communication, multithreading and system level C programming!!
