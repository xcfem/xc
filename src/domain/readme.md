# Domain

The Domain object is responsible for storing the objects created by the Preprocessor object and for providing the Analysis and Recorder objects access to these objects.

This directory contains the classes that define the domain components (nodes, elements, constraints, loads, ...) and its containers. 

## Contents

         - component: abstract base classes for the domain components.
         - constraints: classes that represent the problem constraints.
		 - load: classes that represent the problem loads.
		 - mesh: finite elements mesh (nodes and elements).
         - partitioner: object that can partition and load balance a PartitionedDomain.
