
# -*- coding: utf-8 -*-
from __future__ import print_function



__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import geom
import xc
import math
from misc_utils import log_messages as lmsg

# The tributary area is cumulative (to avoid discontinuities for
# the nodes at the set boundaries), so if you call it twice, you'll get this
# value doubled unless you call resetTributaries first.
# The following functions are written to avoid this problem by returning
# the tributary values in a dictionary whose key is the node tag and
# reseting them inmediately.

def get_tributary_lengths(xcSet, initialGeometry= False):
    ''' Compute the tributary lengths corresponding to the nodes of a set.

    :param xcSet: set contaning the nodes whose tributary element lengths 
                  will be computed.
    :param initialGeometry: if true compute lengths on the initial geometry
                            of the model. Otherwise use its current geometry.
    '''
    retval= dict()
    xcSet.resetTributaries()
    xcSet.computeTributaryLengths(initialGeometry)
    for n in xcSet.nodes:
        retval[n.tag]= n.getTributaryLength()
    xcSet.resetTributaries() # Keep the tributary values clean.
    return retval

def get_tributary_areas(xcSet, initialGeometry= False):
    ''' Compute the tributary areas corresponding to the nodes of a set.

    :param xcSet: set contaning the nodes whose tributary element lengths 
                  will be computed.
    :param initialGeometry: if true compute lengths on the initial geometry
                            of the model. Otherwise use its current geometry.
    '''
    retval= dict()
    xcSet.resetTributaries()
    xcSet.computeTributaryAreas(initialGeometry)
    for n in xcSet.nodes:
        retval[n.tag]= n.getTributaryArea()
    xcSet.resetTributaries() # Keep the tributary values clean.
    return retval

def get_tributary_volumes(xcSet, initialGeometry= False):
    ''' Compute the tributary volumes corresponding to the nodes of a set.

    :param xcSet: set contaning the nodes whose tributary element lengths 
                  will be computed.
    :param initialGeometry: if true compute lengths on the initial geometry
                            of the model. Otherwise use its current geometry.
    '''
    retval= dict()
    xcSet.resetTributaries()
    xcSet.computeTributaryVolumes(initialGeometry)
    for n in xcSet.nodes:
        retval[n.tag]= n.getTributaryVolume()
    xcSet.resetTributaries() # Keep the tributary values clean.
    return retval

def check_element_sets_are_disjoint(xcSets):
    ''' Check that the given element sets are disjoint.

    :param xcSets: sets to check.
    '''
    retval= dict()
    visitedSets= list()
    for xcSet in xcSets:
        for e in xcSet.elements:
            for visitedSet in visitedSets:
                if visitedSet.In(e):
                    eTag= e.tag
                    tpl= (visitedSet.name, xcSet.name)
                    if(eTag in retval):
                        retval[eTag].append(tpl)
                    else:
                        retval[eTag]= [tpl]
        visitedSets.append(xcSet)
    return retval

def check_node_sets_are_disjoint(xcSets):
    ''' Check that the given node sets are disjoint.

    :param xcSets: sets to check.
    '''
    retval= dict()
    visitedSets= list()
    for xcSet in xcSets:
        for n in xcSet.nodes:
            for visitedSet in visitedSets:
                if visitedSet.In(n):
                    nTag= n.tag
                    tpl= (visitedSet.name, xcSet.name)
                    if(nTag in retval):
                        retval[nTag].append(tpl)
                    else:
                        retval[nTag]= [tpl]
        visitedSets.append(xcSet)
    return retval

def check_line_sets_are_disjoint(xcSets):
    ''' Check that the given line sets are disjoint.

    :param xcSets: sets to check.
    '''
    retval= dict()
    visitedSets= list()
    for xcSet in xcSets:
        for l in xcSet.lines:
            for visitedSet in visitedSets:
                if visitedSet.In(l):
                    lTag= l.tag
                    tpl= (visitedSet.name, xcSet.name)
                    if(lTag in retval):
                        retval[lTag].append(tpl)
                    else:
                        retval[lTag]= [tpl]
        visitedSets.append(xcSet)
    return retval

