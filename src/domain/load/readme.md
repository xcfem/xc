# Domain loads

Loads on nodes and elements.

## How to add a new load type

The path to do this is as follows:

1. Search for a suitable parent class between the load types already defined. Create a preliminary version of the new class for the load. The methods which actually add the load to the element are `addReactionsInBasicSystem` and `addFixedEndForcesInBasicSystem`, you can see those methods in the existing classes to see how they work.
2. Object serialization: add the new `LOAD_TAG_NewLoadType` values to the [classTags.h](https://github.com/xcfem/xc/blob/master/src/classTags.h) file. Add the header files to the [all_includes.h](https://github.com/xcfem/xc/blob/master/src/utility/actor/objectBroker/all_includes.h) header.
3. Insert the new files in CMakeLists.txt files.
4. Python interface: expose the new class to Python; see the [python interface for loads](https://github.com/xcfem/xc/blob/master/src/domain/load/python_interface.tcc).
5. Add the corresponding command to the function [process_element_load](https://github.com/xcfem/xc/blob/master/src/domain/load/elem_load.cc).
6. Compile the code.
7. Write at least a verification test as simple as possible to check that things are working as expected.
