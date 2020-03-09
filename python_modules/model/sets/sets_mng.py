
# -*- coding: utf-8 -*-


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import geom
from miscUtils import log_messages as lmsg

def get_subset_inside(geomObj,fromSet,toSetName,tol=0.0):
    '''return a subset of fromSet composed by the entities inside the 
    3D geometric figure geomObj.

    :param geomObj: geom object that delimits the subset
    :param fromSet: set from which to extract the subset
    :param toSetName: string to name the created subset
    :param tol: geometric tolerance for the search (defaults to 0.0)
    '''
    newSet=fromSet.getPreprocessor.getSets.defSet(toSetName)
    newSet.surfaces=fromSet.surfaces.pickSurfacesInside(geomObj,tol)
    newSet.lines=fromSet.lines.pickLinesInside(geomObj,tol)
    newSet.points=fromSet.points.pickPointsInside(geomObj,tol)
    newSet.elements=fromSet.elements.pickElemsInside(geomObj,tol)
    newSet.nodes=fromSet.nodes.pickNodesInside(geomObj,tol)
    return newSet

def get_subset_elem_of_type(elemType,fromSet,toSetName):
    '''return a subset of fromSet composed by the elements of type
    elemType

    :param elemType: name of the type of target elements (string)
    :param fromSet: set from which to extract the subset
    :param toSetName: string to name the created subset
    '''
    newSet=fromSet.getPreprocessor.getSets.defSet(toSetName)
    newSet.elements=fromSet.elements.pickElemsOfType(elemType)
    return newSet

def get_subset_elem_of_mat(matType,fromSet,toSetName):
    '''return a subset of fromSet composed by the elements of material 
    matType

    :param matType: name of the material of target elements (string)
    :param fromSet: set from which to extract the subset
    :param toSetName: string to name the created subset
    '''
    newSet=fromSet.getPreprocessor.getSets.defSet(toSetName)
    newSet.elements=fromSet.elements.pickElemsOfMaterial(matType)
    return newSet

def get_min_coo_nod(setWithNod,cooId):
    '''Return the minimum value of the specified coordinate in the set of nodes

    :param setWithNod: set that contains the nodes
    :param cooId:  identification of the desired coordinate (0 for X coordinate,
                   1 for Y coord. and 2 for Z coord.
    '''
    retval=0
    nodInSet=setWithNod.nodes
    if cooId in [0,1,2]:
        CoorNod=[n.getCoo[cooId] for n in nodInSet]
        retval=min(CoorNod)
    else:
        lmsg.error("Wrong coordinate id. Available values: 0, 1, 2  \n")
    return retval
    
def get_max_coo_nod(setWithNod,cooId):
    '''Return the maximum value of the specified coordinate in the set of nodes

    :param setWithNod: set that contains the nodes
    :param cooId:  identification of the desired coordinate (0 for X coordinate,
                   1 for Y coord. and 2 for Z coord.
    '''
    retval=0
    nodInSet=setWithNod.nodes
    if cooId in [0,1,2]:
        CoorNod=[n.getCoo[cooId] for n in nodInSet]
        retval=max(CoorNod)
    else:
        lmsg.error("Wrong coordinate id. Available values: 0, 1, 2  \n")
    return retval
    
    
    
def setLin_to_lst(setLin):
    '''return a list with the lines included in the set `setLin` '''
    return [l for l in setLin.getLines]
    
def setPnt_to_lst(setPnt):
    '''return  a list with the points included in set `setPnt` '''
    return [p for p in setPnt.getPoints]
 
def setSurf_to_lst(setSurf):
    '''return a list with the surfaces included in the set `setSurf`'''
    return [s for s in setSurf.getSurfaces]

def setNod_to_lst(setNod):
    '''return a list with the nodes included in the set `setNod` '''
    return [n for n in setNod.nodes]

def setElem_to_lst(setElem):
    '''return a list with the elements included in the set `setElem` '''
    return [e for e in setElem.elements]
   
def lstNod_to_set(preprocessor,lstNod,setName):
    '''add the nodes in list `lstNod` to the set named setName.
    If the set doesn't exist, the function creates it.
    '''
    setsMng=preprocessor.getSets
    if setsMng.exists(setName):
        s=setsMng.getSet(setName)
    else:
        s=setsMng.defSet(setName)
    for n in lstNod:
        s.nodes.append(n)
    return s   

def lstLin_to_set(preprocessor,lstLin,setName):
    '''add the lines in list `lstNod` to the set named setName.
    If the set doesn't exist, the function creates it.
    '''
    setsMng=preprocessor.getSets
    if setsMng.exists(setName):
        s=setsMng.getSet(setName)
    else:
        s=setsMng.defSet(setName)
    for l in lstLin:
        s.getLines.append(l)
    return s   

