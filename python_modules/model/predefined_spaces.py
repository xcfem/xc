# -*- coding: utf-8 -*-
''' Classes and functions to facilitate the building of the model.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

# import math
import math
from typing import Sequence
import sys
import geom
import xc
import numpy as np
from misc_utils import log_messages as lmsg
from materials import typical_materials as tm
from postprocess import extrapolate_elem_attr
from postprocess import get_reactions
from postprocess import control_vars
from actions.load_combination_utils import utils
from actions import time_series
from solution import predefined_solutions
from import_export import reader_base
from import_export import neutral_mesh_description as nmd
import uuid

defaultSolutionProcedureType= predefined_solutions.SimpleStaticLinear

def getSuitableXZVector(iNode, jNode):
    ''' Return a vector that can be used to define
        a coordinate transformation for an element
        between the node arguments.

    :param iNode: first node.
    :param jNode: second node.
    '''
    p1= iNode.getInitialPos3d
    p2= jNode.getInitialPos3d
    sg= geom.Line3d(p1,p2)
    v3d= sg.getKVector
    return xc.Vector([v3d.x, v3d.y, v3d.z])

def setWebOrientation(lines, vertBarsWebOrientationVector= geom.Vector3d(0,1,0), barsWebOrientationVector= geom.Vector3d(0,0,1)):
    ''' Defines a property called 'webOrientation' on each line that contains
        a vector that will define the orientation of the web of the structural shape.

    :param lines: line container.
    :vertBarsWebOrientationVector: orientation of the web for the vertical members.
    :barsWebOrientationVector: orientation of the web for the non vertical members.
    '''
    for l in lines:
        iVector= l.getIVector
        angle= abs(barsWebOrientationVector.getAngle(iVector))
        if(angle<1e-3 or abs(angle-math.pi)<1e-3 or abs(angle-2*math.pi)<1e-3): # vertical line.
            orientationVector= vertBarsWebOrientationVector
        else: # non-vertical line.
            orientationVector= barsWebOrientationVector
        kVector= orientationVector.cross(iVector)
        l.setProp('webOrientation', kVector) # orientation of the web.

def setBearingBetweenNodes(prep, iNodA, iNodB, bearingMaterialNames, orientation= None):
    ''' To DEPRECATE. 3/11/2023 LP.'''

    funcName= sys._getframe(0).f_code.co_name
    lmsg.error(funcName+'; will be DEPRECATED soon (camel case notation is reserved for methods). Use function: set_bearing_between_nodes.')


def set_bearing_between_nodes(prep, iNodA, iNodB, bearingMaterialNames, orientation= None):
    '''Modelize a bearing between the nodes

     :param prep: preprocessor
     :param iNodA: (int) first node identifier (tag).
     :param iNodB: (int) second node identifier (tag).
     :param bearingMaterialNames: (list) material names for the zero 
        length element [mat1,mat2,mat3,mat4,mat5,mat6], where:
        mat1,mat2,mat3 correspond to translations along local x,y,z 
        axes, respectively,
        mat3,mat4,mat5 correspond to rotation about local x,y,z 
        axes, respectively.
     :param orientation: (list) of two vectors [x,yp] used to orient 
        the zero length element, where: 
        x: are the vector components in global coordinates defining 
           local x-axis (optional)
        yp: vector components in global coordinates defining a  vector
             that lies in the local x-y plane of the element(optional).
      If the optional orientation vector are not specified, the local
      element axes coincide with the global axes. Otherwise, the local
      z-axis is defined by the cross product between the vectors x 
      and yp specified in the command line.
      :return: newly created zero length element that represents the bearing.

    '''
    # Element definition
    elems= prep.getElementHandler
    elems.dimElem= prep.getNodeHandler.dimSpace # space dimension.
    # Search for the first material that is not None (if any).
    defaultMaterial= next((item for item in bearingMaterialNames if item is not None), None)
    if(defaultMaterial): # Found it.
        elems.defaultMaterial= defaultMaterial
        zl= elems.newElement("ZeroLength",xc.ID([iNodA,iNodB]))
        zl.clearMaterials()
        if(orientation): #Orient element.
            zl.setupVectors(orientation[0],orientation[1])
        numMats= len(bearingMaterialNames)
        for i, materialName in enumerate(bearingMaterialNames):
            if(materialName is not None):
                zl.setMaterial(i,materialName)
    else: # All are None.
        funcName= sys._getframe(0).f_code.co_name
        lmsg.error(funcName+'; all the given materials are None.')
        zl= None
    return zl

# Imperfections
def setImperfectionsXY(nodeSet, slopeX= 1.0/500.0, slopeY= 1.0/500.0):
    '''Set the initial imperfection of the model.

    :param nodeSet: set which nodes will be moved.
    :param slopeX: out of plumbness on x axis.
    :param slopeY: out of plumbness on y axis.
    '''
    if(abs(slopeX)<1.0/500.0):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; imperfection on X quite small.')
    if(abs(slopeY)<1.0/500):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; imperfection on Y quite small.')
    maxValue= 0.0
    connectedElements= list() # Collect elements connected to this nodes.
    if(len(nodeSet.nodes)>0):
        pos= nodeSet.nodes[0].getInitialPos3d
        zMin= pos.z
        zMax= pos.z
        for n in nodeSet.nodes:
            pos= n.getInitialPos3d
            zMin= min(zMin,pos.z)
            zMax= max(zMax,pos.z)
        for n in nodeSet.nodes:
            pos= n.getInitialPos3d
            h= pos.z-zMin
            deltaX= h*slopeX
            deltaY= h*slopeY
            maxValue= max(maxValue, deltaX**2+deltaY**2)
            newPos= pos+geom.Vector3d(deltaX,deltaY,0.0)
            n.setPos(newPos)
            # Collect connected elements.
            connectedElements.extend(n.getConnectedElements())
    # Initialize coordinate transformation of the connected elements.
    for e in connectedElements:
        e.initializeCoordTransf()
    return math.sqrt(maxValue)

def setImperfectionsX(nodeSet, slopeX= 1.0/500.0):
    '''Set the initial imperfection of the model.

    :param nodeSet: set which nodes will be moved.
    :param slopeX: out of plumbness on x axis.
    '''
    setImperfectionsXY(nodeSet, slopeX, 0.0)

def setImperfectionsY(nodeSet, slopeY= 1.0/500.0):
    '''Set the initial imperfection of the model.

    :param nodeSet: set which nodes will be moved.
    :param slopeY: out of plumbness on y axis.
    '''
    setImperfectionsXY(nodeSet, 0.0, slopeY)

def pick_elements_in_zone(plg, resultSet, originSet):
    ''' Return a set containing the elements that lie
        inside the zone argument.

    :param plg: polygon to select the elements inside.
    :param resultSet: set that will be populated with the elements.
    :param originSet: set that contains the elements to check.
    '''
    pln= plg.getPlane()
    for e in originSet.elements:
        pos= pln.getProjection(e.getPosCentroid(True))
        if(plg.In(pos,1e-2)):
            resultSet.elements.append(e)
    return resultSet

def pick_node_on_point(pos, resultSet, originSet):
    ''' Append to resultSet the nearest node to the point argument
        from those in originSet.

    :param pos: 3D point.
    :param resultSet: set that will be populated with the picked node.
    :param originSet: set that contains the nodes to pick.
    '''
    n= originSet.getNearestNode(pos)
    resultSet.nodes.append(n)
    return n

class PredefinedSpace(object):
    ''' Convenience class that sets the space dimension and
        the number of degrees of freedom for a XC finite element
        problem.

    :ivar dimSpace: dimension of the space (1, 2 or 3).
    :ivar numDOFs: number of degrees of freedom for each node (1, 2, 3 or 6).
    :ivar solProcType: type of the solution procedure.
    :ivar fixedNodesTags: tags of the constrained nodes.
    :ivar createdNodeLockersNames: list containing the names of the node lockers created by the methods that activate/deactivate a set of elements.
    :ivar dBase: current database.
    '''
    def __init__(self, nodes, dimSpace, numDOFs, solProcType: predefined_solutions.SolutionProcedure = defaultSolutionProcedureType):
        '''Defines the dimension of the space and the number 
         of DOFs for each node.

         :param nodes: preprocessor nodes handler
         :param dimSpace: dimension of the space (1, 2 or 3)
         :param numDOFs: number of degrees of freedom for each node.
         :param solProcType: type of the solution procedure.
        '''
        self.setPreprocessor(nodes.getPreprocessor)
        nodes.dimSpace= dimSpace
        nodes.numDOFs= numDOFs
        self.solutionProcedureType= solProcType
        self.analysis= None
        self.fixedNodesTags= set()
        self.createdNodeLockersNames= list()
    
    def getProblem(self):
        ''' Return the XC finite element problem object.
        '''
        return self.preprocessor.getProblem

    def clearAll(self):
        ''' Clear the finite element model.'''
        prb= self.getProblem()
        prb.clearAll()
        self.setPreprocessor(preprocessor= None)

    def setVerbosityLevel(self, level):
        ''' Set the verbosity level (0 to 1000). A higher verbosity
            level means more info printed out.

        :param level: verbosity level.
        '''
        self.preprocessor.setVerbosityLevel(level)
        
    def getVerbosityLevel(self):
        ''' Return the current verbosity level.. A higher verbosity
            level means more info printed out.
        '''
        return self.preprocessor.getVerbosityLevel()

    def getNewDatabase(self, fName, dbType= 'BerkeleyDB'):
        ''' Defines a new database.

        :param fName: file name.
        :param dbType: database type (defaults as BerkeleyDB).
        '''
        self.dBase= self.getProblem().newDatabase(dbType,fName)
        return self.dBase

    def save(self, flag: int):
        ''' Saves the current state on the current database.

        :param flag: integer indentifier for the current state
                     in the database.
        '''
        return self.dBase.save(flag)
        
    def restore(self, flag: int):
        ''' Restore the current state from the current database.

        :param flag: integer indentifier for the state to restore
                     in the database.
        '''
        return self.dBase.restore(flag)

    def getSurfaceHandler(self):
        ''' Return the surface handler for this model.'''
        return self.preprocessor.getMultiBlockTopology.getSurfaces

    def getLineHandler(self):
        ''' Return the line handler for this model.'''
        return self.preprocessor.getMultiBlockTopology.getLines

    def getPointHandler(self):
        ''' Return the point handler for this model.'''
        return self.preprocessor.getMultiBlockTopology.getPoints

    def getElementHandler(self):
        ''' Return the element handler for this model.'''
        return self.preprocessor.getElementHandler
    
    def getSeedElementHandler(self):
        ''' Return the seed element handler for this model.'''
        return self.getElementHandler().seedElemHandler

    def setDefaultMaterial(self, material):
        ''' Assigns the material to be used when creating new elements.

        :param material: default material.
        '''
        self.preprocessor.getElementHandler.defaultMaterial= material.name
        self.preprocessor.getElementHandler.seedElemHandler.defaultMaterial= material.name

    def getDefaultMaterials(self):
        ''' Return the default materials for the element handler and the seed element handler.'''
        return [self.preprocessor.getElementHandler.defaultMaterial, self.preprocessor.getElementHandler.seedElemHandler.defaultMaterial]

    def setDefaultCoordTransf(self, coordinateTransformation):
        ''' Assigns the coordinate transformation to be used when creating 
            new elements.

        :param coordinateTransformation: default coordinate transfomation.
        '''
        self.preprocessor.getElementHandler.defaultTransformation= coordinateTransformation.name
        self.preprocessor.getElementHandler.seedElemHandler.defaultTransformation= coordinateTransformation.name

    def setElementDimension(self, elementDimension):
        ''' Set the dimension for the new elements (when appropriate).

        :param elementDimension: dimension for the new elements (1, 2 or 3).
        '''
        self.preprocessor.getElementHandler.dimElem= elementDimension
        self.preprocessor.getElementHandler.seedElemHandler.dimElem= elementDimension

    def newElement(self, elementType:str, nodeTags):
        ''' Create a new element.

        :param elementType: type of the new element (i.e.. 'ShellMITC4' or 'ZeroLength').
        :param nodeTags: list of the identifiers of the element nodes.
        '''
        return self.preprocessor.getElementHandler.newElement(elementType, xc.ID(nodeTags))

    def newSeedElement(self, elementType:str):
        ''' Create a new seed element.

        :param elementType: type of the new element (i.e.. 'ShellMITC4' or 'ZeroLength').
        :param nodeTags: list of the identifiers of the element nodes.
        '''
        return self.preprocessor.getElementHandler.seedElemHandler.newElement(elementType)

    def removeElement(self, element):
        ''' Remove the given element from the FE problem.
     
        :param element: element to remove.
        '''
        return self.preprocessor.removeElement(element)

    def getElements(self, tags: Sequence[int]):
        ''' Return the elements that correspond to the argument
            tags.

        :param tags: element tags.
        '''
        retval= list()
        elementHandler= self.getElementHandler()
        for t in tags:
            e= elementHandler.getElement(t)
            retval.append(e)
        return retval

    def getNumberOfElements(self):
        ''' Return the number of elements.'''
        return self.preprocessor.getDomain.getMesh.getNumElements()

    def getNodeHandler(self):
        ''' Return the node handler for this model.'''
        return self.preprocessor.getNodeHandler

    def newNodeX(self, x):
        ''' Create a new node.

        :param x: x coordinate for the new node.
        '''
        return self.getNodeHandler().newNodeX(x)
    
    def newNodeXY(self, x, y):
        ''' Create a new node.

        :param x: x coordinate for the new node.
        :param y: y coordinate for the new node.
        '''
        return self.getNodeHandler().newNodeXY(x,y)
    
    def newNodeXYZ(self, x, y, z):
        ''' Create a new node.

        :param x: x coordinate for the new node.
        :param y: y coordinate for the new node.
        :param z: z coordinate for the new node.
        '''
        return self.getNodeHandler().newNodeXYZ(x,y,z)
    
    def newNodeFromVector(self, v):
        ''' Create a new node from vector

        :param v: xc vector
        '''
        return self.getNodeHandler().newNodeFromVector(v)

    def removeNode(self, node):
        ''' Remove the given node from the FE problem.
     
        :param node: node to remove:
        '''
        return self.preprocessor.removeNode(node)
    
    def getNodes(self, tags: Sequence[int]):
        ''' Return the nodes that correspond to the argument
            tags.

        :param tags: node tags.
        '''
        retval= list()
        nodeHandler= self.getNodeHandler()
        for t in tags:
            n= nodeHandler.getNode(t)
            retval.append(n)
        return retval
    
    def getNumberOfNodes(self):
        ''' Return the number of elements.'''
        return self.preprocessor.getDomain.getMesh.getNumNodes()

    def getIntForceComponentFromName(self, componentName: str):
        ''' Return the internal force component from the name argument.

        :param componentName: name of the component.
        '''
        if componentName[0] in ['N','M']:
            return componentName.lower()
        elif componentName == 'Q1':
            return 'q13'
        elif componentName == 'Q2':
            return 'q23'
        else: #LCPT I don't like this too much, I prefer let the user make the program to crass. Maybe a Warning? 
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(componentName) + ' is not a valid component. Available components are: N1, N2, N12, M1, M2, M12, Q1, Q2')
            return 'N1'
        
    def setPreprocessor(self, preprocessor: xc.Preprocessor):
        '''Sets suitable values for the members from the dimension of the space 
         and the number of DOFs for each node obtained from the argument.

         :param preprocessor: preprocessor of the finite element problem.
        '''
        if(preprocessor):
            self.preprocessor= preprocessor
            self.constraints= self.preprocessor.getBoundaryCondHandler
        else:
            self.preprocessor= None
            self.constraints= None

    def getMaterialHandler(self):
        ''' Return the preprocessor material handler.'''
        return self.preprocessor.getMaterialHandler

    def setCreepOn(self):
        ''' Activates creep for TD concrete materials.'''
        self.getMaterialHandler().setCreepOn()

    def setCreepOff(self):
        ''' Deactivates creep for TD concrete materials.'''
        self.getMaterialHandler().setCreepOff()

    def setCreepDt(self, dt):
        ''' Set the creep gime increment for TD concrete materials.'''
        self.getMaterialHandler().setCreepDt(dt)

    def getCreepDt(self):
        ''' get the creep gime increment for TD concrete materials.'''
        return self.getMaterialHandler().getCreepDt()

    def conciliaNDivs(self):
        '''Conciliate the number of divisions of the lines.'''
        bodies= self.preprocessor.getMultiBlockTopology.getBodies
        if(bodies.size>0):
            retval= bodies.conciliaNDivs()
        else:
            surfaces= self.preprocessor.getMultiBlockTopology.getSurfaces
            retval= surfaces.conciliaNDivs()
        return retval
    
    def getNDivErrorsTags(self):
        ''' Return the identifies of the lines that have an incomptible number of divisions.'''
        return self.preprocessor.getMultiBlockTopology.getSurfaces.getNDivErrorsTags()
    
    def getMaterialNames(self):
        ''' Return the names of the materials.'''
        return self.getMaterialHandler().getMaterialNames()
    
    def getSpaceDimension(self):
        ''' Return the dimensions of the problem space.'''
        return self.preprocessor.getNodeHandler.dimSpace

    def getNearestPoint(self, pos3d):
        ''' Return the nearest point to the position
            argument.'''
        return self.preprocessor.getMultiBlockTopology.getPoints.getNearest(pos3d)

    def getLoadHandler(self):
        ''' Return the preprocessor load handler.'''
        return self.preprocessor.getLoadHandler

    def getLoadCombinationsHandler(self):
        ''' Return the preprocessor load combinations handler.'''
        return self.getLoadHandler().getLoadCombinations

    def getCurrentTime(self):
        ''' Return the value of the current pseudo-time.'''
        return self.preprocessor.getDomain.currentTime

    def setCurrentTime(self, t):
        ''' Set the value of the current pseudo-time.

        :parma t: time to set.
        '''
        self.preprocessor.getDomain.currentTime= t

    def getCommittedTime(self):
        ''' Return the value of the committed pseudo-time.'''
        return self.preprocessor.getDomain.commitedTime

    def newTimeSeries(self, name= 'ts', tsType= 'constant_ts'):
        ''' Creates a times series -modulation of the load
            in time-.

        :param name: name of the new time series.
        :param tsType: type of the time series -constant_ts, linear_ts,
                       path-ts, pulse_ts, rectangular_ts, triangular_ts
                       or trig_ts-. Defaults to constant_ts.
        '''
        lPatterns= self.getLoadHandler().getLoadPatterns
        ts= lPatterns.newTimeSeries(tsType,name)
        lPatterns.currentTimeSeries= name
        return ts

    def plotTimeSeries(self, timeSeries, timeIncrement= None, timeUnits= None):
        ''' Shows a diagram of the time series.

        :param timeSeries: the time series to display.
        :param timeIncrement: time increment to use in the diagram
                              if None then timeIncrement= duration/100.0
        :param timeUnits: string representing the time units 
                          ('days' or 's' or ...).
        '''
        time_series.plot_time_series(timeSeries= timeSeries, timeIncrement= timeIncrement, timeUnits= timeUnits)
        
    def newLoadPattern(self, name:str, lpType= 'default', setCurrent= False):
        ''' Creates a times series -modulation of the load
            in time-.

        :param name: name of the new time load pattern.
        :param lpType: type of the load pattern -default, uniform_excitation,
                       multi_support_pattern, pbowl_loading-.
        :param setCurrent: if true set the newly created load pattern as the
                           current load pattern.
        '''
        lPatterns= self.getLoadHandler().getLoadPatterns
        if(lPatterns.currentTimeSeries=='nil'):
            self.newTimeSeries()
        lp= lPatterns.newLoadPattern(lpType, name)
        if(setCurrent):
            self.setCurrentLoadPattern(lp.name)
        return lp

    def removeLoadPattern(self, name:str):
        ''' Remove the load pattern with the given name.

        :param name: name of the new time load pattern.
        '''
        retval= False
        currentLPName= self.getCurrentLoadPatternName()
        if(currentLPName==name): # if it's the current load pattern.
            # Issue a warning.
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+"; load pattern: '"+str(name) + "' was the current load pattern. Removing anyway.")
            self.setCurrentLoadPattern('') # No current load pattern anymore
        self.removeLoadCaseFromDomain(name)
        lPatterns= self.getLoadHandler().getLoadPatterns
        retval= lPatterns.removeLoadPattern(name)
        return retval
    
    def getCurrentLoadPatternName(self):
        ''' Return the current load pattern.'''
        return self.getLoadHandler().getLoadPatterns.currentLoadPattern
        
    def setCurrentLoadPattern(self, lpName: str):
        ''' Set the current load pattern.

        :param lpName: load pattern name.
        '''
        self.getLoadHandler().getLoadPatterns.currentLoadPattern= lpName
    
    def getLoadPattern(self, lpName: str):
        ''' Return the load pattern with the argument name.

        :param lpName: load pattern name.
        '''
        return self.getLoadHandler().getLoadPatterns[lpName]

    def getCurrentLoadPattern(self):
        ''' Return the current load pattern.'''
        lpName= self.getCurrentLoadPatternName()
        return self.getLoadPattern(lpName)

    def getLoadPatternsActingOnNode(self, n):
        ''' Return the load patterns that act on the given node.

        :param n: node to search load patterns acting on.
        '''
        return self.getLoadHandler().getLoadPatternsActingOn(n)
    
    def getLoadPatternsActingOnElement(self, e):
        ''' Return the load patterns that act on the given element.

        :param e: element to search load patterns acting on.
        '''
        return self.getLoadHandler().getLoadPatternsActingOn(e)

    def copyNodalLoads(self, fromNode, toNode):
        ''' Copy the nodal loads acting on the first node to the second one.

        :param fromNode: node to copy the loads from.
        :param toNode: node to copy the loads to.
        '''
        return self.getLoadHandler().copyLoads(fromNode, toNode)
    
    def copyElementalLoads(self, fromElement, toElement):
        ''' Copy the nodal loads acting on the first element to the second one.

        :param fromElement: element to copy the loads from.
        :param toElement: element to copy the loads to.
        '''
        return self.getLoadHandler().copyLoads(fromElement, toElement)

    def newLoadCombination(self, name:str, loadCaseExpression:str, force= False):
        '''Defines a new combination and add it to the domain.

           :param name: name of the load combination.
           :param loadCaseExpression: expression that defines the load case as a
                                      combination of previously defined actions
                                      e.g. '1.0*GselfWeight+1.0*GearthPress'
           :param force: if true, force redefinition of the load combination
                         even if it exists.
        '''
        combs= self.getLoadHandler().getLoadCombinations
        retval= None
        if(force):
            retval= combs.newLoadCombination(name,loadCaseExpression)
        else:
            retval= combs.getComb(name)
            if(retval is None): # does not already exists. 
                retval= combs.newLoadCombination(name,loadCaseExpression)
        return retval
        
    def removeLoadCombination(self, name:str):
        ''' Remove the load combination with the given name. Returns true if
            the combination has been succesfully removed.

        :param name: name of the load combination.
        '''
        self.removeLoadCaseFromDomain(name)
        combs= self.getLoadHandler().getLoadCombinations
        return combs.remove(name)

    def removeSPConstraint(self, constraintToRemove):
        ''' Remove the given constrain. Returns true if
            the constraint has been succesfully removed.

        :param constraintToRemove: constraint to remove.
        '''
        return self.preprocessor.removeSFreedom_Constraint(constraintToRemove)

    def removeMFreedom_Constraint(self, constraintToRemove):
        ''' Remove the given constrain. Returns true if
            the constraint has been succesfully removed.

        :param constraintToRemove: constraint to remove.
        '''
        return self.preprocessor.removeMFreedom_Constraint(constraintToRemove)

    def removeMRMFreedom_Constraint(self, constraintToRemove):
        ''' Remove the given constrain. Returns true if
            the constraint has been succesfully removed.

        :param constraintToRemove: constraint to remove.
        '''
        return self.preprocessor.removeMRMFreedom_Constraint(constraintToRemove)

    def newSPConstraint(self, nodeTag: int, dof: int, prescribedDisp= 0.0):
        ''' Prescribe displacement for node DOFs.

        :param nodeTag: tag of the node.
        :param dof: index of the degree of freedom.
        :param prescribedDisp: values of the displacement.
        '''
        self.constraints.newSPConstraint(nodeTag,dof,prescribedDisp)
        self.fixedNodesTags.add(nodeTag)

    def fixNode(self, DOFpattern, nodeTag, restrainedNodeId: str= None):
        '''Restrain DOF of a node according to the DOFpattern, which is a given
         string of type '0FF' that matches the DOFs (uX,uY,uZ)
         where 'F' means FREE and '0' means constrained with value=0
         Note: DOFpaterns '0FF','0_FF', ... are equivalent

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        if(restrainedNodeId):
            nod= self.preprocessor.getNodeHandler.getNode(nodeTag)
            nod.setProp('restrainedNodeId', restrainedNodeId)
        DOFpatclean= DOFpattern.replace('_','')
        DOFtoConstr= [i for i in range(len(DOFpatclean)) if DOFpatclean[i]=='0']
        for nc in DOFtoConstr:
            self.newSPConstraint(nodeTag,nc,0.0)
            
    def setPrescribedDisplacements(self, nodeTag: int, prescDisplacements: Sequence[float]):
        '''Prescribe displacement for node DOFs.

        :param nodeTag: tag of the node.
        :param prescDisplacements: (list) values of the displacements.
        '''

        numDOFs= self.preprocessor.getNodeHandler.numDOFs
        numDisp= len(prescDisplacements)
        if(numDisp<numDOFs):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; prescribed '+str(numDisp)+' displacements, nDOFS= '+str(numDOFs))
        sz= min(numDOFs,numDisp)
        for i in range(0,sz):
            spc= self.newSPConstraint(nodeTag,i,prescDisplacements[i])
            if(__debug__):
                if(not spc):
                    AssertionError('Can\'t create the constraint.')
                    
    def setRigidBeamBetweenNodes(self,nodeTagA: int, nodeTagB: int):
        '''Create a rigid beam between the nodes passed as parameters.

        :param   nodeTagA: tag of the master node.
        :param   nodeTagB: tag of the slave node.
        '''
        return self.constraints.newRigidBeam(nodeTagA,nodeTagB)

    def setRigidRodBetweenNodes(self, nodeTagA: int, nodeTagB: int):
        '''Create a rigid rod between the nodes passed as parameters.

        :param   nodeTagA: tag of the master node.
        :param   nodeTagB: tag of the slave node.
        '''
        return self.constraints.newRigidRod(nodeTagA,nodeTagB)

    def newEqualDOF(self, nodeTagA: int, nodeTagB: int , dofs: xc.ID):
        '''Create an equal DOF constraint between the nodes.

        :param nodeTagA: tag of the master node.
        :param nodeTagB: tag of the slave node.
        :param dofs: degrees of freedom to be glued 
                   (e.g.: dofs=xc.ID([0,2,5]) means to equal ux,uz,rotz)
        '''
        return self.constraints.newEqualDOF(nodeTagA,nodeTagB,dofs)

    def glueNodeToElement(self, nodeTag:int, elemTag:int, gluedDOFs:xc.ID):
        '''Create a constraint that glues the given node to the given element.

        :param nodeTag: tag of the node.
        :param elemTag: tag of the element.
        :param gluedDofs: degrees of freedom to be glued 
                   (e.g.: dofs=xc.ID([0,2,5]) means to equal ux,uz,rotz)
        '''
        node= self.getNodeHandler().getNode(nodeTag)
        elem= self.getElementHandler().getElement(elemTag)
        return self.constraints.newGlueNodeToElement(node,elem,gluedDOFs)
    
    def setFulcrumBetweenNodes(self,nodeTagA: int, pivotNodeTag: int):
        '''Create a fulcrum between the nodes passed as parameters.

        Creates a rigid link between the nodes.
        It's called fulcrum because it's pinned on pivotNodeTag.

        :param   nodeTagA: tag of the primary node.
        :param   pivotNodeTag: tag of the pivot (secondary node).
        '''
        nodes= self.preprocessor.getNodeHandler
        coordNodeB= nodes.getNode(pivotNodeTag).getCoo
        fulcrumNode= nodes.newNodeFromVector(coordNodeB)
        rb= self.constraints.newRigidBeam(nodeTagA,fulcrumNode.tag)
        ed= self.constraints.newEqualDOF(fulcrumNode.tag,pivotNodeTag,xc.ID(self.getDisplacementDOFs()))
        if(__debug__):
            if(not rb):
                AssertionError('Can\'t create the rigid beam constraint.')
            if(not ed):
                AssertionError('Can\'t create the constraint.')
        return fulcrumNode

    def setBearingBetweenNodes(self,iNodA: int, iNodB: int, bearingMaterialNames: Sequence[str], orientation= None):
        '''Modelize a bearing between the nodes

         :param iNodA: (int) first node identifier (tag).
         :param iNodB: (int) second node identifier (tag).
         :param bearingMaterialNames: (list) material names for the zero 
            length element [mat1,mat2,mat3,mat4,mat5,mat6], where:
            mat1,mat2,mat3 correspond to translations along local x,y,z 
            axes, respectively,
            mat3,mat4,mat5 correspond to rotation about local x,y,z 
            axes, respectively.
         :param orientation: (list) of two vectors [x,yp] used to orient 
            the zero length element, where: 
            x: are the vector components in global coordinates defining 
               local x-axis (optional)
            yp: vector components in global coordinates defining a  vector
                 that lies in the local x-y plane of the element (optional).
          If the optional orientation vector are not specified, the local
          element axes coincide with the global axes. Otherwise, the local
          z-axis is defined by the cross product between the vectors x 
          and yp specified in the command line.
          :return: newly created zero length element that represents the 
                   bearing.
        '''
        return set_bearing_between_nodes(self.preprocessor, iNodA, iNodB, bearingMaterialNames, orientation)

    def setBearing(self, iNod: int, bearingMaterialNames: Sequence[str], orientation= None):
        '''Modelize a bearing on X, XY or XYZ directions.

          :param iNod: (int) node identifier (tag).
          :param bearingMaterialNames (list): (list) material names for the zero 
             length element [mat1,mat2,mat3,mat4,mat5,mat6], where:
             mat1,mat2,mat3 correspond to translations along local x,y,z 
             axes, respectively,
             mat3,mat4,mat5 correspond to rotation about local x,y,z 
             axes, respectively.
          :param orientation: (list) of two vectors [x,yp] used to orient 
             the zero length element, where: 
             - x: are the vector components in global coordinates defining 
                local x-axis (optional)
             - yp: vector components in global coordinates defining a  vector
                  that lies in the local x-y plane of the element(optional).

           If the optional orientation vector are not specified, the local
           element axes coincide with the global axes. Otherwise, the local
           z-axis is defined by the cross product between the vectors x 
           and yp specified in the command line.

          :return rtype: (int, int) new node tag, new element tag.
        '''
        nodes= self.preprocessor.getNodeHandler
        newNode= nodes.duplicateNode(iNod) # new node.
        # Element definition
        newElement= self.setBearingBetweenNodes(newNode.tag, iNod, bearingMaterialNames, orientation)
        # Boundary conditions
        numDOFs= self.preprocessor.getNodeHandler.numDOFs
        for i in range(0, numDOFs): # Fix all the DOFs in the new node.
            spc= self.newSPConstraint(newNode.tag,i,0.0)
            if(__debug__):
                if(not spc):
                    AssertionError('Can\'t create the constraint.')
        return newNode, newElement

    def setBearingOnX(self, iNod: int, bearingMaterialName: str):
        '''Modelize a bearing on X direction.

           :param iNod (int): node identifier (tag).
           :param bearingMaterialName (string): material name for the zero 
                                                length element.
        '''
        return self.setBearing(iNod,[bearingMaterialName])

    def setBearingOnXYRigZ(self, iNod: int, bearingMaterialNames: Sequence[str]):
        '''Modelize a non rigid on X and Y directions and rigid on Z bearing.

           :param   iNod (int): node identifier (tag).
           :param bearingMaterialNames (string): material names for the zero length element.
        '''
        newNode, newElement= self.setBearing(iNod,bearingMaterialNames)
        eDofs= self.constraints.newEqualDOF(newNode.tag,iNod,xc.ID([2]))
        if(__debug__):
            if(not eDofs):
                AssertionError('Can\'t create the constraint.')
        return newNode, newElement

    def setUniaxialBearing2D(self, iNod: int, bearingMaterialName: str, direction):
        '''Modelize an uniaxial bearing on the defined direction.

         :param iNod (int): node identifier (tag).
         :param bearingMaterialName (str): material name for the zero length
                 element.
         :param direction: direction of the bearing.
         :return rtype: (int, int) new node tag, new element tag.
        '''
        nodes= self.preprocessor.getNodeHandler
        newNode= nodes.duplicateNode(iNod) # new node.
        # Element definition
        elems= self.getElementHandler()
        elems.dimElem= self.preprocessor.getNodeHandler.dimSpace # space dimension.
        if(elems.dimElem>2):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; Not a bi-dimensional space.')
        elems.defaultMaterial= bearingMaterialName
        zl= elems.newElement("ZeroLength",xc.ID([newNode.tag,iNod]))
        zl.setupVectors(xc.Vector([direction[0],direction[1],0]),xc.Vector([-direction[1],direction[0],0]))
        zl.clearMaterials()
        zl.setMaterial(0,bearingMaterialName)
        # Boundary conditions
        numDOFs= self.preprocessor.getNodeHandler.numDOFs
        for i in range(0,numDOFs):
            spc= self.newSPConstraint(newNode.tag,i,0.0)
            if(__debug__):
                if(not spc):
                    AssertionError('Can\'t create the constraint.')
        return newNode.tag, zl.tag

    def getFloatingNodes(self, xcSet= None):
        ''' Return a list of the nodes that are not connected to any
            element nor any constraint.

        :param xcSet: search only on the nodes in this set.
        '''
        if(xcSet is None):
            xcSet= self.getTotalSet()
        floatingNodes= list()
        for n in xcSet.nodes:
            nElem= n.getNumberOfConnectedElements()
            if(nElem==0):
                nConstraints= n.getNumberOfConnectedConstraints()
                if(nConstraints==0):
                    floatingNodes.append(n)
        return floatingNodes

    def checkFloatingNodes(self, xcSet= None):
        ''' Print an error message if the model has floating nodes.

        :param xcSet: search only on the nodes in this set.
        '''
        floatingNodes= self.getFloatingNodes(xcSet= xcSet)
        if(len(floatingNodes)>0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; there are floating nodes in the model:')
            for n in floatingNodes:
                lmsg.error('   node: '+str(n.tag)+ ' position: '+str(n.getInitialPos3d)+'\n')
        return floatingNodes
        
    def locateEquationNumber(self, eqNumber, xcSet= None):
        ''' Locate the node that contains the DOF that correspond to
            the equation number argument.

        :param eqNumber: number of the equation to locate.
        :param xcSet: search only on the nodes in this set.
        '''
        if(xcSet is None):
            xcSet= self.getTotalSet()
        retval= None
        for n in xcSet.nodes:
            equationNumbers= n.DOFs
            if(eqNumber in equationNumbers):
                retval= n
                break;
        return retval

    def getReactions(self, xcSet= None, tol= 1e-3):
        ''' Return a Reactions object containing the reactions
            obtained during the analysis.

        :param xcSet: compute only the reactions of the nodes
                      in the set.
        '''
        if(xcSet is None):
            xcSet= self.getTotalSet()
            if(__debug__):
                if(not xcSet):
                    AssertionError("Can\'t get the 'total' set")
                
        supportNodes= list()
        for n in xcSet.nodes:
            R= n.getReaction.Norm()
            if(R>tol):
                supportNodes.append(n)
        return get_reactions.Reactions(self.preprocessor, supportNodes)

    def getSetsNames(self):
        ''' Returns the names of the currently defined sets.'''
        return self.preprocessor.getSets.getSetsNames()
    
    def getSet(self, setName: str):
        '''Return the set with the name argument.

        :param setName: name of the set to retrieve.
        '''
        return self.preprocessor.getSets.getSet(setName)
    
    def getTotalSet(self):
        '''Return the set that contains all the defined
           entities.'''
        return self.getSet('total')

    def defSet(self, setName: str= None, nodes= None, elements= None, points= None, lines= None, surfaces= None, bodies= None):
        ''' Defines a set with the name argument.

        :param setName: name of the set to define.
        :param nodes: node container to initizalize the nodes of the set.
        :param element: element container to initizalize the elements of the set.
        :param points: point container to initizalize the points of the set.
        :param lines: line container to initizalize the lines of the set.
        :param surfaces: surface container to initizalize the surfaces of the set.
        :param bodies: body container to initizalize the bodies of the set.
        '''
        if(setName is None):
            setName= uuid.uuid4().hex
        retval= self.preprocessor.getSets.defSet(setName)
        if(nodes):
            for n in nodes:
                retval.nodes.append(n)
        if(elements):
            for e in elements:
                retval.elements.append(e)
        if(points):
            for p in points:
                retval.points.append(p)
        if(lines):
            for l in lines:
                retval.lines.append(l)
        if(surfaces):
            for s in surfaces:
                retval.surfaces.append(s)
        if(bodies):
            for b in bodies:
                retval.bodies.append(b)
        return retval
    
    def removeSet(self, setName: str):
        ''' Remove the set whose name corresponds to the argument.

        :param setName: name of the set to remove.
        '''
        return self.preprocessor.getSets.removeSet(setName)

    def renewSet(self, setName: str):
        ''' Redefines the set with the name argument.

        :param setName: name of the set to renew.
        '''
        if self.preprocessor.getSets.exists(setName):
            self.preprocessor.getSets.removeSet(setName)
        return self.defSet(setName)

    def setSum(self, setName:str, setsToSum: Sequence[xc.Set]):
        ''' Return a set that contains the union of the
            arguments.

        :param setName: name of the set to create. If 'auto'
                        then assign a random (unique) name.
        :param setsToSum: sets to sum in a list.
        '''
        if(setName=='auto'):
            setName= uuid.uuid4().hex
        retval= self.defSet(setName)
        for s in setsToSum:
            retval+= s
        retval.name= setName # remove all the expressions from name.
        return retval
    
    def setIntersection(self, setName: str, setsToIntersect: Sequence[xc.Set]):
        ''' Return a set that contains the union of the
            arguments.

        :param setName: name of the set to create. If 'auto'
                        then assign a random (unique) name.
        :param setsToSum: sets to sum in a list.
        '''
        if(setName=='auto'):
            setName= uuid.uuid4().hex
        retval= self.defSet(setName)
        retval+= setsToIntersect[0]
        for s in setsToIntersect[1:]:
            retval*= s
        retval.name= setName # remove all the expressions from name.
        return retval

    def getSetsContainingNode(self, node):
        ''' Return the sets that contain the given node.

        :param node: node that is contained by the returned sets.
        '''
        return self.preprocessor.getSets.getSetsContaining(node)

    def getSetsContainingElement(self, element):
        ''' Return the sets that contain the given element.

        :param element: element that is contained by the returned sets.
        '''
        return self.preprocessor.getSets.getSetsContaining(element)

    def pickElementsInZone(self, zone, resultSet, originSet= None):
        ''' Return a set containing the elements that lie
            inside the zone argument.

        :param zone: surface representing the zone to load.
        :param resultSet: set that will be populated with the picked elements.
        :param originSet: set that contains the elements to pick. If None
                          consider the total set.
        '''
        if(originSet is None):
            originSet= self.getTotalSet()
        return pick_elements_in_zone(zone, resultSet, originSet)

    def pickNodeOnPoint(self, pt, resultSet, originSet= None):
        ''' Append to resultSet the nearest node to the point argument
            from those in originSet.

        :param pt: point to load.
        :param resultSet: set that will be populated with the picked nodes.
        :param originSet: set that contains the nodes to pick. If None
                          consider the total set.
        '''
        if(originSet is None):
            originSet= self.getTotalSet()
        return pick_node_on_point(pt, resultSet, originSet)

    def removeAllLoadPatternsFromDomain(self):
        ''' Remove all load patterns from domain.'''
        self.preprocessor.getDomain.removeAllLoadPatterns()

    def removeAllLoadsAndCombinationsFromDomain(self):
        ''' Remove all the load patterns and load combinations from the
            domain.'''
        self.preprocessor.getDomain.removeAllLoadsAndCombinations()

    def clearCombinationsFromLoadHandler(self):
        ''' Remove the combinations currently defined in the load handler.'''
        self.getLoadHandler().getLoadCombinations.clear()

    def getActiveLoadPatterns(self):
        ''' Return a list of the active load patterns.'''
        activeLoadPatterns= self.preprocessor.getDomain.getConstraints.getLoadPatterns
        retval= list()
        for al in activeLoadPatterns:
            retval.append(al.data())
        return retval
    
    def getActiveLoadPatternNames(self):
        ''' Return a list of the active load patterns.'''
        activeLoadPatterns= self.getActiveLoadPatterns()
        retval= list()
        for lp in activeLoadPatterns:
            retval.append(lp.name)
        return retval
        
    def revertToStart(self):
        ''' Revert the domain to its initial state..'''
        self.preprocessor.getDomain.revertToStart()

    def commit(self):
        ''' Commit the current state.'''
        self.preprocessor.getDomain.commit()

    def dumpCombinations(self, combContainer):
        ''' Dump combination definitions into XC.

        :param combContainer: combination container (see combinations module).
        '''
        combContainer.dumpCombinations(self.preprocessor)

    def getLoadCaseNamed(self, loadCaseName: str):
        '''Return the load case argument (load pattern or combination) whose 
           name is passed as parameter.

        :param loadCaseName: name of the load pattern or combination.
        '''
        loadHandler= self.getLoadHandler()
        # Return the corresponding load pattern.
        return loadHandler.getLoadPatterns[loadCaseName]
        
    def addLoadCaseToDomain(self, loadCaseName: str):
        '''Add the load case argument (load pattern or combination) to 
           the domain.

        :param loadCaseName: name of the load pattern or combination.
        '''
        loadHandler= self.getLoadHandler()
        loadHandler.addToDomain(loadCaseName)
        # Return the current load pattern.
        return loadHandler.getLoadPatterns[loadCaseName]

    def removeLoadCaseFromDomain(self, loadCaseName: str):
        '''Add the load case argument (load pattern or combination) to the 
           domain.

        :param loadCaseName: name of the load pattern or combination.
        '''
        self.getLoadHandler().removeFromDomain(loadCaseName)

    def getCurrentLoadCaseName(self):
        ''' Return the name of the current combination (if any).'''
        retval= self.preprocessor.getDomain.currentCombinationName
        return retval

    def resetLoadCase(self):
        ''' Remove all the load cases from the domain.'''
        self.preprocessor.resetLoadCase()        
        
    def addNewLoadCaseToDomain(self, loadCaseName: str, loadCaseExpression:str, force= False):
        '''Defines a new combination and add it to the domain.

           :param loadCaseName: name of the load pattern or combination.
           :param loadCaseExpression: expression that defines de load case as a
                                      combination of previously defined actions
                                      e.g. '1.0*GselfWeight+1.0*GearthPress'
           :param force: if true, force redefinition of the load combination
                         even if it exists.
        '''
        lCase= self.newLoadCombination(loadCaseName, loadCaseExpression, force= force)
        self.preprocessor.resetLoadCase()
        self.addLoadCaseToDomain(lCase.name)

    def addCombinationForNonLinearAnalysis(self, loadCaseExpression:str):
        ''' Adds a load combination to the domain and sets the partial
            safety factors obtained from the expresion.

           :param loadCaseExpression: expression that defines de load case as a
                                      combination of previously defined actions
                                      e.g. '1.0*GselfWeight+1.0*GearthPress'
        '''
        # Get load pattern names and partial safety factors.
        combDict= utils.getCombinationDict(loadCaseExpression)
        for lpName in combDict:
            factor= combDict[lpName] # partial safety factor.
            lp= self.addLoadCaseToDomain(lpName)
            lp.gammaF= factor
                
    def createSelfWeightLoad(self, xcSet: xc.Set, gravityVector, alreadyLoaded= None):
        ''' Creates the self-weight load on the elements. Return the 
            identifiers of the loaded elements.

        :param xcSet: set with the elements to load.
        :param gravityVector: gravity acceleration vector.
        '''
        retval= set()
        if(alreadyLoaded):
            retval.update(alreadyLoaded)
        for e in xcSet.getElements:
            tag= e.tag
            if(tag not in retval):
                e.createInertiaLoad(gravityVector)
                retval.add(tag)
        return retval

    def setSolutionProcedureType(self, solutionProcedureType: predefined_solutions.SolutionProcedure):
        ''' Set the solution procedure that will be used
            to solve the finite element problem.

        :param solutionProcedureType: type of the solution procedure.
        '''
        self.solutionProcedureType= solutionProcedureType

    def calculateNodalReactions(self, includeInertia= False, reactionCheckTolerance= 1e-7):
        ''' Compute reactions at nodes.
        :param includeInertia: if true calculate reactions including inertia
                               effects.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        '''
        result= self.preprocessor.getNodeHandler.calculateNodalReactions(includeInertia, reactionCheckTolerance)
        return result
        
    def analyze(self, numSteps= 1, calculateNodalReactions= False, includeInertia= False, reactionCheckTolerance= 1e-7):
        ''' Triggers the analysis of the model with a simple static linear
            solution.

        :param numSteps: number of analysis steps (only useful for loads that vary with time).
        :param calculateNodalReactions: if true calculate reactions at
                                        nodes.
        :param includeInertia: if true calculate reactions including inertia
                               effects.
        :param reactionCheckTolerance: tolerance when checking reaction values.
        '''
        if(numSteps<1):
            numSteps= 1
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; number of steps must be greater than zero. Setting numSteps= '+str(numSteps))
        result= 0
        problem= self.getProblem()
        if(not self.analysis):
            solProc= self.solutionProcedureType(problem)
            solProc.setup()
            self.analysis= solProc.analysis
        result= self.analysis.analyze(numSteps)
        if(result!=0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; error in analysis.')
        else:
            if(calculateNodalReactions):
                result= self.calculateNodalReactions(includeInertia, reactionCheckTolerance)
        return result

    def getEigenvectorsMaxNormInf(self, mode):
        ''' Return the maximum value of the infinity norm for the eigenvectors
            computed for the given node at each mode.

        :param mode: eingenmode.
        '''
        retval= None
        domain= self.preprocessor.getDomain
        if(mode<=domain.numModes):
            retval= self.preprocessor.getDomain.getMesh.getEigenvectorsMaxNormInf(mode)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.log(className+'.'+methodName+'; eigenmode: '+str(mode)+' not computed.')
        return retval
    
    def normalizeEigenvectors(self, mode):
        ''' Normalize the eigenvectors corresponding to the given mode if not
            already done.

        :param mode: eingenmode.
        '''
        retval= self.getEigenvectorsMaxNormInf(mode)
        if(abs(retval-1.0)>1e-4):
            domain= self.preprocessor.getDomain
            retval= domain.getMesh.normalizeEigenvectors(mode)
            retval= domain.getMesh.getEigenvectorsMaxNormInf(mode)
        return retval
    
    def zeroEnergyModes(self, numModes= 1):
        ''' Obtains the zero energy modes of the finite element model.

        :param numModes: number of zero energy modes to obtain.
        '''
        problem= self.getProblem()
        if(self.analysis):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.log(className+'.'+methodName+'; redefining analysis.')
        self.analysis= predefined_solutions.zero_energy_modes(problem)
        return self.analysis.analyze(numModes)

    def ordinaryEigenvalues(self, numModes= 1):
        ''' Obtains the ordinary eigenvalues of the model stiffness matrix.

        :param numModes: number of zero energy modes to obtain.
        '''
        problem= self.getProblem()
        if(self.analysis):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.log(className+'.'+methodName+'; redefining analysis.')
        self.analysis= predefined_solutions.ordinary_eigenvalues(problem)
        return self.analysis.analyze(numModes)
    
    def illConditioningAnalysis(self, numModes= 1):
        ''' Perform an ill-conditioning analysis on the finite element model.

        :param numModes: number of ill-conditioned modes to obtain.
        '''
        problem= self.getProblem()
        if(self.analysis):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.log(className+'.'+methodName+'; edefining analysis.')
        self.analysis= predefined_solutions.ill_conditioning_analysis(problem)
        return self.analysis.analyze(numModes)

    def deactivateElements(self, elemSet: xc.Set, srf= 1e-6, freezeDeadNodes= True):
        ''' Deactivate the elements on the set argument.

        :param elemSet: set of elements to be deactivated.
        :param srf: stress reduction factor for element deactivation.
        '''
        elemSet.killElements()
        mesh= self.preprocessor.getDomain.getMesh
        mesh.setDeadSRF(srf) # set stress reduction factor for
                             # element deactivation.
        if(freezeDeadNodes):
            lockerName= elemSet.name+'_locker'
            mesh.freezeDeadNodes(lockerName) # froze nodes surrounded by dead elements.
            self.createdNodeLockersNames.append(lockerName)

    def activateElements(self, elemSet: xc.Set):
        ''' Activate the (previoulsy deactivated) elements on the set argument.

        :param elemSet: set of elements to be deactivated.
        '''
        elemSet.aliveElements()
        mesh= self.preprocessor.getDomain.getMesh
        lockerName= elemSet.name+'_locker'
        # Check if the nodes have been frozen.
        if(lockerName in self.createdNodeLockersNames):
            mesh.meltAliveNodes(lockerName) # melt frozen nodes.

    def getValuesAtNodes(self, element, code: str, silent= False):
        ''' Return the values corresponding to code at each of the element nodes.

         :param element: element which the stresses at its nodes will be retrieved.
         :param code: magnitude to return (stress, strain,...).
         :param silent: if true don't complain about non-existent properties.
        '''
        return element.getValuesAtNodes(code, silent)
    
    def computeValuesAtNodes(self, setToCompute: xc.Set, propToDefine= 'stress'):
        ''' Extrapolate the stresses to the nodes of the set argument and
            stores them in the property "propToDefine".

        :param setToCompute: set of elements to be processed.
        :param propToDefine: name of the property to define at the nodes.
        '''
        extrapolate_elem_attr.extrapolate_elem_data_to_nodes(setToCompute.getElements,propToDefine,self.getValuesAtNodes, argument= propToDefine, initialValue= xc.Vector([0.0,0.0,0.0,0.0,0.0,0.0]))

    def setNodePropertyFromElements(self, compName: str, xcSet: xc.Set, function, propToDefine: str):
        '''display the stresses on the elements.

        :param compName: name of the component of the magnitude ('sigma_11', 'strain_xx', ...)
        :param xcSet: set of nodes to define the propery at.
        :param function: function to call to retrieve the component value.
        :param propToDefine: name of the property to define at the nodes.
        '''
        # Define the property at nodes.
        self.computeValuesAtNodes(xcSet, propToDefine= propToDefine)
        propertyName= propToDefine
        nodSet= xcSet.nodes
        vComp= 0
        if(compName): # Defined property has components.
            propertyName+= compName
            vComp= function(compName)
        for n in nodSet:
            n.setProp(propertyName,n.getProp(propToDefine)[vComp])
        return propertyName

    def getDisplacementFileHeader(self):
        ''' Return the string header for the file with the displacements.'''
        retval= ' Comb., Node'
        dispLabels= self.getDisplacementComponentsLabels()
        for dl in dispLabels:
            retval+= ', '+dl
        return retval

    def newKPoint(self, x, y, z= 0.0, tol= 0.0):
        ''' Creates a key point.

        :param x: x coordinate for the new point.
        :param y: y coordinate for the new point.
        :param z: z coordinate for the new point (defaults to 0.0).
        :param tol: if greter than zero create the point only if
                    there is no other point at a distance smaller than
                    tol, otherwise return the already existing point.
        '''
        pos3d= geom.Pos3d(x,y,z)

        pointHandler= self.preprocessor.getMultiBlockTopology.getPoints
        if(tol>0.0): # Search for the nearest point.
            otherPt= pointHandler.getNearest(pos3d)
            if(otherPt.dist(pos3d)<tol):
                retval= otherPt
            else:
                retval= pointHandler.newPoint(pos3d)
        else:
            retval= pointHandler.newPoint(pos3d)
        return retval
    
    def newLine(self, p1: xc.Pnt, p2: xc.Pnt):
        ''' Creates a line between the argument points.

        :param p1: from point.
        :param p2: to point.
        '''
        return self.preprocessor.getMultiBlockTopology.getLines.newLine(p1.tag, p2.tag)
    
    def newQuadSurface(self, p1: xc.Pnt, p2: xc.Pnt, p3: xc.Pnt, p4: xc.Pnt):
        ''' Creates a quad surface between the argument points.

        :param p1: first point.
        :param p2: second point.
        '''
        return self.preprocessor.getMultiBlockTopology.getSurfaces.newQuadSurfacePts(p1.tag, p2.tag, p3.tag, p4.tag)
    
    def getLineWithEndPoints(self, pA: xc.Pnt, pB: xc.Pnt):
        ''' Return the line from its endpoints.

        :param pA: from point.
        :param pB: to point.
        '''
        return self.preprocessor.getMultiBlockTopology.getLineWithEndPoints(pA, pB)
        
    def newSurface(self, pointList: Sequence[xc.Pnt]):
        ''' Creates a surface whose vertices are the argument points.

        :param pointList: list of vertices.
        '''
        numPoints= len(pointList)
        surfaceHandler= self.preprocessor.getMultiBlockTopology.getSurfaces
        pntTags= list()
        for p in pointList:
            pntTags.append(p.tag)
        if(numPoints==4):
            retval= surfaceHandler.newQuadSurfacePts(pntTags[0], pntTags[1], pntTags[2], pntTags[3])
        else:
            retval= surfaceHandler.newPolygonalFacePts(pntTags)
        return retval

    def meshIFCStructuralSurfaces(self, elementType: str, elemSize: float, materialDict, xcSet= None):
        ''' Generate the finite element mesh for the surfaces of the set.
        The surfaces are imported from an IFC file.

        :param elementType: element type (ShellMIC4,...).
        :param elemSize: element size.
        :param materialDict: material dictionary.
        :param xcSet: set containing the surfaces to mesh.
        '''
        if(not xcSet):
            xcSet= self.getTotalSet()
        # Get structural surfaces
        structuralSurfaces= list()
        for face in xcSet.surfaces:
            ifcType= face.getProp('attributes')['IfcType']
            if(ifcType=='Structural Surface Member'):
                structuralSurfaces.append(face)
        # Define materials
        for face in structuralSurfaces:
            matId= face.getProp('attributes')['matId']
            concrete= materialDict[matId]
            thickness= face.getProp('attributes')['Thickness']
            membranePlateSectionName= face.name+'_'+concrete.materialName
            membranePlateSection= concrete.defElasticMembranePlateSection(self.preprocessor, membranePlateSectionName, thickness)
            face.setProp('materialName', membranePlateSection.name)
        # Generate mesh
        seedElementHandler= self.preprocessor.getElementHandler.seedElemHandler
        for face in structuralSurfaces:
            seedElementHandler.defaultMaterial= face.getProp('materialName')
            seedElem= seedElementHandler.newElement(elementType)
            if(__debug__):
                if(not seedElem):
                    AssertionError('Can\'t create the seed element.')
            face.setElemSizeIJ(elemSize,elemSize)
            face.genMesh(xc.meshDir.I)
            
    def setImperfectionsXY(self, nodeSet, slopeX= 1.0/500.0, slopeY= 1.0/500.0):
        '''Set the initial imperfection of the model.

        :param nodeSet: set which nodes will be moved.
        :param slopeX: out of plumbness on x axis.
        :param slopeY: out of plumbness on y axis.
        '''
        setImperfectionsXY(nodeSet, slopeX, slopeY)
        
    def setImperfectionsX(self, nodeSet, slopeX= 1.0/500.0):
        '''Set the initial imperfection of the model.

        :param nodeSet: set which nodes will be moved.
        :param slopeX: out of plumbness on x axis.
        '''
        setImperfectionsXY(nodeSet, slopeX, 0.0)

    def setImperfectionsY(self, nodeSet, slopeY= 1.0/500.0):
        '''Set the initial imperfection of the model.

        :param nodeSet: set which nodes will be moved.
        :param slopeY: out of plumbness on y axis.
        '''
        setImperfectionsXY(nodeSet, 0.0, slopeY)

    def importFromFreeCAD(self, fileName, groupsToImport, getRelativeCoo, threshold= 0.01,importLines= True, importSurfaces= True):
        ''' Constructor.

           :param fileName: file name to import.
           :param groupsToImport: list of regular expressions to be tested.
           :param getRelativeCoo: coordinate transformation to be applied to the
                                  points.
           :param importLines: if true import lines.
           :param importSurfaces: if true import surfaces.
        '''
        from import_export import freecad_reader
        freeCADImport= freecad_reader.FreeCADImport(fileName, groupsToImport, getRelativeCoo, threshold, importLines, importSurfaces)
        # Create block topology from FreeCAD model.
        ieData= nmd.XCImportExportData()
        ieData.blockData= freeCADImport.exportBlockTopology('test')
        ## Return the XC commands.
        preprocessor= self.preprocessor # used inside xcCommands
        if(not preprocessor):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; preprocessor not set.')
        ieData.dumpToXC(preprocessor)

    def classifyBlockTopologyObjects(self, setsFromLabels, xcSet= None):
        ''' Insert each block topology object (point, line, surface, volume)
            in one or more set according to the value of its "Labels" property.

        :param setsFromLabels: Python list containing the (regex, set) pairs
                               defining the classification. The first value of
                               the pair is a regular expression to be matched
                               against the object labels, the second value is 
                               the set that will receive the object if the 
                               regular expression matches.
        :param xcSet: set containing the objects to classify (defaults to the
                      total set).
        '''
        if(xcSet is None):
            xcSet= self.getTotalSet()
        reader_base.populateSetsFromEntitiesLabels(setsFromLabels, xcSet)

    def displayLoads(self, oh, loadCasesToDisplay, setsToDisplay= None, elLoadComp='xyzComponents',fUnitConv=1,caption= None,fileName=None, defFScale=0.0, scaleConstr= 0.2):
        ''' Display the loads.

        :param oh: output handler to use as display engine.
        :param loadCasesToDisplay: load cases that will be displayed.
        :param setsToDisplay: element sets to display the loads on.
        :param elLoadComp:component of the linear loads on elements to be 
               depicted [available components: 'xyzComponents' (default),
               'axialComponent', 'transComponent', 'transYComponent', 
               'transZComponent']
        :param fUnitConv:  factor of conversion to be applied to the results
                        (defaults to 1)
        :param caption:   caption for the graphic
        :param fileName:  name of the file to plot the graphic. Defaults to None
                          in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
        :param scaleConstr: scale of SPConstraints symbols (defaults to 0.2)
        '''
        if(setsToDisplay is None):
            setsToDisplay= [self.getTotalSet()]
        for lcName in loadCasesToDisplay:
            self.removeAllLoadPatternsFromDomain()
            self.revertToStart()
            lp= self.getLoadCaseNamed(lcName)
            if(not lp.loads.empty()): # Contains loads.
                lp= self.addLoadCaseToDomain(lcName)
                for xcSet in setsToDisplay:
                    oh.displayLoads(setToDisplay= xcSet, elLoadComp= elLoadComp,fUnitConv= fUnitConv, caption= caption, fileName= fileName, defFScale= defFScale, scaleConstr= scaleConstr)
            else: # There are no loads.
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(methodName+'; load case: '+str(lp.name)+' is empty.')

    def displayReactions(self, oh, setsToDisplay= None, loadCasesToDisplay= None, combContainer= None, echo= False, fileName=None, defFScale=0.0, inclInertia= False, reactionCheckTolerance= 1e-7):
        ''' Display the loads.

        :param oh: output handler to use as display engine.
        :param model: finite element model to display the loads on.
        :param setsToDisplay: element sets to display the loads on.
        :param loadCasesToDisplay: load cases whose reaction will be displayed.
        :param combContainer: load combinations whose reaction will be displayed.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param inclInertia: include inertia effects (defaults to false).
        :param reactionCheckTolerance: relative tolerance when checking reaction values.
        '''
        if(setsToDisplay is None):
            setsToDisplay= [self.getTotalSet()]
        if(loadCasesToDisplay):
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(methodName+'; not implemented yet.')        
        if(combContainer):
            nameExprPairs= combContainer.getNameExpressionPairs()
            if(nameExprPairs):
                for (combKey, loadCombination) in nameExprPairs:
                    self.removeAllLoadPatternsFromDomain()
                    self.revertToStart()
                    if(echo):
                        lmsg.info(str(combKey)+': '+str(loadCombination))
                    self.addNewLoadCaseToDomain(loadCaseName= combKey, loadCaseExpression= loadCombination)
                    self.analyze()
                    for xcSet in setsToDisplay:
                        oh.displayReactions(setToDisplay= xcSet)

    def readControlVars(self, inputFileName):
        ''' Read control var data from the input file an put them as properties
            of the model elements.

        :param inputFileName: name of the input file containing the data.
        '''
        control_vars.readControlVars(preprocessor= self.preprocessor, inputFileName= inputFileName)

    def newRecorder(self, recorderType, outputHandler= None):
        ''' Creates a new recorder on the problem domain.

        :param recorderType: recorder type.
        :param outputHandler: object that will manage the recorder output.
        '''
        return self.preprocessor.getDomain.newRecorder(recorderType,None)
        
    def getStateComponentFromName(self, compName: str):
        '''Return the component index from the state component name (defined in DruckerPrager.cpp).

        :param compName: state component name.
        '''
        retval= 0
        if((compName == 'Invariant_1') or (compName == 'invariant_1')):
            retval= 0
        elif(compName == 'norm_eta'):
            retval= 1
        elif((compName == 'Invariant_ep') or (compName == 'invariant_ep')):
            retval= 2
        elif(compName == 'norm_dev_ep'):
            retval= 3
        elif(compName == 'norm_ep'):
            retval= 4
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: invariant_1, norm_eta, invariant_ep, norm_dev_ep, norm_ep')
        return retval
                
def getModelSpace(preprocessor: xc.Preprocessor):
      '''Return a PredefinedSpace from the dimension of the space 
       and the number of DOFs for each node obtained from the preprocessor.

       :param preprocessor: preprocessor of the finite element problem.
      '''
      nodes= preprocessor.getNodeHandler
      dimSpace= nodes.dimSpace
      numDOFs= nodes.numDOFs
      return PredefinedSpace(nodes,dimSpace,numDOFs)
  
class SolidMechanics1D(PredefinedSpace):
    def __init__(self, nodes, solProcType: predefined_solutions.SolutionProcedure = defaultSolutionProcedureType):
        '''Defines the dimension of the space: nodes by two coordinates (x,y) 
         and two DOF for each node (Ux,Uy)

         :param nodes: preprocessor nodes handler
         :param solProcType: type of the solution procedure.
        '''
        super(SolidMechanics1D,self).__init__(nodes, dimSpace= 1, numDOFs= 1, solProcType= solProcType)
        self.Ux= 0 # Displacement components
        self.epsilon_11= 0 # Strain components
        self.sigma_11= 0 # Stress components
 
    def newNode(self, x):
        ''' Create a new node.

        :param x: x coordinate for the new node.
        '''
        return super(SolidMechanics1D,self).newNodeX(x)
    
    def fixNode0(self, nodeTag: int, restrainedNodeId: str= None):
        '''Restrain both node DOFs (i. e. make them zero).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('0', nodeTag, restrainedNodeId)

class SolidMechanics2D(PredefinedSpace):
    def __init__(self,nodes, solProcType: predefined_solutions.SolutionProcedure = defaultSolutionProcedureType):
        '''Defines the dimension of the space: nodes by two coordinates (x,y) 
         and two DOF for each node (Ux,Uy)

         :param nodes: preprocessor nodes handler
         :param solProcType: type of the solution procedure.
        '''
        super(SolidMechanics2D,self).__init__(nodes, dimSpace= 2, numDOFs= 2, solProcType= solProcType)
        self.Ux= 0 # Displacement components
        self.Uy= 1
        self.epsilon_11= 0 # Strain components
        self.epsilon_22= 1
        self.epsilon_12= 2
        self.sigma_11= 0 # Stress components
        self.sigma_22= 1
        self.sigma_12= 2
 
    def newNode(self, x, y):
        ''' Create a new node.

        :param x: x coordinate for the new node.
        :param y: y coordinate for the new node.
        '''
        return super(SolidMechanics2D,self).newNodeXY(x,y)

    def getForceComponents(self):
        ''' Return the components of the load vectors that correspond to
            forces.'''
        return [0,1]

    def getMomentComponents(self):
        ''' Return the components of the load vectors that correspond to
            moments.'''
        return []
    
    def getDisplacementComponentsLabels(self):
        ''' Return a list with the labels of the
            displacement components.'''
        return ['uX', 'uY']
        
    def getDispComponentFromName(self,compName: str):
        '''Return the component index from the
           displacement component name.

        :param compName: displacement component name.
        '''
        retval= 0
        if compName == 'uX':
            retval= self.Ux
        elif compName == 'uY':
            retval= self.Uy
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: uX, uY')
        return retval

    def getStrainComponentFromName(self, compName: str):
        '''Return the component index from the
           strain component name.

        :param compName: strain component name.
        '''
        retval= 0
        if((compName == 'epsilon_xx') or (compName == 'epsilon_11')):
            retval= self.epsilon_11
        elif((compName == 'epsilon_yy') or (compName == 'epsilon_22')):
            retval= self.epsilon_22
        elif((compName == 'epsilon_xy') or (compName == 'epsilon_12')
             or (compName == 'epsilon_yx') or (compName == 'epsilon_21')):
            retval= self.epsilon_12
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: epsilon_11, epsilon_22, epsilon_12')
        return retval

    def getStressComponentFromName(self, compName: str):
        '''Return the component index from the
           stress component name.

        :param compName: strain component name.
        '''
        retval= 0
        if((compName == 'sigma_xx') or (compName == 'sigma_11')):
            retval= self.sigma_11
        elif((compName == 'sigma_yy') or (compName == 'sigma_22')):
            retval= self.sigma_22
        elif((compName == 'sigma_xy') or (compName == 'sigma_12')
             or (compName == 'sigma_yx') or (compName == 'sigma_21')):
            retval= self.sigma_12
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: sigma_11, sigma_22, sigma_12')
        return retval

    def getDisplacementDOFs(self):
        ''' Return the indices of the displacement DOFs.'''
        return [self.Ux, self.Uy]

    def getRotationalDOFs(self):
        ''' Return the indices of the rotational DOFs.'''
        return []

    def getDisplacementVector(self,nodeTag: int):
        ''' Return a vector with the displacement components of the 
            node motion.

        :param nodeTag: identifier of the node.
        '''
        nod= self.preprocessor.getNodeHandler.getNode(nodeTag)
        disp= nod.getDisp
        return xc.Vector([disp[self.Ux],disp[self.Uy]])
    
    def fixNode00(self, nodeTag: int, restrainedNodeId: str= None):
        '''Restrain both node DOFs (i. e. make them zero).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('00', nodeTag, restrainedNodeId)

    def fixNode0F(self, nodeTag: int, restrainedNodeId: str= None):
        '''Restrain only displacement DOFs (i. e. Ux= 0 and Uy= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('0F', nodeTag, restrainedNodeId)
        
    def fixNodeF0(self, nodeTag: int, restrainedNodeId: str= None):
        '''Restrain only displacement DOFs (i. e. Ux= 0 and Uy= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('F0', nodeTag, restrainedNodeId)

def gdls_elasticidad2D(nodes):
    '''Defines the dimension of the space: nodes by two coordinates (x,y) and two DOF for each node (Ux,Uy)

    :param nodes: nodes handler
    '''
    lmsg.warning('gdls_elasticidad2D DEPRECATED; use SolidMechanics2D.')
    return SolidMechanics2D(nodes)

class StructuralMechanics(PredefinedSpace):
    '''Structural mechanics finite element problem.'''
    def __init__(self, nodes, dimSpace: int, numDOFs: int, solProcType: predefined_solutions.SolutionProcedure = defaultSolutionProcedureType):
        '''Defines the dimension of the space: nodes by two coordinates (x,y) 
         and three DOF for each node (Ux,Uy,theta)

         :param nodes: preprocessor nodes handler
         :param dimSpace: dimension of the space (1, 2 or 3)
         :param numDOFs: number of degrees of freedom for each node.
         :param solProcType: type of the solution procedure.
        '''
        super(StructuralMechanics,self).__init__(nodes,dimSpace,numDOFs, solProcType)
            
    def createTrusses(self, xcSet: xc.Set, material, area, crossSection= None, corotational= False):
        ''' Meshes the lines of the set argument with Truss
            elements.

        :param xcSet: set with the lines to mesh.
        :param material: material to assign to the elements.
        :param area: area to assign to the elements.
        :param crossSection: object that defines the geometry of the element section.
        :param corotational: if true, use corotational formulation.
        '''
        elementType= 'Truss'
        if(corotational):
            elementType= 'CorotTruss'
        # numDOFs= self.preprocessor.getNodeHandler.numDOFs
        dimElem= self.preprocessor.getNodeHandler.dimSpace
        seedElemHandler= self.getSeedElementHandler()
        seedElemHandler.defaultMaterial= material.name
        seedElemHandler.dimElem= dimElem
        for l in xcSet.getLines:
            l.nDiv= 1
            l.setProp('material',material)
            l.setProp('area',area)
            elem= seedElemHandler.newElement(elementType)
            elem.sectionArea= area
            l.genMesh(xc.meshDir.I)
            if(crossSection):
                for e in l.getElements:
                    e.setProp('crossSection',crossSection)
        xcSet.fillDownwards()
        
    def getSuitableXZVector(self, iNode, jNode):
        ''' Return a vector that can be used to define
            a coordinate transformation for an element
            between the node arguments.

        :param iNode: first node.
        :param jNode: second node.
        '''
        return getSuitableXZVector(iNode, jNode)

    def createElasticBeams(self, xcSet, crossSection, trf, xzVector= None, nDiv= 4):
        ''' Meshes the lines of the set argument with ElasticBeam3d
            elements.

        :param xcSet: set with the lines to mesh.
        :param trf: coordinate transformation to assign to the elements.
        :param xzVector: vector defining transformation XZ plane.
        :param crossSection: object that defines the geometry of the element section.
        :param nDiv: number of divisions on each line.
        '''
        numDOFs= self.preprocessor.getNodeHandler.numDOFs
        xc_material= crossSection.xc_material
        if(numDOFs==3):
            elementType= 'ElasticBeam2d'
            if(not xc_material):
                xc_material= crossSection.defElasticShearSection2d(self.preprocessor)
        elif(numDOFs==6):
            elementType= 'ElasticBeam3d'
            if(not xc_material):
                xc_material= crossSection.defElasticShearSection3d(self.preprocessor)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; something went wrong; numDOFs= '+str(numDOFs))
        seedElemHandler= self.getSeedElementHandler()
            
        seedElemHandler.defaultMaterial= xc_material.getName()
        for l in xcSet.getLines:
            l.nDiv= nDiv
            if(xzVector):
                trf.xzVector= xzVector
            else:
                v3d= l.getKVector
                trf.xzVector= xc.Vector([v3d.x, v3d.y, v3d.z])
            seedElemHandler.defaultTransformation= trf.names
            elem= seedElemHandler.newElement(elementType)
            if(__debug__):
                if(not elem):
                    AssertionError('Can\'t create the element.')
            l.genMesh(xc.meshDir.I)
            if(crossSection):
                for e in l.getElements:
                    e.setProp('crossSection',crossSection)
        xcSet.fillDownwards()

        
class StructuralMechanics2D(StructuralMechanics):
    def __init__(self,nodes, solProcType: predefined_solutions.SolutionProcedure = defaultSolutionProcedureType):
        '''Defines the dimension of the space: nodes by two coordinates (x,y) 
         and three DOF for each node (Ux,Uy,theta)

         :param nodes: preprocessor nodes handler
         :param solProcType: type of the solution procedure.
        '''
        super(StructuralMechanics2D,self).__init__(nodes,2,3, solProcType= solProcType)
        self.Ux= 0 # displacement components
        self.Uy= 1
        self.Theta= 2
        self.epsilon= 0 # generalized strain components; axial,
        self.kappa= 1 # bending,
        self.gamma= 2 # shear
        self.N= 0 # generalized stress components; axial,
        self.M= 1 # bending,
        self.Q= 2 # shear.

    def newNode(self, x, y):
        ''' Create a new node.

        :param x: x coordinate for the new node.
        :param y: y coordinate for the new node.
        '''
        return super(StructuralMechanics2D,self).newNodeXY(x,y)
        
    def getForceComponents(self):
        ''' Return the components of the load vectors that correspond to
            forces.'''
        return [0,1]

    def getMomentComponents(self):
        ''' Return the components of the load vectors that correspond to
            moments.'''
        return [2]
    
    def getDisplacementComponentsLabels(self):
        ''' Return a list with the labels of the
            displacement components.'''
        return ['uX', 'uY', 'rotZ']
        
    def getDispComponentFromName(self,compName):
        '''Return the component index from the
           displacement name.'''
        retval= 0
        if compName == 'uX':
            retval= self.Ux
        elif compName == 'uY':
            retval= self.Uy
        elif compName == 'rotZ':
            retval= self.Theta
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: uX, uY, rotZ')
        return retval

    def getStrainComponentFromName(self, compName):
        '''Return the component index from the
           generalized strain name.'''
        retval= 0
        if(compName == 'epsilon'): # axial
            retval= self.epsilon
        elif(compName == 'kappa'): # bending
            retval= self.kappa
        elif(compName == 'gamma'): # shear
            retval= self.gamma
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: epsilon, kappa, gamma')
        return retval

    def getStressComponentFromName(self, compName):
        '''Return the component index from the
           stress name.'''
        retval= 0
        if((compName == 'N') or (compName == 'P')):
            retval= self.N
        elif(compName == 'M'):
            retval= self.M
        elif((compName == 'Q') or (compName == 'V')):
            retval= self.Q
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: N, M, Q')
        return retval

    def getDisplacementDOFs(self):
        ''' Return the indices of the displacement DOFs.'''
        return [self.Ux,self.Uy]

    def getRotationalDOFs(self):
        ''' Return the indices of the rotational DOFs.'''
        return [self.Theta]

    def getDisplacementVector(self,nodeTag):
        ''' Return a vector with the displacement components of the node motion.'''
        nod= self.preprocessor.getNodeHandler.getNode(nodeTag)
        disp= nod.getDisp
        return xc.Vector([disp[self.Ux],disp[self.Uy]])

    def getRotationVector(self,nodeTag):
        ''' Return a vector with the rotation components of the node motion.'''
        nod= self.preprocessor.getNodeHandler.getNode(nodeTag)
        disp= nod.getDisp
        return xc.Vector([disp[self.Theta]])

    def newLinearCrdTransf(self, trfName):
        ''' Creates a new 2D linear transformation.

          :param trfName: name for the new transformation.
        '''
        trfs= self.preprocessor.getTransfCooHandler
        retval= trfs.newLinearCrdTransf2d(trfName)
        return retval

    def newPDeltaCrdTransf(self, trfName):
        ''' Creates a new 2D PDelta transformation.

          :param trfName: name for the new transformation.
        '''
        trfs= self.preprocessor.getTransfCooHandler
        retval= trfs.newPDeltaCrdTransf2d(trfName)
        return retval

    def newCorotCrdTransf(self, trfName):
        ''' Creates a new 2D corotational transformation.

          :param trfName: name for the new transformation.
        '''
        trfs= self.preprocessor.getTransfCooHandler
        retval= trfs.newCorotCrdTransf2d(trfName)
        return retval
    
    def newCrdTransf(self, trfName, trfType= 'linear'):
        ''' Creates a new 3D transformation.

          :param trfName: name for the new transformation.
          :param trfType: type of the transformation ('linear', 'p_delta' or 'corotational')
        '''
        retval= None
        if(trfType=='linear'):
            retval= self.newLinearCrdTransf(trfName)
        elif(trfType=='p_delta'):
            retval= self.newPDeltaCrdTransf(trfName)
        elif(trfType=='corotational'):
            retval= self.newCorotCrdTransf(trfName)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; unknown transformation type: \''+trfType+'\'')
        return retval
    
    def fixNode000(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all three node DOFs (i. e. make them zero).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('000', nodeTag, restrainedNodeId)

    def fixNode00F(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain only displacement DOFs (i. e. Ux= 0 and Uy= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('00F', nodeTag, restrainedNodeId)

    def fixNode0F0(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all three node DOFs (i. e. make them zero).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('0F0', nodeTag, restrainedNodeId)

    def fixNode0FF(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain only X displacement DOF (i. e. Ux= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('0FF', nodeTag, restrainedNodeId)

    def fixNodeF00(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain displacement Y and rotation (i. e. Uy= 0, Theta= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('F00', nodeTag, restrainedNodeId)
        
    def fixNodeF0F(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain only Y displacement DOF (i. e. Uy= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('F0F', nodeTag, restrainedNodeId)

    def fixNodeFF0(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain only rotation DOF (i. e. Theta= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('FF0', nodeTag, restrainedNodeId)

    def fixNodesLine(self, line, restrainedNodeId: str= None):
        '''Restrain all DOFs of the line nodes.
        
        :param line: line whose nodes will be restrained.
        :param restrainedNodeId: identifier of the node to display with
                                 the reaction values.
        '''  
        nn= line.getNumNodes
        for i in range(1,nn+1):
            nodeTag= line.getNodeI(i).tag
            self.fixNode000(nodeTag, restrainedNodeId)
            

def getStructuralMechanics2DSpace(preprocessor: xc.Preprocessor):
    '''Return a PredefinedSpace from the dimension of the space 
     and the number of DOFs for each node obtained from the preprocessor.

     :param preprocessor: preprocessor of the finite element problem.
    '''
    nodes= preprocessor.getNodeHandler
    assert(nodes.dimSpace==2)
    assert(nodes.numDOFs==3)
    return StructuralMechanics2D(nodes)

def gdls_resist_materials2D(nodes):
    '''Defines the dimension of the space: nodes by two coordinates (x,y) and three DOF for each node (Ux,Uy,theta)

    :param nodes: preprocessor nodes handler
    '''
    lmsg.warning('gdls_resist_materials2D DEPRECATED; use StructuralMechanics2D.')
    return StructuralMechanics2D(nodes)

class SolidMechanics3D(PredefinedSpace):
    def __init__(self,nodes, solProcType: predefined_solutions.SolutionProcedure = defaultSolutionProcedureType):
        '''Defines the dimension of the space: nodes by three coordinates (x,y,z) 
         and two DOF for each node (Ux,Uy,Uz)

         :param nodes: preprocessor nodes handler
         :param solProcType: type of the solution procedure.
        '''
        super(SolidMechanics3D,self).__init__(nodes,dimSpace= 3, numDOFs= 3, solProcType= solProcType)
        self.Ux= 0 # displacement components.
        self.Uy= 1
        self.Uz= 2
        self.epsilon_11= 0 # Strain components
        self.epsilon_22= 1
        self.epsilon_33= 2
        self.epsilon_12= 3
        self.epsilon_23= 4
        self.epsilon_13= 5
        self.sigma_11= 0 # Stress components
        self.sigma_22= 1
        self.sigma_33= 2
        self.sigma_12= 3
        self.sigma_23= 4
        self.sigma_13= 5

    def newNode(self, x, y, z):
        ''' Create a new node.

        :param x: x coordinate for the new node.
        :param y: y coordinate for the new node.
        :param z: z coordinate for the new node.
        '''
        return super(SolidMechanics3D,self).newNodeXYZ(x,y,z)
    
        
    def getForceComponents(self):
        ''' Return the components of the load vectors that correspond to
            forces.'''
        return [0, 1, 2]

    def getMomentComponents(self):
        ''' Return the components of the load vectors that correspond to
            moments.'''
        return []
    
    def getDisplacementComponentsLabels(self):
        ''' Return a list with the labels of the
            displacement components.'''
        return ['uX', 'uY', 'uZ']
        
    def getDispComponentFromName(self,compName):
        '''Return the component index from the
           displacement name.'''
        retval= 0
        if compName == 'uX':
            retval= self.Ux
        elif compName == 'uY':
            retval= self.Uy
        elif compName == 'uZ':
            retval= self.Uz
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: uX, uY, uZ')
        return retval
    
    def getStrainComponentFromName(self, compName):
        '''Return the component index from the
           strain name.'''
        retval= 0
        if((compName == 'epsilon_xx') or (compName == 'epsilon_11')):
            retval= self.epsilon_11
        elif((compName == 'epsilon_yy') or (compName == 'epsilon_22')):
            retval= self.epsilon_22
        elif((compName == 'epsilon_zz') or (compName == 'epsilon_33')):
            retval= self.epsilon_33
        elif((compName == 'epsilon_xy') or (compName == 'epsilon_12')
             or (compName == 'epsilon_yx') or (compName == 'epsilon_21')):
            retval= self.epsilon_12
        elif((compName == 'epsilon_xz') or (compName == 'epsilon_13')
             or (compName == 'epsilon_zx') or (compName == 'epsilon_31')):
            retval= self.epsilon_13
        elif((compName == 'epsilon_yz') or (compName == 'epsilon_23')
             or (compName == 'epsilon_zy') or (compName == 'epsilon_32')):
            retval= self.epsilon_23
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: epsilon_11, epsilon_22, epsilon_33, epsilon_12, epsilon_13, epsilon_23')
        return retval

    def getStressComponentFromName(self, compName):
        '''Return the component index from the
           stress name.'''
        retval= 0
        if((compName == 'sigma_xx') or (compName == 'sigma_11')):
            retval= self.sigma_11
        elif((compName == 'sigma_yy') or (compName == 'sigma_22')):
            retval= self.sigma_22
        elif((compName == 'sigma_zz') or (compName == 'sigma_33')):
            retval= self.sigma_33
        elif((compName == 'sigma_xy') or (compName == 'sigma_12')
             or (compName == 'sigma_yx') or (compName == 'sigma_21')):
            retval= self.sigma_12
        elif((compName == 'sigma_xz') or (compName == 'sigma_13')
             or (compName == 'sigma_zx') or (compName == 'sigma_31')):
            retval= self.sigma_13
        elif((compName == 'sigma_yz') or (compName == 'sigma_23')
             or (compName == 'sigma_zy') or (compName == 'sigma_32')):
            retval= self.sigma_23
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: sigma_11, sigma_22, sigma_33, sigma_12, sigma_13, sigma_23')
        return retval
    
    def getDisplacementDOFs(self):
        ''' Return the indices of the displacement DOFs.'''
        return [self.Ux,self.Uy,self.Uz]

    def getRotationalDOFs(self):
        ''' Return the indices of the rotational DOFs.'''
        return []

    def getDisplacementVector(self,nodeTag):
        ''' Return a vector with the displacement components of the node motion.

          :param nodeTag: node identifier.
        '''
        nod= self.preprocessor.getNodeHandler.getNode(nodeTag)
        disp= nod.getDisp
        return xc.Vector([disp[self.Ux],disp[self.Uy],disp[self.Uz]])

    def fixNode000(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all three node DOFs (i. e. make them zero).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('000', nodeTag, restrainedNodeId)
        

def gdls_elasticidad3D(nodes):
    '''Defines the dimension of the space: nodes by three coordinates (x,y,z) 
       and three DOF for each node (Ux,Uy,Uz)

    :param nodes: preprocessor nodes handler
    '''
    lmsg.warning('gdls_elasticidad3D DEPRECATED; use SolidMechanics3D.')
    return SolidMechanics3D(nodes)


class StructuralMechanics3D(StructuralMechanics):
    def __init__(self,nodes, solProcType: predefined_solutions.SolutionProcedure = defaultSolutionProcedureType):
        '''Define the dimension of the space: nodes by three coordinates (x,y,z) 
        and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

        :param nodes: preprocessor nodes handler
        :param solProcType: type of the solution procedure.
        '''
        super(StructuralMechanics3D,self).__init__(nodes,3,6, solProcType= solProcType)
        self.Ux= 0 # displacement components
        self.Uy= 1
        self.Uz= 2
        self.ThetaX= 3
        self.ThetaY= 4
        self.ThetaZ= 5
        self.epsilon= 0 # generalized strains; axial strain,
        self.kappa_z= 1 # curvature about z-axis,
        self.gamma_y= 2 # shear along y-axis.
        self.kappa_y= 3 # curvature about y-axis,
        self.gamma_z= 4 # shear along z-axis.
        self.theta= 5 # torsion along x-axis.
        self.N= 0 # generalized strains; axial strain,
        self.Mz= 1 # curvature about z-axis,
        self.Vy= 2 # shear along y-axis.
        self.My= 3 # curvature about y-axis,
        self.Vz= 4 # shear along z-axis.
        self.T= 5 # torsion along x-axis.

    def newNode(self, x, y, z):
        ''' Create a new node.

        :param x: x coordinate for the new node.
        :param y: y coordinate for the new node.
        :param z: z coordinate for the new node.
        '''
        return super(StructuralMechanics3D,self).newNodeXYZ(x,y,z)
        
        
    def getForceComponents(self):
        ''' Return the components of the load vectors that correspond to
            forces.'''
        return [0, 1, 2]

    def getMomentComponents(self):
        ''' Return the components of the load vectors that correspond to
            moments.'''
        return [3, 4, 5]
    
    def getDisplacementComponentsLabels(self):
        ''' Return a list with the labels of the
            displacement components.'''
        return ['uX', 'uY', 'uZ', 'rotX', 'rotY', 'rotZ']
    
    def getDispComponentFromName(self,compName):
        '''Return the component index from the
           displacement name.'''
        retval= 0
        if compName == 'uX':
            retval= self.Ux
        elif compName == 'uY':
            retval= self.Uy
        elif compName == 'uZ':
            retval= self.Uz
        elif compName == 'rotX':
            retval= self.ThetaX
        elif compName == 'rotY':
            retval= self.ThetaY
        elif compName == 'rotZ':
            retval= self.ThetaZ
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: uX, uY, uZ, rotX, rotY, rotZ')
        return retval
    
    def getStrainComponentFromName(self, compName):
        '''Return the component index from the
           generalized strain name.'''
        lmsg.warning('getStrainComponentFromName not tested yet.')
        retval= 0        
        if(compName == 'epsilon'): # axial
            retval= self.epsilon
        elif(compName == 'kappa_z'): # bending about local z axis
            retval= self.kappa_z
        elif(compName == 'gamma_y'): # shear along y-axis.
            retval= self.gamma_y
        elif(compName == 'kappa_y'): # bending about local y axis
            retval= self.kappa_y
        elif(compName == 'gamma_z'): # shear along z-axis.
            retval= self.gamma_z
        elif(compName == 'theta'): # torsion along x-axis.
            retval= self.theta
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: epsilon, kappa_z, gamma_y, kappa_y, gamma_z, theta')
        return retval

    def getStressComponentFromName(self, compName):
        '''Return the component index from the
           stress name.'''
        lmsg.warning('getStressComponentFromName not tested yet.')
        retval= 0
        self.N= 0 # generalized strains; axial strain,
        self.Mz= 1 # curvature about z-axis,
        self.Vy= 2 # shear along y-axis.
        self.My= 3 # curvature about y-axis,
        self.Vz= 4 # shear along z-axis.
        self.T= 5 # torsion along x-axis.
        if((compName == 'N') or (compName == 'P')):
            retval= self.N
        elif(compName == 'Mz'):
            retval= self.Mz
        elif((compName == 'Qy') or (compName == 'Vy')):
            retval= self.Qy
        elif(compName == 'My'):
            retval= self.My
        elif((compName == 'Qz') or (compName == 'Vz')):
            retval= self.Qz
        elif(compName == 'T'):
            retval= self.Qz
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; item '+str(compName) + ' is not a valid component. Available components are: N, Mz, Qy, My, Qz, T.')
        return retval
        
    def getDisplacementDOFs(self):
        ''' Return the indices of the displacement DOFs.'''
        return [self.Ux,self.Uy,self.Uz]

    def getRotationalDOFs(self):
        ''' Return the indices of the rotational DOFs.'''
        return [self.ThetaX,self.ThetaY,self.ThetaZ]

    def getDisplacementVector(self,nodeTag):
        ''' Return a vector with the displacement components of the node 
            motion.

          :param nodeTag: node identifier.
        '''
        nod= self.preprocessor.getNodeHandler.getNode(nodeTag)
        disp= nod.getDisp
        return xc.Vector([disp[self.Ux],disp[self.Uy],disp[self.Uz]])

    def getRotationVector(self,nodeTag):
        ''' Return a vector with the rotational components of the node 
            motion.

         :param nodeTag: node identifier.
        '''
        nod= self.preprocessor.getNodeHandler.getNode(nodeTag)
        disp= nod.getDisp
        return xc.Vector([disp[self.ThetaX],disp[self.ThetaY],disp[self.ThetaZ]])

    def newLinearCrdTransf(self, trfName, xzVector):
        ''' Creates a new 3D linear transformation.

          :param trfName: name for the new transformation.
          :param xzVector: vector defining transformation XZ plane.
        '''
        trfs= self.preprocessor.getTransfCooHandler
        retval= trfs.newLinearCrdTransf3d(trfName)
        retval.xzVector= xzVector
        return retval

    def newPDeltaCrdTransf(self, trfName,xzVector):
        ''' Creates a new 3D PDelta transformation.

          :param trfName: name for the new transformation.
          :param xzVector: vector defining transformation XZ plane.
        '''
        trfs= self.preprocessor.getTransfCooHandler
        retval= trfs.newPDeltaCrdTransf3d(trfName)
        retval.xzVector= xzVector
        return retval

    def newCorotCrdTransf(self, trfName,xzVector):
        ''' Creates a new 3D corotational transformation.

          :param trfName: name for the new transformation.
          :param xzVector: vector defining transformation XZ plane.
        '''
        trfs= self.preprocessor.getTransfCooHandler
        retval= trfs.newCorotCrdTransf3d(trfName)
        retval.xzVector= xzVector
        return retval

    def newCrdTransf(self, trfName, xzVector, trfType= 'linear'):
        ''' Creates a new 3D transformation.

          :param trfName: name for the new transformation.
          :param xzVector: vector defining transformation XZ plane.
          :param trfType: type of the transformation ('linear', 'p_delta' or 'corotational')
        '''
        retval= None
        if(trfType=='linear'):
            retval= self.newLinearCrdTransf(trfName, xzVector)
        elif(trfType=='p_delta'):
            retval= self.newPDeltaCrdTransf(trfName, xzVector)
        elif(trfType=='corotational'):
            retval= self.newCorotCrdTransf(trfName, xzVector)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; unknown transformation type: \''+trfType+'\'')
        return retval

    def fixNode000_000(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all six node DOFs (i. e. make them zero).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode("000_000", nodeTag, restrainedNodeId)

    def fixNode000_FFF(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain only displacement DOFs (i. e. Ux= 0, Uy= 0 and Uz= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('000_FFF', nodeTag, restrainedNodeId)

    def fixNode000_0FF(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain the displacements (Ux,Uy and Uz) and
         the rotation about X axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('000_0FF', nodeTag, restrainedNodeId)

    def fixNode000_F0F(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain the displacements (Ux,Uy and Uz) and
         the rotation about Y axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('000_F0F', nodeTag, restrainedNodeId)

    def fixNode000_FF0(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain the displacements (Ux,Uy and Uz) and
         the rotation about Z axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('000_FF0', nodeTag, restrainedNodeId)


    def fixNode000_F00(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for the rotation about X axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('000_F00', nodeTag, restrainedNodeId)

    def fixNode000_0F0(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for the rotation about Y axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('000_0F0', nodeTag, restrainedNodeId)

    def fixNode000_00F(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for the rotation about Z axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('000_00F', nodeTag, restrainedNodeId)

    def fixNodeF00_F00(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for X displacement and the
         rotation about X axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('F00_F00', nodeTag, restrainedNodeId)

    def fixNodeF00_0F0(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for X displacement and the
         rotation about Y axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('F00_0F0', nodeTag, restrainedNodeId)

    def fixNodeF00_00F(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for X displacement and the
         rotation about Z axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('F00_00F', nodeTag, restrainedNodeId)

    def fixNodeF00_0FF(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for X displacement and the
         rotations about Y and Z axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('F00_0FF', nodeTag, restrainedNodeId)

    def fixNodeF00_FFF(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for X displacement and the
         rotations about Y and Z axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('F00_FFF', nodeTag, restrainedNodeId)

    def fixNode0F0_F00(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for Y displacement and the
         rotation about X axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('0F0_F00', nodeTag, restrainedNodeId)

    def fixNode0F0_0F0(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for Y displacement and the
         rotation about Y axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('0F0_0F0', nodeTag, restrainedNodeId)

    def fixNode0F0_00F(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for Y displacement and the
         rotation about Z axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('0F0_00F', nodeTag, restrainedNodeId)

    def fixNode0F0_0FF(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for Y displacement and the
         rotations about Y and Z axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('0F0_0FF', nodeTag, restrainedNodeId)

    def fixNode0F0_FFF(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain all DOFs except for Y displacement and the
         rotations about Y and Z axis.

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('0F0_FFF', nodeTag, restrainedNodeId)

    def fixNodeFFF_000(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain only rotations (i. e. ThetaX= 0, ThetaY= 0 and ThetaZ= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('FFF_000', nodeTag, restrainedNodeId)

    def fixNodeFFF_0F0(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain only X and Z rotations (i. e. ThetaX= 0, and ThetaZ= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('FFF_0F0', nodeTag, restrainedNodeId)

    def fixNode0FF_FFF(self, nodeTag, restrainedNodeId: str= None):
        '''Restrain only X displacement DOF (i. e. Ux= 0).

         :param nodeTag: node identifier.
         :param restrainedNodeId: identifier of the node to display with
                                  the reaction values.
        '''
        self.fixNode('0FF_FFF', nodeTag, restrainedNodeId)

    def LstNodes6DOFConstr(self,lstNodes,constrCond):
        '''Constraint the nodes in the list passed as parameter 
        according to given 6-values set of constraints conditions

        :param lstNodes:     list of nodes to which apply the 
                           constraints
        :param constrCond:   list of constraint conditions, expressed as 
                   [uX, uY, uZ,rotX, rotY, rotZ], where:

                 - uX, uY, uZ: translations in the X, Y and Z directions; 
                 - rotX, rotY, rotZ: rotations about the X, Y and Z axis
                 - 'free': means no constraint values 
        '''
        for n in lstNodes:
            for i in range(0,6):
                if(constrCond[i] != 'free'):
                    self.newSPConstraint(n.tag,i,constrCond[i])
                    
    def setBeamBetweenNodes(self,nodeA, nodeB, sectionName, nmbTransf= None, trfType= 'linear', beamElementType= 'ElasticBeam3d'):
        '''
        Creates a beam/column element between the two nodes being passed as parameters.

        :param nodeA: bar's from node.
        :param nodeB: bar's to node.
        :param sectionName: name of the section material to assign to the element.
        :param nmbTransf: name of the coordinate transformation to use for the new bar.
        :param trfType: type of the transformation ('linear', 'p_delta' or 'corotational')
                        used only if nmbTransf is None.
        :param stiffnessFactor: factor to apply to the beam stiffness.
        '''
        elements= self.getElementHandler()
        # Transformation definition.
        if(not nmbTransf): # define a new transformation.
            nmbTransf= 'bar' + str(nodeA.tag) + str(nodeB.tag)
            xzVector= self.getSuitableXZVector(nodeA, nodeB)
            trf= self.newCrdTransf(nmbTransf, xzVector= xzVector, trfType= trfType)
            if(__debug__):
                if(not trf):
                    AssertionError('Can\'t create the transformation.')
        elements.defaultTransformation= nmbTransf
        # Element definition
        elements.defaultMaterial= sectionName
        elem= elements.newElement(beamElementType,xc.ID([nodeA.tag,nodeB.tag]))
        return elem
    
    def setHugeBeamBetweenNodes(self,nodeA, nodeB, nmbTransf= None, trfType= 'linear', stiffnessFactor= 1.0):
        '''
        Creates a very stiff bar between the two nodes being passed as parameters.
        (it was a workaround to the problem with the reactions values in nodes when
        using multipoint constraints. This problem has been be solved with the
        implementation of MFreedom_ConstraintBase::addResistingForceToNodalReaction).

        :param nodeA: bar's from node.
        :param nodeB: bar's to node.
        :param nmbTransf: name of the coordinate transformation to use for the new bar.
        :param trfType: type of the transformation ('linear', 'p_delta' or 'corotational')
                        used only if nmbTransf is None.
        :param stiffnessFactor: factor to apply to the beam stiffness.
        '''
        # Material definition
        matName= 'bar' + str(nodeA.tag) + str(nodeB.tag) + nmbTransf
        E= 1e14*stiffnessFactor
        G= E/(2.0*(1+0.3)) # nu= 0.3
        side= 3.0
        area= side**2
        inertia= 1/12.0*side**4
        torsional_inertia= inertia/100
        (A,Iz,Iy,J)= (area, inertia, inertia, torsional_inertia)
        scc= tm.defElasticSection3d(self.preprocessor,matName,A,E,G,Iz,Iy,J)
        if(__debug__):
            if(not scc):
                AssertionError('Can\'t create the section.')
        return self.setBeamBetweenNodes(nodeA, nodeB, matName, nmbTransf, trfType, beamElementType= 'ElasticBeam3d')

    def setHugeTrussBetweenNodes(self,nodeA, nodeB, stiffnessFactor= 1.0):
        '''
        Creates a very stiff bar between the two nodes being passed as parameters.

        :param   nodeA: tag of bar's from node.
        :param   nodeB: bar's to node.
        :param stiffnessFactor: factor to apply to the beam stiffness.
        '''
        elements= self.getElementHandler()
        # Material definition
        matName= 'truss' + str(nodeA.tag) + str(nodeB.tag)
        (A,E)=( 10 , 1e14*stiffnessFactor)
        mat= tm.defElasticMaterial(self.preprocessor, matName,E)
        if(__debug__):
            if(not mat):
                AssertionError('Can\'t create the material.')
        elements.dimElem= 3
        elements.defaultMaterial= matName
        elem= elements.newElement("Truss",xc.ID([nodeA.tag,nodeB.tag]))
        elem.sectionArea=A
        return elem

    def releaseLineExtremities(self, ln, stiffnessFactors= [1.0e7,1.0e7,1.0e7,.001,.001,.001], extremitiesToRelease= [0,1]):
        ''' Releases the rotational degrees of fredom at the extremities 
            of the line.

        :param ln: line whose end elements will be pinned.
        :stiffnessFactors: factors that multiply the element stiffnesses on 
                           each DOF: [KX, KY, KZ, KrotX, KrotY, KrotZ]
                           the axis correspond to the local axis of the element.
        :param extremitiesToRelease: indexes of the element extremities 
                                     to release: [0]->only the "from" point
                                     [1]->only the "to" point
                                     [0,1]-> both extremities.
        '''
        # nodes= self.preprocessor.getNodeHandler
        if(0 in extremitiesToRelease):
            n0= ln.firstNode # First node.
            connectedElements= ln.getConnectedElements(n0)
            if(len(connectedElements)>0):
                e0= connectedElements[0] # First element.
                nodeIndex= e0.find(n0) # Index of the node in the element.
                self.releaseBeamEnd(e0, stiffnessFactors, [nodeIndex])
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; last node of the line not connected to any element of the line itself. Has been already released?')
        if(1 in extremitiesToRelease):
            n1= ln.lastNode # Last node.
            connectedElements= ln.getConnectedElements(n1)
            if(len(connectedElements)>0):
                e1= connectedElements[0] # First element.
                nodeIndex= e1.find(n1) # Index of the node in the element.
                self.releaseBeamEnd(e1, stiffnessFactors, [nodeIndex])
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; last node of the line not connected to any element of the line itself. Has been already released?')
        
    def releaseBeamEnd(self, beamElement, stiffnessFactors, nodesToRelease):
        ''' Releases some degrees of fredom at the extremities of the beam element.

        :param beamElement: element that will be released.
        :stiffnessFactors: factors that multiply the element stiffnesses on 
                           each DOF: [KX, KY, KZ, KrotX, KrotY, KrotZ]
                           the axis correspond to the local axis of the element.
        :param nodesToRelease: indexes of the element nodes to release.
        '''
        if(len(nodesToRelease)>0):
            beamMaterial= beamElement.physicalProperties.getVectorMaterials[0]
            # Read stiffnesses from element material
            tangent= beamMaterial.getInitialTangentStiffness()
            if(tangent.noRows!=4):
                lmsg.warning('release of shear stifnesses not implemented yet.')
            if(len(stiffnessFactors)!=6):
                lmsg.error('6 stifness factors expected.')                
            KX= tangent(0,0)*stiffnessFactors[0]
            KY= 1e9*stiffnessFactors[1] # Shear not implemented yet. 
            KZ= 1e9*stiffnessFactors[2] # Shear not implemented yet.
            KrotX= tangent(3,3)*stiffnessFactors[3]
            KrotY= tangent(2,2)*stiffnessFactors[4]
            KrotZ= tangent(1,1)*stiffnessFactors[5]
            matKX= tm.defElasticMaterial(self.preprocessor,'matKX'+str(KX),KX)
            matKY= tm.defElasticMaterial(self.preprocessor,'matKY'+str(KY),KY)
            matKZ= tm.defElasticMaterial(self.preprocessor,'matKZ'+str(KZ),KZ)
            matKrotX= tm.defElasticMaterial(self.preprocessor,'matKrotX'+str(KrotX),KrotX)
            matKrotY= tm.defElasticMaterial(self.preprocessor,'matKropY'+str(KrotY),KrotY)
            matKrotZ= tm.defElasticMaterial(self.preprocessor,'matKrotZ'+str(KrotZ),KrotZ)
            releaseMats=  [matKX,matKY,matKZ,matKrotX,matKrotY,matKrotZ]
            releaseMatsNames=[mat.name for mat in releaseMats]
            vx= xc.Vector(beamElement.getIVector3d(False))
            vy= xc.Vector(beamElement.getJVector3d(False))
            newNodes= list()
            newElements= list()
            for iNod in nodesToRelease:
                nodeToRelease= beamElement.getNodes[iNod]
                nodes= self.preprocessor.getNodeHandler
                newNode= nodes.duplicateNode(nodeToRelease.tag) # new node.
                # Connect the beam with the new node.
                beamElement.setIdNode(iNod, newNode.tag)
                # Put the zero length element between the nodes.
                newElement= self.setBearingBetweenNodes( nodeToRelease.tag, newNode.tag, releaseMatsNames,orientation= [vx, vy])
                newNodes.append(newNode)
                newElements.append(newElement)
        return (newNodes, newElements)

    def distributeLoadOnNodes(self, loadSVS, nodeSet, loadPattern= None):
        ''' Distribute the load (represented by a sliding vector system
            between the nodes of the set.

        :param loadSVS: sliding vector system representing the load to be
                        distributed.
        :param nodeSet: the nodes receiving the loads.
        :param loadPattern: load pattern to create the loads into. If None
                            use the current load pattern.
        '''
        ptList= list()
        nodeList= nodeSet.nodes
        if(len(nodeList)>0):
            for n in nodeList:
                ptList.append(n.getInitialPos3d)
            loadVectors= loadSVS.distribute(ptList)
            if(not loadPattern):
                loadPattern= self.getCurrentLoadPattern()
            for n, v in zip(nodeList,loadVectors):
                f= v.getVector3d()
                loadPattern.newNodalLoad(n.tag, xc.Vector([f.x,f.y,f.z,0.0,0.0,0.0]))
        else:
            lmsg.warning("Set: '"+nodeSet.name+"' argument has no nodes.")
                

def getStructuralMechanics3DSpace(preprocessor: xc.Preprocessor):
    '''Return a tStructuralMechanics3DSpace from an
       already defined preprocessor.

       :param preprocessor: preprocessor of the finite element problem.
    '''
    nodes= preprocessor.getNodeHandler
    assert(nodes.dimSpace==3)
    assert(nodes.numDOFs==6)
    return StructuralMechanics3D(nodes)

def getModelSpaceFromPreprocessor(preprocessor: xc.Preprocessor):
    '''Return a PredefinedSpace from the dimension of the space 
       and the number of DOFs for each node obtained from the preprocessor.

       :param preprocessor: preprocessor of the finite element problem.
    '''
    retval= None
    nodes= preprocessor.getNodeHandler
    dimSpace= nodes.dimSpace
    numDOFs= nodes.numDOFs
    if(dimSpace==2):
        if(numDOFs==2):
            retval= SolidMechanics2D(nodes)
        elif(numDOFs==3):
            retval= StructuralMechanics2D(nodes)
        else:
            lmsg.error('cannot figure out the problem type dimSpace= '+str(dimSpace)+' numDOFs= '+str(numDOFs))
    elif(dimSpace==3):
        if(numDOFs==3):
            retval= SolidMechanics3D(nodes)
        elif(numDOFs==6):
            retval= StructuralMechanics3D(nodes)
        else:
            lmsg.error('cannot figure out the problem type dimSpace= '+str(dimSpace)+' numDOFs= '+str(numDOFs))
    else:
       lmsg.error('cannot figure out the problem type dimSpace= '+str(dimSpace)+' numDOFs= '+str(numDOFs))
    return retval
      
def gdls_resist_materials3D(nodes):
    '''Define the dimension of the space: nodes by three coordinates (x,y,z) and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

    :param nodes: preprocessor nodes handler
    '''
    lmsg.warning('gdls_resist_materials3D DEPRECATED; use StructuralMechanics3D.')
    return StructuralMechanics3D(nodes)

def ConstraintsForLineInteriorNodes(lineSet, constraint):
    '''Apply constraint on the interior nodes of a model line.'''

    tags= lineSet.getNodeLayers.getLayer(0).getTagsInteriorNodes()
    for i in tags:
        constraint(i)

def ConstraintsForLineExtremeNodes(lineSet, constraint):
    '''Apply constraint on the extreme nodes of a model line.'''
    fN= lineSet.firstNode.tag
    lN= lineSet.lastNode.tag
    constraint(fN)
    constraint(lN)

def glueSets(preprocessor: xc.Preprocessor,DOF2Glue,masterSet,slaveSet,onCoord=None):
    '''Creates rigid links between nodes in masterSet and their 
    corresponding ones (for reasons of proximity) in slaveSet.
    Each node in masterSet coupled with one and only one node in
    slaveSet. 

    :param preprocessor: preprocessor of the finite element problem.
    :param DOF2Glue: degrees of freedom to match. (e.g. [1,4] equals 
           DOF 1 and 4 between coupled nodes.
    :param masterSet: set with the master nodes
    :param slaveSet: set with the slave nodes
    :param onCoord: coordinate on which the search of the nearest node is 
           based ('X','Y','Z'). If None or other value, the search is
           based on the minimum distance. (Defaults to None)
    '''
    mdlSpace=getModelSpace(preprocessor)
    if onCoord in ['x','y','z','X','Y','Z']:
        indCoo=['x','y','z'].index(onCoord.lower())
        nodSlave=np.array([n for n in slaveSet.nodes])
        cooSlave=np.array([n.get3dCoo[indCoo] for n in nodSlave])
        slSize=len(nodSlave)
        for n in masterSet.nodes:
            aux=np.full((slSize),n.get3dCoo[indCoo])
            nSlave=nodSlave[abs(aux-cooSlave).argmin()]
            mdlSpace.constraints.newEqualDOF(n.tag,nSlave.tag,xc.ID(DOF2Glue))
    else:
        for n in masterSet.nodes:
            nSlave=slaveSet.nodes.getNearestNode(geom.Pos3d(n.get3dCoo[0],n.get3dCoo[1],n.get3dCoo[2]))
            mdlSpace.constraints.newEqualDOF(n.tag,nSlave.tag,xc.ID(DOF2Glue))
  

