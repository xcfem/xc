# -*- coding: utf-8 -*-

import xc

print "define_apoyos DEPRECATED use predefined_spaces"



# # Coloca apoyos en todos los nodos de la lista (numera los elementos con el mismo índice de los nodos).
# def defApoyoNodosListaXYZ(preprocessor,tagNodos, tagElementos, matKXName, matKYName, matKZName):
#   sz= len(tagNodos)
#   nodosNuevos= []
#   for i in range(0,sz):
#     nodosNuevos.append(defApoyoXYZ(preprocessor,tagNodos[i],tagElementos[i],matKXName,matKYName,matKZName))
#   return nodosNuevos

# # Coloca apoyos en todos los nodos de la lista de puntos.
# def defApoyoPuntos(lstPuntosApoyo, offset, matKX, matKY, matKZ):
#   nodosApoyo= [] 
#   elementosApoyo= [] 
#   szListaPuntos= lstPuntosApoyo.size
#   # Formamos la lista de nodos de arranque.
#   for ii in range(0,szListaPuntos+1):
#     pnt= preprocessor.getCad.getPnt(lstPuntosApoyo[ii])
#     tagNodo= pnt.getTagNodo()
#     nodosApoyo.append(tagNodo)
#     elementosApoyo.append(tagNodo+offset)
#   nodosArranque= defApoyoNodosLista(nodosApoyo,elementosApoyo,matKX,matKY,matKZ)
#   return nodosArranque

# def colocaApoyoEntrePuntos(preprocessor,iPtA, iPtB, iElem, matKX, matKY, matKZ, matKTHX, matKTHY, matKTHZ):
#   '''Define un elemento entre los nodos pertenecientes a los puntos cuyos tags se pasan como parámetros.'''
#   tgNodA= preprocessor.getCad.getPnt(iPtA).getTagNode()
#   tgNodB= preprocessor.getCad.getPnt(iPtB).getTagNode()
#   colocaApoyoNodos(tgNodA,tgNodB,iElem,matKX,matKY,matKZ,matKTHX,matKTHY,matKTHZ)
