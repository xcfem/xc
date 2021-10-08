# Mesh

The mesh object is responsible for storing the mesh components (nodes and elements) created by the Preprocessor object and for providing the Domain and Recorder objects access to these objects.

This directory contains the classes that define the mesh components: nodes, elements and its containers. 

## Contents

		- element: contains the element classes, all in subdirectories, i.e. the Truss.h and Truss.C files are in a subdirectory truss.
        - node: the Node and NodalLoad classes.
		- region: nodes and elements belonging to a mesh region.
