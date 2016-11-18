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

def getNodesInCoordinateRegion(xmin,xmax,ymin,ymax,zmin,zmax,xcSet):
  ''':returns: a list with the the nodes in a cubic region defined by 2 points
  with coordinates (xmin,ymin,zmin) and (xmax,ymax,zmax)

  :param xmin,ymin,zmin,xmax,ymax,zmax: coordinates delimiting the region
  :param xcSet:   xc set of nodes to which restrict the search 

  '''
  retval=list()
  setNodes=xcSet.getNodes
  for n in setNodes:
    vCoord=n.getCoo
    xNod=vCoord[0]
    if xNod >= xmin:
      if xNod <= xmax:
        yNod=vCoord[1]
        if yNod >= ymin:
          if yNod <= ymax:
            zNod=vCoord[2]
            if zNod >= zmin:
              if zNod <= zmax:
                retval.append(n)
  return retval