def check_surface_sets_are_disjoint(xcSets):
    ''' Check that the given surface sets are disjoint.

    :param xcSets: sets to check.
    '''
    retval= dict()
    visitedSets= list()
    for xcSet in xcSets:
        for s in xcSet.surfaces:
            for visitedSet in visitedSets:
                if visitedSet.In(s):
                    sTag= s.tag
                    tpl= (visitedSet.name, xcSet.name)
                    if(sTag in retval):
                        retval[sTag].append(tpl)
                    else:
                        retval[sTag]= [tpl]
        visitedSets.append(xcSet)
    return retval

def check_body_sets_are_disjoint(xcSets):
    ''' Check that the given body sets are disjoint.

    :param xcSets: sets to check.
    '''
    retval= dict()
    visitedSets= list()
    for xcSet in xcSets:
        for b in xcSet.bodies:
            for visitedSet in visitedSets:
                if visitedSet.In(s):
                    bTag= b.tag
                    tpl= (visitedSet.name, xcSet.name)
                    if(bTag in retval):
                        retval[bTag].append(tpl)
                    else:
                        retval[bTag]= [tpl]
        visitedSets.append(xcSet)
    return retval

def check_sets_are_disjoint(xcSets):
    ''' Check that the given sets are disjoint.

    :param xcSets: sets to check.
    '''
    retval= dict()
    nodeDict= check_node_sets_are_disjoint(xcSets)
    if(len(nodeDict)>0):
        retval['nodes']= nodeDict
    elementDict= check_element_sets_are_disjoint(xcSets)
    if(len(elementDict)>0):
        retval['elements']= elementDict
    lineDict= check_line_sets_are_disjoint(xcSets)
    if(len(lineDict)>0):
        retval['lines']= lineDict
    surfaceDict= check_surface_sets_are_disjoint(xcSets)
    if(len(surfaceDict)>0):
        retval['surfaces']= surfaceDict
    bodyDict= check_body_sets_are_disjoint(xcSets)
    if(len(bodyDict)>0):
        retval['bodys']= bodyDict
    return retval

def get_subset_inside(geomObj,fromSet,toSetName,tol=0.0):
    '''return a subset of fromSet composed by the entities inside the 
    3D geometric figure geomObj.

    :param geomObj: geom object that delimits the subset
    :param fromSet: set from which to extract the subset
    :param toSetName: string to name the created subset
    :param tol: geometric tolerance for the search (defaults to 0.0)
    '''
    newSet=fromSet.getPreprocessor.getSets.defSet(toSetName)
    # newSet.bodies= fromSet.surfaces.pickBodiesInside(geomObj,tol)
    newSet.surfaces= fromSet.surfaces.pickSurfacesInside(geomObj,tol)
    newSet.lines= fromSet.lines.pickLinesInside(geomObj,tol)
    newSet.points= fromSet.points.pickPointsInside(geomObj,tol)
    newSet.elements= fromSet.elements.pickElemsInside(geomObj,tol)
    newSet.nodes= fromSet.nodes.pickNodesInside(geomObj,tol)
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
    print('name: ', retval.name)
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


