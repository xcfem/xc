# Preprocessor

As in any finite element analysis, the analyst's first step is to subdivide the body under study into elements and nodes, to define loads acting on the elements and nodes, and to define constraints acting on the nodes. The Preprocessor is the object in the program responsible for building the finite element model (elements, nodes, load patterns, time series loads and constraint objects.

The tools that generate the finite element model are organized as follows:

  - multi_block_topology: geometric entities that define the block topology model (k-points, lines, surfaces and blocks). You can find some simple examples of use [here](https://github.com/xcfem/xc/tree/master/verif/tests/preprocessor/geom_entities).
  - set_management: routines to manage model component sets (nodes, elements, k-points, lines,...). See some simple examples [here](https://github.com/xcfem/xc/tree/master/verif/tests/preprocessor/sets).
  - prep_handlers: objects to interact directly with the model entities containers (node container, element container, load container, sets container, ...). Those are used in every example.
  

