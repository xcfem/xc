# Directory contents
This directory contains the classes that encapsulate the addresses
used to send/recv messages using MPI or Socket channels.

# [Message passing interface](https://en.wikipedia.org/wiki/Message_Passing_Interface "Wikipedia MPI page") 
Message Passing Interface (MPI) is a standardized and portable message-passing standard designed by a group of researchers from academia and industry to function on a wide variety of parallel computing architectures. The standard defines the syntax and semantics of a core of library routines useful to a wide range of users writing portable message-passing programs in C, C++, and Fortran. There are several well-tested and efficient implementations of MPI, many of which are open-source or in the public domain. These fostered the development of a parallel software industry, and encouraged development of portable and scalable large-scale parallel applications. 

# Sockets

## [Unix domain socket](https://en.wikipedia.org/wiki/Unix_domain_socket "Wikipedia IPC socket page")
A Unix domain socket or IPC socket (inter-process communication socket) is a data communications endpoint for exchanging data between processes executing on the same host operating system.

The API for Unix domain sockets is similar to that of an Internet socket, but rather than using an underlying network protocol, all communication occurs entirely within the operating system kernel.

## [Berkeley sockets](https://en.wikipedia.org/wiki/Berkeley_sockets "Wikipedia Berkeley sockets page")
Berkeley sockets is an application programming interface (API) for Internet sockets and Unix domain sockets, used for inter-process communication (IPC). It is commonly implemented as a library of linkable modules.
