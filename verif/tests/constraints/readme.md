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

## References
 - [Remote Control](https://portwooddigital.com/2026/02/10/remote-control/)
 - [Handle Your Constraints with Care](https://portwooddigital.com/2020/06/05/handle-your-constraints-with-care/)
 - [Support and Connection Types](https://web.mit.edu/4.441/1_lectures/1_lecture13/1_lecture13.html)
 - [Handling of Constraints in Finite-Element Response Sensitivity Analysis](http://jaguar.ucsd.edu/sensitivity/Gu_2009_b.pdf)
 - [Rigid Diaphragm for 2D Models](https://portwooddigital.com/2022/11/05/rigid-diaphragm-for-2d-models/)
 - [Handle Your Constraints with Care](https://portwooddigital.com/2020/06/05/handle-your-constraints-with-care/)
 - [A Rigid Bar Walks Into a Bar](https://portwooddigital.com/2023/04/02/a-rigid-bar-walks-into-a-bar/)
- [Handling a Doubt](https://portwooddigital.com/2023/11/09/handling-a-doubt/)
- [MultiFreedom constraints II](https://quickfem.com/wp-content/uploads/IFEM.Ch09.pdf)
- [What Is a Good Penalty Number?](https://portwooddigital.com/2023/11/29/what-is-a-good-penalty-number/)
 - [Concepts and Applications of Finite Element Analysis](https://books.google.es/books/about/Concepts_and_Applications_of_Finite_Elem.html?id=b8seAQAAIAAJ&redir_esc=y)
 - [No reaction](https://portwooddigital.com/2024/11/20/no-reaction/)
