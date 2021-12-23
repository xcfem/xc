# Constraints verification tests
Constraints limit the motion (translation, rotation, or both) of one or more nodes in the finite element model. In statics, this is known as limiting the degrees of freedom. The main concern here must be that constraints in the model represent adequately the real constraints in the structure.

This folder contain tests relative to constraint creation and removal. All the examples require constraints so simple cases are not treated here.

+ Constraints removal tests: `remove\_fix\_XX.py`.
+ Create elastic bearings: `test_elastic_bearing_XX.py`.
+ Put a fulcrum between two nodes: `test_fulcrum_01.py`.
+ Glue a node to an arbitrary position of a quad element: `test_glue_node_to_element_XX.py`.
+ Basic simulation of soil-structure interaction of a pile: `test_pile_01.py`.
+ Release of some DOF at beam end nodes: `test_release_beam_node_01.py`.
+ Simulation of rigid links between nodes: `test_huge_xxx.py` and `test_rigid_xxx.py`.