def append_quadSurfaces_on_points(setPoints,setSurf,onlyIncluded=True):
    '''append to a set of surfaces those attached to a given set of points.

    :param setPoints:    set of points
    :param setSurf    :  set of surfaces 
    :param onlyIncluded: True to select only lines whose both ends are in the 
                         set of points
                         False to select all lines that 'touch' the set of 
                         points
                         (defaults to True)
                         
    '''
    prep=setPoints.getPreprocessor
    allSurf=prep.getSets.getSet('total').getSurfaces
    lstTagsPnt=[p.tag for p in setPoints.getPoints]
    Dqsurf=setSurf.getSurfaces
    if onlyIncluded==True:
        lstSurf=[s for s in allSurf if (s.getKPoints()[0] in lstTagsPnt and s.getKPoints()[1] in lstTagsPnt and s.getKPoints()[2]  in lstTagsPnt and s.getKPoints()[3]  in lstTagsPnt)]
    else:
        lstSurf=[s for s in allSurf if (s.getKPoints()[0] in lstTagsPnt or s.getKPoints()[1] in lstTagsPnt or s.getKPoints()[2] in lstTagsPnt or s.getKPoints()[3] in lstTagsPnt)]
    for s in lstSurf:
        if s not in Dqsurf:
            Dqsurf.append(s)

def get_lstLines_from_lstPoints(lstPoints,fromSet,onlyIncluded=True):
    ''' return a list of lines from a given set of points that are included
    in a given set

    :param lstPoints: list of points
    :param fromSet: set of entities for the search
    :param onlyIncluded: True to select only lines whose both ends are in the 
                         set of points
                         False to select all lines that 'touch' the set of 
                         points
                         (defaults to True)
    '''
    if len(fromSet.lines)==0:
        fromSet.fillDownwards()
    allLines=fromSet.getLines
    lstTagsPnt=[p.tag for p in lstPoints]
    if onlyIncluded==True:
        lstLin=[l for l in allLines if (l.getKPoints()[0] in lstTagsPnt and l.getKPoints()[1] in lstTagsPnt)]
    else:
        lstLin=[l for l in allLines if (l.getKPoints()[0]   in lstTagsPnt or l.getKPoints()[1] in lstTagsPnt)]
    return lstLin
    
    
        
def get_setLines_on_setPoints(setPoints,setLinName,onlyIncluded=True):
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
    totalSet=prep.getSets.getSet('total')
    lstPoints=[p for p in setPoints.points]
    lstLin=get_lstLines_from_lstPoints(lstPoints,totalSet,onlyIncluded)
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
    
def get_nodes_wire(setBusq,lstPointsWire,tol=0.01):
    '''return the list of nodes from the set `setBusq` that belong to 
    the line defined by the successive points in list `lstPointsWire`  
    '''
    lstPos3DWire=[p.getPos for p in lstPointsWire]
    retval=get_nodes_pos3D_wire(setBusq,lstPos3DWire,tol)
    return retval
    
def get_nodes_pos3D_wire(setBusq,lstPos3DWire,tol=0.01):
    '''return the list of nodes from the set `setBusq` that belong to 
    the line defined by the successive points in list `lstPos3DWire`  
    (expressed as geom.Pos3d(x,y,z))
    '''
    nodAux= setBusq.nodes
    nodTags=set()
    retval= list() 
    for i in range(0,len(lstPos3DWire)-1):
        segmAux= geom.Segment3d(lstPos3DWire[i],lstPos3DWire[i+1])
        for n in nodAux:
            p= n.getInitialPos3d
            d= p.dist(segmAux)
            if(d<tol):
                if n.tag not in nodTags:
                    nodTags.add(n.tag)
                    retval.append(n)
    return retval

def get_lstNodes_from_lstLines(lstLines,fromSet,tol=0.01):
    '''return the list of nodes from the set fromSet
    that touch a list of lines
    '''
    if len(fromSet.nodes)==0:
        fromSet.fillDownwards()
    lstNodTags=list()
    for l in lstLines:
        lstNodLine=get_nodes_wire(fromSet,l.getVertices,tol)
        lstNodTags+=[n.tag for n in lstNodLine]
    lstNodTags=list(set(lstNodTags))
    prep=fromSet.getPreprocessor
    nodH=prep.getNodeHandler
    lstNod=[nodH.getNode(tag) for tag in lstNodTags]
    return lstNod


