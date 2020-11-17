# Parallel classes

The classes in this folder are intended to facilitate the development of parallel object-oriented finite element programs. The classes are:

- **Shadow** - A _Shadow_ object represents a remote actor object
in the local actor process.

- **Actor** - An _Actor_ object is a local object in the remote
actor process. It performs the operations requested of it by the
_Shadow_ object. The actor objects in an aggregation collectively
perform the analysis operations by communicating between themselves.

- **Channel** - The _Shadow_ and _Actor_ objects communicate with 
each other through _Channel_ objects. A _Channel_ object represents a
point in a local actor process through which a local object can send
and receive information.

- **Address** - An _Address_ object represents the location of a
_Channel_ object in the machine space. _Channel_ objects send
information to other _Channel_ objects, whose locations are given by
an _Address_ object. _Channel_ objects also receive
information from other _Channel_ objects, whose locations are
given by an _Address_ object.  

- **MovableObject** - A _MovableObject_ is an object which can
send its state from one actor process to another.

- **ObjectBroker** - An _ObjectBroker_ is an object in a local
actor process for creating new objects.

- **MachineBroker** - A _MachineBroker_ is an object in a local
actor process that is responsible for creating remote actor processes
at the request of _Shadow_ objects in the same local process.

