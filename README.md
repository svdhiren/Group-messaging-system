# Group-messaging-system


![Messaging system](https://github.com/svdhiren/Group-messaging-system/assets/84956418/30af4877-14a9-454c-851b-c37831aaa3d4)

The entities in the system are a server and multiple clients in the form of groups.

The clients in a particular group can communicate with each other with the help of the server using pipes and message queues.

The clients in a particular group are identified using their unique Process IDs by the server.

Since each client can send and receive the messages, and server can receive messages from multiple groups, all process are multithreaded for efficiency.

I additionally added a separated thread to accept broadcasting requests from clients. So the messages sent by clients will shared across all the groups!


Played around a lot and learnt a ton about Interprocess communication, multithreading and system level C programming!!


