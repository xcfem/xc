# def CondContornoNodosLinea(setLinea, condContorno):
#     \sets
#       {
#         \setLinea
#           {
#             \for_each_nod
#               {
#                 tagNodo= tag
#                 \preprocessor{\constraints{\condContorno(tagNodo){}}}
#               }
#           }
#       }
#   }

def CondContornoNodosInterioresLinea(setLinea, constraints, condContorno):
  tags= setLinea.getNodeLayers.getLayer(0).getTagsInteriorNodes()
  for i in tags:
    condContorno(constraints,i)

# def CondContornoNodoDorsalLinea(setLinea, condContorno):
#     \sets
#       {
#         \setLinea
#           {
#             \primer_nodo
#               {
#                 tagNodo= tag
#                 \preprocessor{\constraints{\condContorno(tagNodo){}}}
#               }
#           }
#       }
#   }

# def CondContornoNodoFrontalLinea(setLinea, condContorno):
#     \sets
#       {
#         \setLinea
#           {
#             \ultimo_nodo
#               {
#                 tagNodo= tag
#                 \preprocessor{\constraints{\condContorno(tagNodo){}}}
#               }
#           }
#       }
#   }

def CondContornoNodosExtremosLinea(setLinea, constraints, condContorno):
  fN= setLinea.firstNode.tag
  lN= setLinea.lastNode.tag
  condContorno(constraints,fN)
  condContorno(constraints,lN)

# def CondContornoNodosInterioresSetLineas(nmbSetLineas, condContorno):
#     \nmbSetLineas
#       {
#         \lineas
#           {
#             \for_each
#               {
#                 \for_each_interior_nod
#                   {
#                     tagNodo= tag
#                     \preprocessor{\constraints{\condContorno(tagNodo){}}}
#                   }
#               }
#           }
#       }
#   }
