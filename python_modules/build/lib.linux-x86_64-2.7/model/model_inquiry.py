# -*- coding: utf-8 -*-

# Devuelve los identificadores de las lineas de un conjunto.
def getIdsLineasSet(mdlr, nmbSet):
  retvalIdsLineasSet= [] 
  # Formamos la lista de identificadores de línea.
  st= mdlr.getSets.getSet(nmbSet)
  lineas= st.getLines()
  for l in lineas:
    retvalIdsLineasSet.append(l.codigo)
  return retvalIdsLineasSet

# Devuelve los tags de los nodos de un conjunto.
def getTagsNodosSet(mdlr, nmbSet):
  retvalTagsNodosSet= [] 
  st= mdlr.getSets.getSet(nmbSet)
  nodos= st.getNodes()
  for n in nodos:
    retvalTagsNodosSet.append(n.tag)
  return retvalTagsNodosSet


# Devuelve los tags de los elementos de un conjunto.
def getTagsElementosSet(mdlr,nmbSet):
  retvalTagsElementosSet= []
  st= mdlr.getSets.getSet(nmbSet)
  elems= st.getElements()
  for n in elems:
    retvalTagsElementosSet.append(n.tag)
  return retvalTagsElementosSet
