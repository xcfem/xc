# -*- coding: utf-8 -*-
import xc

# Asigna una carga al nodo asociado a un punto.
def CargaNodoPunto(preprocessor, lp, idPunto, carga):
  tagNodo= preprocessor.getCad.getPoints.get(idPunto).getTagNode
  lp.newNodalLoad(tagNodo,xc.Vector([carga[0],carga[1],carga[2],carga[3],carga[4],carga[5]]))

# Carga los nodos de la lista.
def CargaNodosLista(lp, tagNodos, carga):
  for i in tagNodos:
    lp.newNodalLoad(i,xc.Vector([carga[0],carga[1],carga[2],carga[3],carga[4],carga[5]]))

# Carga los nodos asociados a los puntos de la lista.
def CargaPuntosLista(preprocessor, lp,lstPuntosCarga, carga):
  for i in lstPuntosCarga:
    CargaNodoPunto(preprocessor,lp,i,carga)

# Asigna una carga a los nodos interiores de la línea.
def CargaNodosInterioresLinea(setLinea, loadPattern, carga):
  tags= setLinea.getNodeLayers.getLayer(0).getTagsInteriorNodes()
  for i in tags:
    loadPattern.newNodalLoad(i,carga)

# Asigna una carga a los nodos interiores de la cara.
def CargaNodosInterioresCara(face, lp, carga):
  capa= face.getNodeLayers.getLayer(0)
  tagsIntNodes= capa.getTagsInteriorNodes()
  for i in tagsIntNodes:
    lp.newNodalLoad(i,xc.Vector(carga))

# # Asigna una carga a los nodos del conjunto.
# def CargaNodosSet(setNameNodos, cargaNod):
#     \setNameNodos
#       {
#         \nodos
#           {
#             \for_each
#               { \load{cargaNod} }
#           }
#       }
#   }

# # Asigna una masa a los nodos interiores de la línea.
# def MasaNodosInterioresLineaRMPlana(nmbLinea, masa):
#     \sets
#       {
#         \nmbLinea
#           {
#             \for_each_interior_nod
#               {
#                 \mass{[[masa,0,0],[0,masa,0],[0,0,masa]]}
#               }
#           }
#       }
#   }

