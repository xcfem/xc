
# -*- coding: utf-8 -*-


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import geom
from miscUtils import LogMessages as lmsg



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
    return [n for n in setNod.getNodes]

def setElem_to_lst(setElem):
    '''return a list with the elements included in the set `setElem` '''
    return [e for e in setElem.getElements]
   
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
        s.getNodes.append(n)
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
    sElIni=setInit.getElements
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
        s.getElements.append(e)
    return s  

def get_lin_2Pts(lstLinBusq,tPto1,tPto2):
    '''return the line that belongs to the set `lstLinBusq` and whose
    starting and ending points are those of tags `tPto1` and `tPto2`
    '''
    broke_out= False
    for l in lstLinBusq:
        extr= l.getKPoints()
        if (extr[0]== tPto1 and extr[1]== tPto2) or (extr[0]== tPto2 and extr[1]== tPto1):
            broke_out= True
            break
    if not broke_out:
#        print "Didn't find the line"
        return
    else:
        return l

def setAddOf(setA,setB):
    '''return a new set that has all the entities in setA and setB
    This function is going to be replaced by the overload of add operator
    for sets.
    '''
    nSet=setA.getPreprocessor.getSets.defSet('nSet')
    nSet.extend(setA)
    nSet.extend(setB)
    return nSet
    

def get_nodes_wire(setBusq,lstPtsWire):
    '''return the subset of nodes from the set `setBusq` that belong to 
    the line defined by the successive points in list `lstPts`
    '''
    nodAux= setBusq.getNodes
    retval= list() 
    for i in range(0,len(lstPtsWire)-1):
        segmAux= geom.LineSegment3d(lstPtsWire[i].getPos,lstPtsWire[i+1].getPos)
        for n in nodAux:
            p= n.getInitialPos3d
            d= p.distSegmento3d(segmAux)
            if(d<0.01):
                retval.append(n)
    retval= list(set(retval))       #elimina nudos repetidos
    return retval

def get_lstNod_from_lst3DPos(preprocessor,lst3DPos):
    '''return the set of the nearest nodes to the points passed as a list of
    points (expressed as geom.Pos3d(x,y,z))
    '''
    msNod=preprocessor.getNodeLoader.getDomain.getMesh
    return[msNod.getNearestNode(pnt) for pnt in lst3DPos]
    
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
    setTo.extend(s)

def append_nodes_from_line(setTo,line):
  '''Appends the nodes in the specified line to a set of entities

  :param setTo: set of entities
  :param line: line that contains the nodes 
  '''
  nn= line.getNumNodes
  nodes= setTo.getNodes
  for i in range(1,nn):
    n= line.getNode(1,1,i)
    nodes.append(n)
  