def set_included_in_orthoPrism(preprocessor,setInit,prismBase,prismAxis,setName):
    '''reselect from set setInit those elements included in a orthogonal prism
    defined by a 2D polygon and the direction of its axis. 

    :param preprocessor: preprocessor
    :param setInit:      set of elements to which restrict the search
    :param prismBase:    2D polygon that defines the n-sided base of the prism.
                         The vertices of the polygon are defined in global 
                         coordinates in the following way:
                         - for X-axis-prism: (y,z)
                         - for Y-axis-prism: (x,z)
                         - for Z-axis-prism: (x,y)
    
    :param prismAxis:    axis of the prism (can be equal to 'X', 'Y', 'Z')
    :param setName:      name of the set to be generated                   
    '''
    sElIni=setInit.elements
    if prismAxis in ['X','x']:
        elem_inside_prism=[e for e in sElIni if prismBase.In(geom.Pos2d(e.getPosCentroid(True).y,e.getPosCentroid(True).z),0)]
    elif prismAxis in ['Y','y']:
        elem_inside_prism=[e for e in sElIni if prismBase.In(geom.Pos2d(e.getPosCentroid(True).x,e.getPosCentroid(True).z),0)]
    elif prismAxis in ['Z','z']:
        elem_inside_prism=[e for e in sElIni if prismBase.In(geom.Pos2d(e.getPosCentroid(True).x,e.getPosCentroid(True).y),0)]
    else:
        lmsg.error("Wrong prisma axis. Available values: 'X', 'Y', 'Z' \n")
    s=lstElem_to_set(preprocessor,elem_inside_prism,setName)
    s.fillDownwards()
    return s

def set_not_included_in_orthoPrism(preprocessor,setInit,prismBase,prismAxis,setName):
    '''reselect from set setInit those elements NOT included in a orthogonal prism
    defined by a 2D polygon and the direction of its axis. 

    :param preprocessor: preprocessor
    :param setInit:      set of elements to which restrict the search
    :param prismBase:    2D polygon that defines the n-sided base of the prism.
                         The vertices of the polygon are defined in global 
                         coordinates in the following way:
                         - for X-axis-prism: (y,z)
                         - for Y-axis-prism: (x,z)
                         - for Z-axis-prism: (x,y)
    
    :param prismAxis:    axis of the prism (can be equal to 'X', 'Y', 'Z')
    :param setName:      name of the set to be generated                   
    '''
    elementsToRemove= set_included_in_orthoPrism(preprocessor,setInit,prismBase,prismAxis,setName+'toRemove')
    retval= setInit-elementsToRemove
    print 'name: ', retval.name
    retval.name= setName
    return retval

def get_subset_lin_parallel_to_axis(axis,fromSet,toSetName,tol=0.001):
    '''return a subset of fromSet composed by lines parallel to
    one of the global axes.  If toSetName exists the subset is appended to it.

    :param axis: global axis (can be equal to 'X', 'Y', 'Z')
    :param fromSet: set from which to extract the subset
    :param toSetName: string to name the created subset
    :param tol: tolerance in line tangent to capture lines (defaults to 0.001)
    '''
    lstLin=None
    fromSet.fillDownwards()
    allLines=fromSet.getLines
    if axis in ['X','x']:
        lstLin=[l for l in allLines if abs(abs(l.getTang(1)[0])-1)<tol]
    elif axis in ['Y','y']:
        lstLin=[l for l in allLines if abs(abs(l.getTang(1)[1])-1)<tol]
    elif axis in ['Z','z']:
        lstLin=[l for l in allLines if abs(abs(l.getTang(1)[2])-1)<tol]
    else:
        lmsg.error("Wrong axis. Available values: 'X', 'Y', 'Z' \n")
    if lstLin:
        s=lstLin_to_set(fromSet.getPreprocessor,lstLin,toSetName)
        s.fillDownwards()
        return s

def get_subset_lin_shorter_than(Lmax,fromSet,toSetName):
    '''return a subset of fromSet composed by lines shorter than 
    Lmax. If toSetName exists the subset is appended to it.

    :param Lmax: maximum length of lines
    :param fromSet: set from which to extract the subset
    :param toSetName: string to name the subset
    '''
    lstLin=None
    fromSet.fillDownwards()
    allLines=fromSet.getLines
    lstLin=[l for l in allLines if l.getLength()<=Lmax]
    if lstLin:
        s=lstLin_to_set(fromSet.getPreprocessor,lstLin,toSetName)
        s.fillDownwards()
        return s
    else:
        lmsg.warning("No lines shorter than Lmax \n")
        return
    
