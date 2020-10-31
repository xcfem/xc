# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

# Return the identifiers of the lines in the set.
def getIdsLineasSet(preprocessor, setName):
    retvalIdsLineasSet= [] 
    # Formamos la lista de identificadores de línea.
    st= preprocessor.getSets.getSet(setName)
    lineas= st.getLines()
    for l in lineas:
      retvalIdsLineasSet.append(l.code)
    return retvalIdsLineasSet

def getTagsNodesFromSet(preprocessor, setName):
    '''Return the tags of the nodes from a set.'''
    retvalSetNodeTags= [] 
    st= preprocessor.getSets.getSet(setName)
    nodes= st.nodes
    for n in nodes:
      retvalSetNodeTags.append(n.tag)
    return retvalSetNodeTags


# Return the tags de los elementos de un conjunto.
def getTagsElementosSet(preprocessor,setName):
    retvalTagsElementosSet= []
    st= preprocessor.getSets.getSet(setName)
    elems= st.elements
    for n in elems:
      retvalTagsElementosSet.append(n.tag)
    return retvalTagsElementosSet

def getNodesInCoordinateRegion(xmin,xmax,ymin,ymax,zmin,zmax,xcSet):
    '''return a list with the the nodes in a cubic region defined by 2 points
    with coordinates (xmin,ymin,zmin) and (xmax,ymax,zmax)

    :param xmin,ymin,zmin,xmax,ymax,zmax: coordinates delimiting the region
    :param xcSet:   xc set of nodes to which restrict the search 

    '''
    retval=list()
    setNodes=xcSet.nodes
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

def get_attached_PhModElems(elemTag,setElPhMod):
    '''This function returns an ordered (by number of section) list with the 
    elements of the phantom model (or a set of its elements) that are associated 
    with the element of the actual model whose tag is passed as a parameter

    :param elemTag:   tag of the element for which the function returns the 
                      Phantom Model elements associated   
    :param selElPhMod: set of elements of the phantom model to which limit
                      the search
    '''
    #notation: AM actual model, PM phantom model
    retval=list()
    tmpSectList=list()
    for e in setElPhMod:
      tgElass=e.getProp("idElem") #tag of the AM element associated 
      if tgElass==elemTag:
        tmpSectList.append(e.getProp("dir")) #number of section
        retval.append(e)
    #sorting list of elements by number of section
    tmpSectList,retval=zip(*sorted(zip(tmpSectList,retval)))
    return retval
  
def distance_2points(preprocessor,tagPt1,tagPt2):
    '''return the distance between two 3D points

    :param preprocessor:  preprocessor
    :param tagPt1: tag that identifies the first point 
    :param tagPt2: tag that identifies the second point 
    '''
    points= preprocessor.getMultiBlockTopology.getPoints
    pA= points.get(tagPt1).getPos
    pB= points.get(tagPt2).getPos
    return pA.dist(pB)
  
