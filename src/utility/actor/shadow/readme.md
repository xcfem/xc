# Shadow

Object associated with an Actor object which may exist in another process. The Shadow acts like a normal object in the users address space, data and processing that is done by the Shadow may be stored and processed in a remote process, the Actor object resides in this remote address space. The Actor and the Shadow both have a Channel, a communication port. This allows the two to communicate with each other.
