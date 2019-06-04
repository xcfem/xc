Interaction diagrams for reinforced concrete sections
=====================================================

## Overview
Three and two-dimensional <a href="https://github.com/xcfem/xc/blob/master/doc/screenshots/3D_interaction_diagram.png" target="_new">interaction diagrams</a> for reinforced concrete sections fiber models.

## Contents
- Pivots.h (Pivots.cc): positions of the pivots computed for a bending plane.
- PivotsUltimateStrains.h (.cc): definition of the ultimate strains for each of the A, B and C pivots.
- ComputePivots.h (.cc): compute the position of the pivots given a bending plane.
- NormalStressStrengthParameters.h (.cc): parameters that define the ultimate limit states of a reinforced concrete section under normal stresses (bending and/or compression).
- InteractionDiagramData.h (.cc): parameters needed to obtain the interaction diagram. You can set here the ultimate strains for the pivots according to the different standards.
- InteractionDiagram2d.h (.cc): two-dimensional interaction diagram for a reinforced concrete section.
- InteractionDiagram.h (.cc): three-dimensional interaction diagram for a reinforced concrete section.
- DeformationPlane.h (.cc): deformation plane for a generic section.
- ClosedTriangleMesh.h (.cc): geometric representation of the three-dimensional interaction diagram.
- NMPointCloudBase.h (.cc): base class for the point cloud structure. Each point in the cloud represents a (N,M) or (N,My,Mz) tuple.
- NMPointCloud.h (.cc): 2D point cloud structure. Each point in the cloud represents a (N,M) pair.
- NMyMzPointCloud.h (.cc): 3D point cloud structure. Each point in the cloud represents a (N,My,Mz) triplet.
-python_inteface.tcc: boost.python code to expose all those clases to Python. 

