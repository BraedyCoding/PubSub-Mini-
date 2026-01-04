# PubSub-Mini-
This is a simple project that uses concurency, locks, mutexes and networking using linux POSIX system calls to create sockets.


A broker object (made for pub/sub operations) was created, and a messaging class which serializes and deserializes the data to be transfered over TCP.
A user first connects to the listening socket, and they are created a session object and 2 threads (one thread is constantly checking for input), and the other thread looks sleeps awaiting a publish to occur (using a messaging queue and std::condition_variable).

When a user subscribes or publishes (writes to a socket), and read_loop thread processes it, it will call on one of the broker operations.

There is so much more to talk about, but that is what it does at a really simple level.
