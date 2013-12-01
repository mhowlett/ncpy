# ncpy

This is a utility for copying large files from one computer to another in situations where 
network connectivity is potentially very poor. I built this because I frequently need to 
transfer large data files from a server to my laptop and
frequently want to do this whilst working in cafes in developing contries with very unreliable
internet connections. If the network connection goes bad or slow or I suddenly want to leave
because someone started talking loudly, I want to be able to move to a different
cafe and have the transfer seemlessly continue from where it left off.


### Usage

On the server

    ncpy -s PATH-OF-FILE-TO-SEND
    
On the client:

    ncpy -r ADDRESS-TO-RECEIVE-FROM

ADDRESS-TO-RECEIVE-FROM is either an IP address or fully qualified domain name.

ncpy uses port 9005. This is currently not configurable. 

In order to prevent a transfer aborting, the only thing that must be done is to
ensure the client is not terminated.

It is ok to put a latop running the client to sleep. Everything will continue as 
normal when the laptop is woken up and regains network access.

It is ok to run the server in a shell which could potentially hang if the connection
is lost. Simply start the server up again if it dies in this way. No need to use
virtual terminal manager such as screen.

The server will automatically shut down if it has been idle for too long. If this
happens and you still want to continue the transfer, simply start the server up
again. Note however that the file must not have changed between server restarts.

If you want to pause the transfer, you can kill the server and restart it when you
want to resume.


### Development Status

Appears to be working as intended after some basic testing.


### Building

Depends on nanomsg, get this and install.

Change the include directory in the makefile as needed.

make

### Authors

Matt Howlett
