# -*- coding: utf-8 -*-
# \mdlr
#   {
#     \cad
#       {
#         # Cargamos las celdas en vtk
#         \numera
#         \for_each_edge
#           {
#             \vtk
#               {
#                 \ugrid{\insert_next_cell[getVtkCellType]{getVertices}}
#               }
#           }
#       }
#   }
