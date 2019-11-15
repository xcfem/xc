Tools that generate the FE model (mesh, constraints, loads,...):

  - multi_block_topology: geometric entities that define the block topology model (k-points, lines, surfaces and blocks). You can find some simple examples of use [here](https://github.com/xcfem/xc/tree/master/verif/tests/preprocessor/geom_entities).
  - set_management: routines to manage model component sets (nodes, elements, k-points, lines,...). See some simple examples [here](https://github.com/xcfem/xc/tree/master/verif/tests/preprocessor/sets).
  - prep_handlers: objects to interact directly with the model entities containers (node container, element container, load container, sets container, ...). Those are used in every example.
  