def get_subset_lin_longer_than(Lmin,fromSet,toSetName):
    '''return a subset of fromSet composed by lines longer than 
    Lmin. If toSetName exists the subset is appended to it.

    :param Lmin: minimum length of lines
    :param fromSet: set from which to extract the subset
    :param toSetName: string to name the subset
    '''
    lstLin=None
    fromSet.fillDownwards()
    allLines=fromSet.getLines
    lstLin=[l for l in allLines if l.getLength()>=Lmin]
    if lstLin:
        s=lstLin_to_set(fromSet.getPreprocessor,lstLin,toSetName)
        s.fillDownwards()
        return s
    else:
        lmsg.warning("No lines longer than Lmin \n")
        return

def lstElem_to_set(preprocessor,lstElem,setName):
    '''add the elements in list `lstElem` to the set named setName.
    If the set doesn't exist, the function creates it.
    '''
    setsMng=preprocessor.getSets
    if setsMng.exists(setName):
        s=setsMng.getSet(setName)
    else:
        s=setsMng.defSet(setName)
    for e in lstElem:
        s.elements.append(e)
    return s  


def get_lines_on_points(setPoints,setLinName,onlyIncluded=True):
    '''return a set of lines (and all the entities of lower rank associated) 
    from a given set of points.

    :param setPoints:    set of points
    :param setLinName  : name of the returned set of lines
    :param onlyIncluded: True to select only lines whose both ends are in the 
                         set of points
                         False to select all lines that 'touch' the set of 
                         points
                         (defaults to True)
                         
    '''
    prep=setPoints.getPreprocessor
    allLines=prep.getSets.getSet('total').getLines
    lstTagsPnt=[p.tag for p in setPoints.getPoints]
    if onlyIncluded==True:
        lstLin=[l for l in allLines if (l.getKPoints()[0] in lstTagsPnt and l.getKPoints()[1] in lstTagsPnt)]
    else:
        lstLin=[l for l in allLines if (l.getKPoints()[0]   in lstTagsPnt or l.getKPoints()[1] in lstTagsPnt)]
    s=lstLin_to_set(prep,lstLin,setLinName)
    s.fillDownwards()
    return s


def setAddOf(setA,setB):
    '''return a new set that has all the entities in setA and setB
    This function is going to be replaced by the overload of add operator
    for sets.
    '''
    nSet=setA.getPreprocessor.getSets.defSet('nSet')
    nSet+=(setA)
    nSet+=(setB)
    return nSet
    

def get_nodes_wire(setBusq,lstPtsWire,tol=0.01):
    '''return the list of nodes from the set `setBusq` that belong to 
    the line defined by the successive points in list `lstPtsWire`  
    (expressed as geom.Pos3d(x,y,z))
    '''
    nodAux= setBusq.nodes
    retval= list() 
    for i in range(0,len(lstPtsWire)-1):
        
        segmAux= geom.Segment3d(lstPtsWire[i],lstPtsWire[i+1])
        for n in nodAux:
            p= n.getInitialPos3d
            d= p.distSegment3d(segmAux)
            if(d<tol):
                retval.append(n)
    retval= list(set(retval))       #clear duplicated nodes
    return retval

def get_lstNod_from_lst3DPos(preprocessor,lst3DPos):
    '''return the set of the nearest nodes to the points passed as a list of
    points (expressed as geom.Pos3d(x,y,z))
    '''
    msNod=preprocessor.getNodeHandler.getDomain.getMesh
    return[msNod.getNearestNode(pnt) for pnt in lst3DPos]
    

def get_lstNod_on_points_fromSet(setFrom):
    '''return the list of nearest nodes to all the points included in  
    the set of entities 'setFrom'
    '''
    setFrom.fillDownwards()
    pts=setFrom.getPoints
    return [p.getNode() for p in pts]

def append_points(setTo,points):
  '''Appends points to set of entities

  :param setTo: set of entities
  :param points: list of points [pt1,pt2, ...]
  '''
  pts= setTo.getPoints
  for p in points:
    pts.append(p)

def append_sets(setTo,sets):
  '''Appends other sets to set of entities

  :param setTo: base set of entities
  :param points: list of sets to be appended [set1,set2, ...]
  '''
  for s in sets:
    setTo+=(s)

def append_nodes_from_line(setTo,line):
  '''Appends the nodes in the specified line to a set of entities

  :param setTo: set of entities
  :param line: line that contains the nodes 
  '''
  nn= line.getNumNodes
  nodes= setTo.nodes
  for i in range(1,nn):
    n= line.getNode(1,1,i)
    nodes.append(n)
  
