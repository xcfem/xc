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
    ''' Generate mesh for the plates in the argument. This routine
    is used to mesh plates with irregular shapes (not quadrilateral)
    and/or with holes in them. The meshing is done for each face so
    the nodes at its borders are not shared with neighbors faces
    (non-conformal mesh). The contact of the plate with the rest
    of the connection is made by a weld model.    

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
            s.genMesh(xc.meshDir.I, False)

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
    ''' Generate mesh using the built-in mesh generation algorithm
        for the plates in the argument. This algorithm doesn't work
        for surfaces that are not quadrilateral or if the surface
        has holes in it. To mesh this kind of surfaces you can use
        Gmsh.

    :param setsToMesh: XC sets containing regular quadrilateral
                       surfaces to be meshed using structured 
                       meshing algorithm.
    :param xc_materials: Python dictionary containing the materials to use
                         when meshing.
    :param seedElemHanlder: XC seed element handler.
    '''
    preprocessor= seedElemHandler.getPreprocessor
    xcTmpSet= createTemporarySet(setsToMesh) # Create temporary set
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
    # Remove temporary set
    preprocessor.getSets.removeSet(xcTmpSet.name)

def genConnectionMesh(regularShapesSet, plateSetsToMesh, xc_materials, seedElemHandler):
    ''' Generate mesh for a steel connection.

    :param regularShapesSet: XC sets containing regular quadrilateral
                             surfaces to be meshed using structured 
                             meshing algorithm (built-in meshing algo).
    :param plateSetsToMesh: XC sets containing the surfaces to mesh using
                            gmsh algorithm (not quadrilateral or surfaces
                            with holes in them).
    :param xc_materials: Python dictionary containing the materials to use
                         when meshing.
    :param seedElemHanlder: XC seed element handler.
    '''
    genRegularMesh(regularShapesSet, xc_materials, seedElemHandler)
    genPlatesMesh(plateSetsToMesh, xc_materials, seedElemHandler)

def genGmshMesh(setsToMesh, xc_materials, seedElemHandler):
    ''' Generate mesh using the Gmsh. This algorithm allow
        meshing surfaces that are not quadrilateral and/or surfaces
        with holes in them.

    :param setsToMesh: XC sets containing regular quadrilateral
                       surfaces to be meshed using structured 
                       meshing algorithm.
    :param xc_materials: Python dictionary containing the materials to use
                         when meshing.
    :param seedElemHanlder: XC seed element handler.
    '''
    preprocessor= seedElemHandler.getPreprocessor
    xcTmpSet= createTemporarySet(setsToMesh) # Create temporary set
    xcTmpSet.conciliaNDivs() # Make the number of divisions compatible
    # Get the first material and create the mesh with it.
    matId= setsToMesh[0].surfaces[0].getProp('matId')
    thk= setsToMesh[0].surfaces[0].getProp('thickness')
    xcMat= xc_materials[matId]
    xcMat.h= thk # set thickness.
    ## Create seed element.
    seedElemHandler.defaultMaterial= xcMat.name
    seedElem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))
    ## Generate mesh.
    xcTmpSet.useGmsh= True # use Gmsh for meshing.
    xcTmpSet.genMesh(xc.meshDir.I)
    # Change the materials as needed.
    for faceSet in setsToMesh:
        for s in faceSet.surfaces:
            matId= s.getProp('matId')
            if(matId in xc_materials):
                xcMat= xc_materials[matId]
            else:
                lmsg.error("Unknown material: '"+str(matId)+"'")
            xcMat.h= s.getProp('thickness') # set thickness
            for e in s.elements:
                e.setMaterial(xcMat.name)
    # Remove temporary set
    preprocessor.getSets.removeSet(xcTmpSet.name)
        
    
class BoundaryConditions(object):
    ''' Steel connection boundary conditions.

    :ivar modelSpace: PredefinedSpace object used to create the FE model.
    :ivar internalForcesData: dictionary containing the information about 
                              internal loads.
    :ivar loadedSides: sides of the model surfaces that receive loads.
    :ivar loadCaseNames: names of the load cases.
    '''
    def __init__(self, modelSpace, fileName):
        ''' Constructor.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param fileName: file name containing the load definition.
        '''
        self.modelSpace= modelSpace
        with open(fileName, 'r') as inputFile:
            self.internalForcesData= json.load(inputFile)
            inputFile.close()

    def genLoads(self):
        ''' Create the loads at the end of the connection members.'''
        for key in self.loadedSides:
            # Get nodes on loaded sides.
            sideData= self.loadedSides[key]
            loadDirI= sideData['I']
            loadDirJ= sideData['J']
            loadDirK= sideData['K']
            sideList= sideData['sideList']
            nodeSet= self.modelSpace.defSet('loaded_nodes'+str(key))
            for pair in sideList:
                side= pair[0]
                for n in side.getEdge.nodes:
                    nodeSet.nodes.append(n)
            centroid= nodeSet.nodes.getCentroid(0.0)
            # Distribute the loads over the nodes.
            centroidNode= nodeSet.getNearestNode(centroid)
            centroidNodePos= centroidNode.getInitialPos3d
            internalForces= self.internalForcesData[str(key)]
            for name in internalForces:
                originLst= internalForces[name][0]
                origin= geom.Pos3d(originLst[0],originLst[1],originLst[2])
                internalForceValue= internalForces[name][1]
                force= internalForceValue['N']*loadDirI+internalForceValue['Vy']*loadDirJ+internalForceValue['Vz']*loadDirK
                moment= internalForceValue['T']*loadDirI+internalForceValue['My']*loadDirJ+internalForceValue['Mz']*loadDirK
                svs= geom.SlidingVectorsSystem3d(origin,force,moment)
                # Apply the loads.
                currentLP= self.modelSpace.getLoadPattern(str(name))
                self.modelSpace.distributeLoadOnNodes(svs, nodeSet, currentLP)
                
    def createConstraints(self, constrainedMember, constraintType= '000_FFF'):
        ''' Create the constraints at the end of the constrained members.

        :param constrainedMember: member which will be constrained.
        :param constraintType: string defining the type of the constraint
                               (see predefined_spaces module).
        '''
        for key in self.loadedSides:
            # Get nodes on loaded sides.
            sideData= self.loadedSides[key]
            member= sideData['member']
            if(member==constrainedMember):
                sideList= sideData['sideList']
                for pair in sideList:
                    side= pair[0]
                    surface= pair[1]
                    # this trick of the opposite side could not
                    # work in the general case.
                    oppositeEdge= surface.getOppositeEdge(side.getEdge)
                    for n in oppositeEdge.nodes:
                        self.modelSpace.fixNode(constraintType, n.tag)
            
    def createLoadPatterns(self):
        ''' Create load patterns corresponding to the internal forces on 
            members.
        '''
        self.loadCaseNames= import_connection.getLoadCaseNames(self.internalForcesData)
        lPatterns= dict()
        for name in self.loadCaseNames:
            lPattern= self.modelSpace.newLoadPattern(str(name))
            lPatterns[name]= lPattern
        return lPatterns
    
    def createLoads(self):
        ''' Create the loads corresponding to the internal forces on members.
        '''

        # Create load patterns.
        lPatterns= self.createLoadPatterns()

        # Get loaded sides.
        xcTotalSet= self.modelSpace.getTotalSet()
        self.loadedSides= import_connection.getLoadedSides(xcTotalSet)
        
        # Create loads.
        self.genLoads()
        return self.loadCaseNames
    
def createLoadsFromFile(modelSpace, fileName):
    ''' Create the loads whose values are defined in the file argument.

    :param modelSpace: PredefinedSpace object used to create the FE model.
    :param fileName: file name containing the load definition.
    '''
    lmsg.warning('createLoadsFromFile DEPRECATED; use BoundaryConditions instead.')
    bc= BoundaryConditions(modelSpace, fileName)
    loadCaseNames= bc.createLoads()
    return loadCaseNames