def get_set_nodes_plane_XY(setName,setBusq,zCoord,tol=1e-4):
    '''Return a set of nodes close to a plane parallel to XY
    within a tolerance.

    :param setName: name of the set to be created 
    :param setBusq: set from which to extract the nodes
    :param zCoord: Z-coordinate of the plane parallel to XY
    :param tol: tolerance (defaults to 1e-4)
    '''
    prep=setBusq.getPreprocessor
    sNodBusq=setBusq.nodes
    nodInPlane=[n for n in sNodBusq if abs(n.getCurrentPos3d(0).z-zCoord)<=tol]
    return lstNod_to_set(prep,nodInPlane,setName)
    
    
def get_set_nodes_plane_XZ(setName,setBusq,yCoord,tol=1e-4):
    '''Return a set of nodes close to a plane parallel to XZ
    within a tolerance.

    :param setName: name of the set to be created 
    :param setBusq: set from which to extract the nodes
    :param yCoord: Y-coordinate of the plane parallel to XZ
    :param tol: tolerance (defaults to 1e-4)
    '''
    prep=setBusq.getPreprocessor
    sNodBusq=setBusq.nodes
    nodInPlane=[n for n in sNodBusq if abs(n.getCurrentPos3d(0).y-yCoord)<=tol]
    return lstNod_to_set(prep,nodInPlane,setName)
    
    
def get_set_nodes_plane_YZ(setName,setBusq,xCoord,tol=1e-4):
    '''Return a set of nodes close to a plane parallel to YZ
    within a tolerance.

    :param setName: name of the set to be created 
    :param setBusq: set from which to extract the nodes
    :param xCoord: X-coordinate of the plane parallel to YZ
    :param tol: tolerance (defaults to 1e-4)
    '''
    prep=setBusq.getPreprocessor
    sNodBusq=setBusq.nodes
    nodInPlane=[n for n in sNodBusq if abs(n.getCurrentPos3d(0).x-xCoord)<=tol]
    return lstNod_to_set(prep,nodInPlane,setName)

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
    if len(setFrom.nodes)==0:
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
  
def rot_X(toSet,angle):
    '''Apply a rotation around global X axis to
    all the entities in the set 

    :param toSet: set of entities
    :param angle: rotation angle (degrees)
    '''
    xAxis= geom.Line3d(geom.Pos3d(0.0,0.0,0.0), geom.Pos3d(100.0,0.0,0.0))
    rot= xc.Rotation(geom.Rotation3d(xAxis,math.radians(angle)))
    toSet.transforms(rot)

def rot_Y(toSet,angle):
    '''Apply a rotation around global Y axis to
    all the entities in the set 

    :param toSet: set of entities
    :param angle: rotation angle (degrees)
    '''
    yAxis= geom.Line3d(geom.Pos3d(0.0,0.0,0.0), geom.Pos3d(0.0,100.0,0.0))
    rot= xc.Rotation(geom.Rotation3d(yAxis,math.radians(angle)))
    toSet.transforms(rot)

def rot_Z(toSet,angle):
    '''Apply a rotation around global Z axis to
    all the entities in the set 

    :param toSet: set of entities
    :param angle: rotation angle (degrees)
    '''
    zAxis= geom.Line3d(geom.Pos3d(0.0,0.0,0.0), geom.Pos3d(0.0,0.0,100.0))
    rot= xc.Rotation(geom.Rotation3d(zAxis,math.radians(angle)))
    toSet.transforms(rot)

def translat(toSet,deltaXYZ):
    '''Apply a translation (dx,dy,dz) to  
    all the entities in the set 

    :param toSet: set of entities
    :param deltaXYZ: displacements in global X-Y-Z axes(dx,dy,dz)
    '''
    transl=xc.Translation(geom.Translation3d(geom.Vector3d(deltaXYZ[0],deltaXYZ[1],deltaXYZ[2])))
    toSet.transforms(transl)
    
