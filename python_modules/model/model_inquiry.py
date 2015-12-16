# -*- coding: utf-8 -*-

# Devuelve los identificadores de las lineas de un conjunto.
def getIdsLineasSet(preprocessor, setName):
  retvalIdsLineasSet= [] 
  # Formamos la lista de identificadores de línea.
  st= preprocessor.getSets.getSet(setName)
  lineas= st.getLines()
  for l in lineas:
    retvalIdsLineasSet.append(l.codigo)
  return retvalIdsLineasSet

# Devuelve los tags de los nodos de un conjunto.
def getTagsNodosSet(preprocessor, setName):
  retvalTagsNodosSet= [] 
  st= preprocessor.getSets.getSet(setName)
  nodos= st.getNodes()
  for n in nodos:
    retvalTagsNodosSet.append(n.tag)
  return retvalTagsNodosSet


# Devuelve los tags de los elementos de un conjunto.
def getTagsElementosSet(preprocessor,setName):
  retvalTagsElementosSet= []
  st= preprocessor.getSets.getSet(setName)
  elems= st.getElements()
  for n in elems:
    retvalTagsElementosSet.append(n.tag)
  return retvalTagsElementosSet
