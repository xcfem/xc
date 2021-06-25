# -*- coding: utf-8 -*-
''' Connection mesh generation.'''
from __future__ import division
from __future__ import print_function

import json
import uuid
import xc_base
import geom
import xc
from connections.steel_connections import import_connection
from misc_utils import log_messages as lmsg

def genPlatesMesh(plateSetsToMesh, xc_materials, seedElemHandler):
    ''' Generate mesh for the plates in the argument.

    :param plateSetsToMesh: XC sets containing the surfaces to mesh.
    :param xc_materials: Python dictionary containing the materials to use
                         when meshing.
    :param seedElemHanlder: XC seed element handler.
    '''
    for plate in plateSetsToMesh:
        for s in plate.surfaces:
            d= s.getProp('holeDiameter')
            s.setElemSize(1.5*d, True)    
            matId= s.getProp('matId')
            xcMat= xc_materials[matId]
            xcMat.h= s.getProp('thickness') # set thickness
            seedElemHandler.defaultMaterial= xcMat.name
            seedElem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))
            #limeSiloBasePlate.setVerbosityLevel(2)
            s.genMesh(xc.meshDir.I, False)
            #limeSiloBasePlate.setVerbosityLevel(1)

def createTemporarySet(setsToMesh):
    ''' Create a temporary set that includes all the surfaces
        contained in the argument sets.

    :param setsToMesh: XC sets containing the surfaces to be meshed 
                       using the built-in structured meshing algorithm.
    '''
    retval= None
    if(len(setsToMesh)>0):
        # Create temporal set
        preprocessor= setsToMesh[0].getPreprocessor
        tmpSetName= str(uuid.uuid4())
        retval= preprocessor.getSets.defSet(tmpSetName)
        for faceSet in setsToMesh:
            for s in faceSet.surfaces:
                s.setElemSizeIJ(0.05, 0.05)
                retval.getSurfaces.append(s)
        retval.fillDownwards()
    return retval
            
def genRegularMesh(setsToMesh, xc_materials, seedElemHandler):
    ''' Generate mesh for the plates in the argument.

    :param setsToMesh: XC sets containing regular quadrilateral
                       surfaces to be meshed using structured 
                       meshing algorithm.
    :param xc_materials: Python dictionary containing the materials to use
                         when meshing.
    :param seedElemHanlder: XC seed element handler.
    '''
    # Create temporal set
    preprocessor= seedElemHandler.getPreprocessor
    xcTmpSet= createTemporarySet(setsToMesh)
    xcTmpSet.conciliaNDivs() # Make the number of divisions compatible
    for faceSet in setsToMesh:
        for s in faceSet.surfaces:
            matId= s.getProp('matId')
            if(matId in xc_materials):
                xcMat= xc_materials[matId]
            else:
                lmsg.error("Unknown material: '"+str(matId)+"'")
            xcMat.h= s.getProp('thickness') # set thickness
            seedElemHandler.defaultMaterial= xcMat.name
            seedElem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))
            s.genMesh(xc.meshDir.I)
    # Remove temporal set
    preprocessor.getSets.removeSet(xcTmpSet.name)

def genConnectionMesh(regularShapesSet, plateSetsToMesh, xc_materials, seedElemHandler):
    ''' Generate mesh for the plates in the argument.

    :param regularShapesSet: XC sets containing regular quadrilateral
                             surfaces to be meshed using structured 
                             meshing algorithm.
    :param plateSetsToMesh: XC sets containing the surfaces to mesh using
                            gmsh algorithm.
    :param xc_materials: Python dictionary containing the materials to use
                         when meshing.
    :param seedElemHanlder: XC seed element handler.
    '''
    genRegularMesh(regularShapesSet, xc_materials, seedElemHandler)
    genPlatesMesh(plateSetsToMesh, xc_materials, seedElemHandler)

def createLoadPatterns(modelSpace, loadCaseNames):
    ''' Create load patterns.'''
    lPatterns= dict()
    for name in loadCaseNames:
        lPattern= modelSpace.newLoadPattern(str(name))
        lPatterns[name]= lPattern
    return lPatterns
    
def genLoads(modelSpace, loadedSides, internalForcesData):
    ''' Create the loads at the end of the connection members.'''
    for key in loadedSides:
        # Get nodes on loaded sides.
        sideData= loadedSides[key]
        loadDirI= sideData['I']
        loadDirJ= sideData['J']
        loadDirK= sideData['K']
        sideList= sideData['sideList']
        nodeSet= modelSpace.defSet('nodes'+str(key))
        for side in sideList:
            for n in side.getEdge.nodes:
                nodeSet.nodes.append(n)
        centroid= nodeSet.nodes.getCentroid(0.0)
        # Put rigid beams between the centroid nodes
        # and the rest of them.
        centroidNode= nodeSet.getNearestNode(centroid)
        centroidNodePos= centroidNode.getInitialPos3d
        internalForces= internalForcesData[str(key)]
        for name in internalForces:
            originLst= internalForces[name][0]
            origin= geom.Pos3d(originLst[0],originLst[1],originLst[2])
            internalForceValue= internalForces[name][1]
            force= internalForceValue['N']*loadDirI+internalForceValue['Vy']*loadDirJ+internalForceValue['Vz']*loadDirK
            moment= internalForceValue['T']*loadDirI+internalForceValue['My']*loadDirJ+internalForceValue['Mz']*loadDirK
            svs= geom.SlidingVectorsSystem3d(origin,force,moment)
            # Apply the loads.
            currentLP= modelSpace.getLoadPattern(str(name))
            modelSpace.distributeLoadOnNodes(svs, nodeSet, currentLP)
    
def createLoadsFromFile(modelSpace, fileName):
    ''' Create the loads whose values are defined in the file argument.

    :param fileName: file name containing the load definition.
    '''
    with open(fileName, 'r') as inputFile:
        internalForcesData= json.load(inputFile)
        inputFile.close()

    # Create load patterns.
    loadCaseNames= import_connection.getLoadCaseNames(internalForcesData)
    lPatterns= createLoadPatterns(modelSpace,loadCaseNames)

    # Get total set.
    xcTotalSet= modelSpace.getTotalSet()

    # Get loaded sides.
    loadedSides= import_connection.getLoadedSides(xcTotalSet)
    genLoads(modelSpace, loadedSides, internalForcesData)
    return loadCaseNames
    
    
