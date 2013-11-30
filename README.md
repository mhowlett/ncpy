# ncpy

This is a small utility for copying large files from one computer to another in situations where 
network connectivity is potentially very poor.

I built this because I frequently need to transfer large data files from a server to my laptop and
and frequently want to do this whilst working in cafes in developing contries with very unreliable
internet connections. If the network connection goes bad (as it usually does), I need to be able to
move to a different cafe and have the transfer seemlessly continue from where it left off.


## Development Status

Now able to transfer files, but not useful for the above purpose yet.

## Building

Depends on nanomsg, get this and install.
Change the include directory in the makefile as needed.
make

## Authors

Matt Howlett
