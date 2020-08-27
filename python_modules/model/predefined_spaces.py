# -*- coding: utf-8 -*-

from __future__ import print_function
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import xc
import numpy as np
from misc_utils import log_messages as lmsg
import geom
from materials import typical_materials as tm
from postprocess import extrapolate_elem_attr
from solution import predefined_solutions
import uuid

class PredefinedSpace(object):
    def __init__(self,nodes,dimSpace,numDOFs):
        '''Defines the dimension of the space and the number 
         of DOFs for each node.

         :param nodes: preprocessor nodes handler
         :param dimSpace: dimension of the space (1, 2 or 3)
         :param numDOFs: number of degrees of freedom for each node.
        '''
        self.setPreprocessor(nodes.getPreprocessor)
        nodes.dimSpace= dimSpace
        nodes.numDOFs= numDOFs
        self.analysis= None

    def getProblem(self):
        ''' Return the XC finite element problem object.
        '''
        return self.preprocessor.getProblem

    def getElements(self, tags):
        ''' Return the elements that correspond to the argument
            tags.

        :param tags: element tags.
        '''
        retval= list()
        elementHandler= self.preprocessor.getElementHandler
        for t in tags:
            e= elementHandler.getElement(t)
            retval.append(e)
        return retval

    def getNodes(self, tags):
        ''' Return the nodes that correspond to the argument
            tags.

        :param tags: node tags.
        '''
        retval= list()
        nodeHandler= self.preprocessor.getNodeHandler
        for t in tags:
            n= nodeHandler.getNode(t)
            retval.append(n)
        return retval    
    
    def getIntForceComponentFromName(self,componentName):
        if componentName[0] in ['N','M']:
            return componentName.lower()
        elif componentName == 'Q1':
            return 'q13'
        elif componentName == 'Q2':
            return 'q23'
        else: #LCPT I don't like this too much, I prefer let the user make the program to crass. Maybe a Warning? 
            lmsg.error('Item '+str(componentName) + ' is not a valid component. Available components are: N1, N2, N12, M1, M2, M12, Q1, Q2')
            return 'N1'
        
    def setPreprocessor(self,preprocessor):
        '''Sets suitable values for the members from the dimension of the space 
         and the number of DOFs for each node obtained from the argument.

         :param preprocessor: preprocessor of the finite element problem.
        '''
        self.preprocessor= preprocessor
        self.constraints= self.preprocessor.getBoundaryCondHandler

    def getNodeHandler(self):
        ''' Return the preprocessor node handler.'''
        return self.preprocessor.getNodeHandler
    
    def getMaterialHandler(self):
        ''' Return the preprocessor material handler.'''
        return self.preprocessor.getMaterialHandler
    
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
        
    def setPrescribedDisplacements(self,nodeTag,prescDisplacements):
        '''Prescribe displacement for node DOFs.

        :param nodeTag: tag of the node.
        :param prescDisplacements: (list) values of the displacements.
        '''

        numDOFs= self.preprocessor.getNodeHandler.numDOFs
        numDisp= len(prescDisplacements)
        if(numDisp<numDOFs):
            lmsg.warning('prescribed '+str(numDisp)+' displacements, nDOFS= '+str(numDOFs))
        sz= min(numDOFs,numDisp)
        for i in range(0,sz):
            spc= self.constraints.newSPConstraint(nodeTag,i,prescDisplacements[i])

    def setRigidBeamBetweenNodes(self,nodeTagA, nodeTagB):
        '''Create a rigid beam between the nodes passed as parameters.

        :param   nodeTagA: tag of the master node.
        :param   nodeTagB: tag of the slave node.
        '''
        return self.constraints.newRigidBeam(nodeTagA,nodeTagB)

    def setRigidRodBetweenNodes(self,nodeTagA, nodeTagB):
        '''Create a rigid rod between the nodes passed as parameters.

        :param   nodeTagA: tag of the master node.
        :param   nodeTagB: tag of the slave node.
        '''
        return self.constraints.newRigidRod(nodeTagA,nodeTagB)

    def newEqualDOF(self,nodeTagA, nodeTagB,dofs):
        '''Create an equal DOF constraint between the nodes.

        :param   nodeTagA: tag of the master node.
        :param   nodeTagB: tag of the slave node.
        :param dofs: degrees of freedom to be glued 
                   (e.g.: dofs=xc.ID([0,3,6]) means to equal ux,uz,rotz)
        '''
        return self.constraints.newEqualDOF(nodeTagA,nodeTagB,dofs)
    
    def setFulcrumBetweenNodes(self,nodeTagA, pivotNode):
        '''Create a fulcrum between the nodes passed as parameters.

        Creates a rigid link between the nodes.
        It's called fulcrum because it's pinned on pivotNode.

        :param   nodeTagA: tag of the primary node.
        :param   pivotNode: tag of the pivot (secondary node).
        :param dofs: degrees of freedom to be glued 
                   (e.g.: dofs=xc.ID([0,3,6]) means to equal ux,uz,rotz)
        '''
        nodes= self.preprocessor.getNodeHandler
        coordNodeB= nodes.getNode(pivotNode).getCoo
        fulcrumNode= nodes.newNodeFromVector(coordNodeB)
        rb= self.constraints.newRigidBeam(nodeTagA,fulcrumNode.tag)
        ed= self.constraints.newEqualDOF(fulcrumNode.tag,pivotNode,xc.ID(self.getDisplacementDOFs()))
        return fulcrumNode

    def setBearingBetweenNodes(self,iNodA,iNodB,bearingMaterialNames,orientation= None):
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
          :return: newly created zero length element that represents the bearing.

        '''
        # Element definition
        elems= self.preprocessor.getElementHandler
        elems.dimElem= self.preprocessor.getNodeHandler.dimSpace # space dimension.
        elems.defaultMaterial= next((item for item in bearingMaterialNames if item is not None), 'All are Nones')
        zl= elems.newElement("ZeroLength",xc.ID([iNodA,iNodB]))
        zl.clearMaterials()
        if(orientation): #Orient element.
            zl.setupVectors(orientation[0],orientation[1])
        numMats= len(bearingMaterialNames)
        for i in range(0,numMats):
            material= bearingMaterialNames[i]
            if(material!=None):
                zl.setMaterial(i,material)
        return zl

    def setBearing(self,iNod,bearingMaterialNames, orientation= None):
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
        newElement= self.setBearingBetweenNodes(newNode.tag,iNod,bearingMaterialNames,orientation)
        # Boundary conditions
        constraints= self.preprocessor.getBoundaryCondHandler
        numDOFs= self.preprocessor.getNodeHandler.numDOFs
        for i in range(0,numDOFs):
            spc= constraints.newSPConstraint(newNode.tag,i,0.0)
        return newNode, newElement

    def setBearingOnX(self,iNod,bearingMaterial):
        '''Modelize a bearing on X direction.

           :param iNod (int): node identifier (tag).
           :param bearingMaterial (string): material name for the zero length
                 element.
        '''
        return setBearing(iNod,[bearingMaterial])

    def setBearingOnXYRigZ(self,iNod,bearingMaterialNames):
        '''Modelize a non rigid on X and Y directions and rigid on Z bearing.

           :param   iNod (int): node identifier (tag).
           :param bearingMaterialNames (string): material names for the zero length element.
        '''
        newNode, newElement= self.setBearing(iNod,bearingMaterialNames)
        eDofs= self.constraints.newEqualDOF(newNode.tag,iNod,xc.ID([2]))
        return newNode, newElement

    def setUniaxialBearing2D(self,iNod,bearingMaterial,direction):
        '''Modelize an uniaxial bearing on the defined direction.

         :param iNod (int): node identifier (tag).
         :param  bearingMaterial (str): material name for the zero length
                 element.
          :return rtype: (int, int) new node tag, new element tag.
        '''
        nodes= self.preprocessor.getNodeHandler
        newNode= nodes.duplicateNode(iNod) # new node.
        # Element definition
        elems= self.preprocessor.getElementHandler
        elems.dimElem= self.preprocessor.getNodeHandler.dimSpace # space dimension.
        if(elems.dimElem>2):
            lmsg.warning("Not a bi-dimensional space.")
        elems.defaultMaterial= bearingMaterial
        zl= elems.newElement("ZeroLength",xc.ID([newNode.tag,iNod]))
        zl.setupVectors(xc.Vector([direction[0],direction[1],0]),xc.Vector([-direction[1],direction[0],0]))
        zl.clearMaterials()
        zl.setMaterial(0,bearingMaterial)
        # Boundary conditions
        numDOFs= self.preprocessor.getNodeHandler.numDOFs
        for i in range(0,numDOFs):
            spc= self.constraints.newSPConstraint(newNode.tag,i,0.0)
        return newNode.tag, zl.tag

    def getTotalSet(self):
        '''Return the set that contains all the defined
           entities.'''
        return self.preprocessor.getSets.getSet("total")

    def setSum(self, setName, setsToSum):
        ''' Return a set that contains the union of the
            arguments.

        :param setName: name of the set to create. If 'auto'
                        then assign a random (unique) name.
        :param setsToSum: sets to sum in a list.
        '''
        if(setName=='auto'):
            setName= uuid.uuid4().hex
        retval= self.preprocessor.getSets.defSet(setName)
        for s in setsToSum:
            retval+= s
        retval.name= setName # remove all the expressions from name.
        return retval
    
    def setIntersection(self, setName, setsToIntersect):
        ''' Return a set that contains the union of the
            arguments.

        :param setName: name of the set to create. If 'auto'
                        then assign a random (unique) name.
        :param setsToSum: sets to sum in a list.
        '''
        if(setName=='auto'):
            setName= uuid.uuid4().hex
        retval= self.preprocessor.getSets.defSet(setName)
        retval+= setsToIntersect[0]
        for s in setsToIntersect[1:]:
            retval*= s
        retval.name= setName # remove all the expressions from name.
        return retval
    
    def removeAllLoadPatternsFromDomain(self):
        ''' Remove all load patterns from domain.'''
        self.preprocessor.getDomain.removeAllLoadPatterns()
        
    def revertToStart(self):
        ''' Revert the domain to its initial state..'''
        self.preprocessor.getDomain.revertToStart()

    def addLoadCaseToDomain(self, loadCaseName):
        '''Add the load case argument (load pattern or
           combination) to the domain.

           :param loadCaseName: name of the load pattern or combination.
        '''
        self.preprocessor.getLoadHandler.addToDomain(loadCaseName)

    def removeLoadCaseFromDomain(self, loadCaseName):
        '''Add the load case argument (load pattern or
           combination) to the domain.

           :param loadCaseName: name of the load pattern or combination.
        '''
        self.preprocessor.getLoadHandler.removeFromDomain(loadCaseName)
        
    def addNewLoadCaseToDomain(self, loadCaseName, loadCaseExpression):
        '''Defines a new combination and add it to the domain.

           :param loadCaseName: name of the load pattern or combination.
           :param loadCaseExpression: expression that defines de load case as a
                                      combination of previously defined actions
                                      e.g. '1.0*GselfWeight+1.0*GearthPress'
        '''
        combs= self.preprocessor.getLoadHandler.getLoadCombinations
        lCase=combs.newLoadCombination(loadCaseName,loadCaseExpression)
        self.preprocessor.resetLoadCase()
        self.addLoadCaseToDomain(loadCaseName)

    def createSelfWeightLoad(self,xcSet, gravityVector):
        ''' Creates the self-weight load on the elements.

         :param xcSet: set with the lines to load.
        '''
        for e in xcSet.getElements:
            e.createInertiaLoad(gravityVector)

    def analyze(self, numSteps= 1, calculateNodalReactions= False, includeInertia= False):
        ''' Triggers the analysis of the model with a simple static linear
            solution.

        :param numSteps: number of analysis steps.
        :param calculateNodalReactions: if true calculate reactions at
                                        nodes.
        :param includeInertia: if true calculate reactions including inertia
                               effects.
        '''
        result= 0
        problem= self.getProblem()
        if(not self.analysis):
            self.analysis= predefined_solutions.simple_static_linear(problem)
        result= self.analysis.analyze(numSteps)
        if(calculateNodalReactions):
            self.preprocessor.getNodeHandler.calculateNodalReactions(includeInertia,1e-7)
        return result

    def zeroEnergyModes(self, numModes= 1):
        ''' Obtains the zero energy modes of the finite element model.

        :param numModes: number of zero energy modes to obtain.
        '''
        problem= self.getProblem()
        if(self.analysis):
            lmsg.log('Redefining analysis.')
        self.analysis= predefined_solutions.zero_energy_modes(problem)
        return self.analysis.analyze(numModes)
    
    def illConditioningAnalysis(self, numModes= 1):
        ''' Perform an ill-conditioning analysis on the finite element model.

        :param numModes: number of ill-conditioned modes to obtain.
        '''
        problem= self.getProblem()
        if(self.analysis):
            lmsg.log('Redefining analysis.')
        self.analysis= predefined_solutions.ill_conditioning_analysis(problem)
        return self.analysis.analyze(numModes)

    def deactivateElements(self, elemSet, srf= 1e-6):
        ''' Deactivate the elements on the set argument.

        :param elemSet: set of elements to be deactivated.
        :param srf: stress reduction factor for element deactivation.
        '''
        elemSet.killElements()
        mesh= self.preprocessor.getDomain.getMesh
        mesh.setDeadSRF(srf) # set stress reduction factor for
                             # element deactivation.
        lockerName= elemSet.name+'_locker'
        mesh.freezeDeadNodes(lockerName)

    def activateElements(self, elemSet):
        ''' Activate the (previoulsy deactivated) elements on the set argument.

        :param elemSet: set of elements to be deactivated.
        :param srf: stress reduction factor for element deactivation.
        '''
        elemSet.aliveElements()
        mesh= self.preprocessor.getDomain.getMesh
        lockerName= elemSet.name+'_locker'
        mesh.meltAliveNodes(lockerName)

    def getValuesAtNodes(self, element, code):
        ''' Return the values corresponding to code at each of the element nodes.

         :param element: element which the stresses at its nodes will be calculated.
         :param code: magnitude to return (stress, strain,...).
        '''
        return element.getValuesAtNodes(code)
    
    def computeValuesAtNodes(self, setToCompute, propToDefine= 'stress'):
        ''' Extrapolate the stresses to the nodes of the set argument and
            stores them in the property "propToDefine".

        :param setToDefine: set of elements to be processed.
        :param propToDefine: name of the property to define at the nodes.
        '''
        extrapolate_elem_attr.extrapolate_elem_data_to_nodes(setToCompute.getElements,propToDefine,self.getValuesAtNodes, argument= propToDefine, initialValue= xc.Vector([0.0,0.0,0.0,0.0,0.0,0.0]))

    def getDisplacementFileHeader(self):
        ''' Return the string header for the file with the displacements.'''
        retval= ' Comb., Node'
        dispLabels= self.getDisplacementComponentsLabels()
        for dl in dispLabels:
            retval+= ', '+dl
        return retval


def getModelSpace(preprocessor):
      '''Return a PredefinedSpace from the dimension of the space 
       and the number of DOFs for each node obtained from the preprocessor.

       :param preprocessor: preprocessor of the finite element problem.
      '''
      nodes= preprocessor.getNodeHandler
      dimSpace= nodes.dimSpace
      numDOFs= nodes.numDOFs
      return PredefinedSpace(nodes,dimSpace,numDOFs)

class SolidMechanics2D(PredefinedSpace):
    def __init__(self,nodes):
        '''Defines the dimension of the space: nodes by two coordinates (x,y) 
         and two DOF for each node (Ux,Uy)

         :param nodes: preprocessor nodes handler
        '''
        super(SolidMechanics2D,self).__init__(nodes,2,2)
        self.Ux= 0 # Displacement components
        self.Uy= 1
        self.epsilon_11= 0 # Strain components
        self.epsilon_22= 1
        self.epsilon_12= 2
        self.sigma_11= 0 # Stress components
        self.sigma_22= 1
        self.sigma_12= 2

    def getDisplacementComponentsLabels(self):
        ''' Return a list with the labels of the
            displacement components.'''
        return ['uX', 'uY']
        
    def getDispComponentFromName(self,compName):
        '''Return the component index from the
           displacement name.'''
        retval= 0
        if compName == 'uX':
            retval= self.Ux
        elif compName == 'uY':
            retval= self.Uy
        else:
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: uX, uY')
        return retval

    def getStrainComponentFromName(self, compName):
        '''Return the component index from the
           strain name.'''
        retval= 0
        if((compName == 'epsilon_xx') or (compName == 'epsilon_11')):
            retval= self.epsilon_11
        elif((compName == 'epsilon_yy') or (compName == 'epsilon_22')):
            retval= self.epsilon_22
        elif((compName == 'epsilon_xy') or (compName == 'epsilon_12')
             or (compName == 'epsilon_yx') or (compName == 'epsilon_21')):
            retval= self.epsilon_12
        else:
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: epsilon_11, epsilon_22, epsilon_12')
        return retval

    def getStressComponentFromName(self, compName):
        '''Return the component index from the
           stress name.'''
        retval= 0
        if((compName == 'sigma_xx') or (compName == 'sigma_11')):
            retval= self.sigma_11
        elif((compName == 'sigma_yy') or (compName == 'sigma_22')):
            retval= self.sigma_22
        elif((compName == 'sigma_xy') or (compName == 'sigma_12')
             or (compName == 'sigma_yx') or (compName == 'sigma_21')):
            retval= self.sigma_12
        else:
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: sigma_11, sigma_22, sigma_12')
        return retval

    def getDisplacementDOFs(self):
        ''' Return the indices of the displacement DOFs.'''
        return [self.Ux,self.Uy]

    def getRotationalDOFs(self):
        ''' Return the indices of the rotational DOFs.'''
        return []

    def getDisplacementVector(self,nodeTag):
        ''' Return a vector with the displacement components of the node motion.'''
        nod= self.preprocessor.getNodeHandler.getNode(nodeTag)
        disp= nod.getDisp
        return xc.Vector([disp[self.Ux],disp[self.Uy]])
    
    def fixNode00(self, nodeTag):
        '''Restrain all three node DOFs (i. e. make them zero).

         :param nodeTag: node identifier.
        '''
        self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
        self.constraints.newSPConstraint(nodeTag,1,0.0)

    def fixNode0F(self, nodeTag):
        '''Restrain only displacement DOFs (i. e. Ux= 0 and Uy= 0).

         :param nodeTag: node identifier.
        '''
        self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
        
    def fixNodeF0(self, nodeTag):
        '''Restrain only displacement DOFs (i. e. Ux= 0 and Uy= 0).

         :param nodeTag: node identifier.
        '''
        self.constraints.newSPConstraint(nodeTag,1,0.0) # nodeTag, DOF, constrValue

def gdls_elasticidad2D(nodes):
    '''Defines the dimension of the space: nodes by two coordinates (x,y) and two DOF for each node (Ux,Uy)

    :param nodes: nodes handler
    '''
    lmsg.warning('gdls_elasticidad2D DEPRECATED; use SolidMechanics2D.')
    return SolidMechanics2D(nodes)

class StructuralMechanics(PredefinedSpace):
    '''Structural mechanics finite element problem.'''
    def __init__(self,nodes,dimSpace,numDOFs):
        '''Defines the dimension of the space: nodes by two coordinates (x,y) 
         and three DOF for each node (Ux,Uy,theta)

         :param nodes: preprocessor nodes handler
         :param dimSpace: dimension of the space (1, 2 or 3)
         :param numDOFs: number of degrees of freedom for each node.
        '''
        super(StructuralMechanics,self).__init__(nodes,dimSpace,numDOFs)
            
    def createTrusses(self, xcSet, material, area, crossSection= None, corotational= False):
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
        numDOFs= self.preprocessor.getNodeHandler.numDOFs
        dimElem= self.preprocessor.getNodeHandler.dimSpace
        seedElemHandler= self.preprocessor.getElementHandler.seedElemHandler
        seedElemHandler.defaultMaterial= material.name
        seedElemHandler.dimElem= dimElem
        for l in xcSet.getLines:
            l.nDiv= 1
            l.setProp('material',material)
            l.setProp('area',area)
            elem= seedElemHandler.newElement(elementType,xc.ID([0,0]))
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
        p1= iNode.getInitialPos3d
        p2= jNode.getInitialPos3d
        sg= geom.Line3d(p1,p2)
        v3d= sg.getKVector
        return xc.Vector([v3d.x, v3d.y, v3d.z])

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
            lmsg.error('Something went wrong; numDOFs= '+str(numDOFs))
        seedElemHandler= self.preprocessor.getElementHandler.seedElemHandler
            
        seedElemHandler.defaultMaterial= xc_material.getName()
        for l in xcSet.getLines:
            l.nDiv= nDiv
            if(xzVector):
                trf.xzVector= xzVector
            else:
                v3d= l.getKVector
                trf.xzVector= xc.Vector([v3d.x, v3d.y, v3d.z])
            elem= seedElemHandler.newElement(elementType,xc.ID([0,0]))
            l.genMesh(xc.meshDir.I)
            if(crossSection):
                for e in l.getElements:
                    e.setProp('crossSection',crossSection)
        xcSet.fillDownwards()

        
class StructuralMechanics2D(StructuralMechanics):
    def __init__(self,nodes):
        '''Defines the dimension of the space: nodes by two coordinates (x,y) 
         and three DOF for each node (Ux,Uy,theta)

         :param nodes: preprocessor nodes handler
        '''
        super(StructuralMechanics2D,self).__init__(nodes,2,3)
        self.Ux= 0 # displacement components
        self.Uy= 1
        self.Theta= 2
        self.epsilon= 0 # generalized strain components; axial,
        self.kappa= 1 # bending,
        self.gamma= 2 # shear
        self.N= 0 # generalized stress components; axial,
        self.M= 1 # bending,
        self.Q= 2 # shear.

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
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: uX, uY, rotZ')
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
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: epsilon, kappa, gamma')
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
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: N, M, Q')
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

    def fixNode000(self, nodeTag):
        '''Restrain all three node DOFs (i. e. make them zero).

         :param nodeTag: node identifier.
        '''
        self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
        self.constraints.newSPConstraint(nodeTag,1,0.0)
        self.constraints.newSPConstraint(nodeTag,2,0.0)

    def fixNode00F(self, nodeTag):
        '''Restrain only displacement DOFs (i. e. Ux= 0 and Uy= 0).

         :param nodeTag: node identifier.
        '''
        self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
        self.constraints.newSPConstraint(nodeTag,1,0.0)

    def fixNode0F0(self, nodeTag):
        '''Restrain all three node DOFs (i. e. make them zero).

         :param nodeTag: node identifier.
        '''
        self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
        self.constraints.newSPConstraint(nodeTag,2,0.0)

    def fixNode0FF(self, nodeTag):
        '''Restrain only X displacement DOF (i. e. Ux= 0).

         :param nodeTag: node identifier.
        '''
        self.preprocessor.getBoundaryCondHandler.newSPConstraint(nodeTag,0,0.0) 

    def fixNodeF0F(self, nodeTag):
        '''Restrain only Y displacement DOF (i. e. Uy= 0).

         :param nodeTag: node identifier.
        '''
        self.preprocessor.getBoundaryCondHandler.newSPConstraint(nodeTag,1,0.0) 

    def fixNodeFF0(self, nodeTag):
        '''Restrain only rotation DOF (i. e. Theta= 0).

         :param nodeTag: node identifier.
        '''
        self.preprocessor.getBoundaryCondHandler.newSPConstraint(nodeTag,2,0.0)

    def fixNodesLine(self, line):
        '''Restrain all DOFs of the line nodes.'''  
        nn= line.getNumNodes
        for i in range(1,nn+1):
            nodeTag= line.getNodeI(i).tag
            self.fixNode000(nodeTag)
            

def getStructuralMechanics2DSpace(preprocessor):
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
    def __init__(self,nodes):
        '''Defines the dimension of the space: nodes by three coordinates (x,y,z) 
         and two DOF for each node (Ux,Uy,Uz)

         :param nodes: preprocessor nodes handler
        '''
        super(SolidMechanics3D,self).__init__(nodes,3,3)
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
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: uX, uY, uZ')
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
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: epsilon_11, epsilon_22, epsilon_33, epsilon_12, epsilon_13, epsilon_23')
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
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: sigma_11, sigma_22, sigma_33, sigma_12, sigma_13, sigma_23')
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

    def fixNode000(self, nodeTag):
        '''Restrain all three node DOFs (i. e. make them zero).

          :param nodeTag: node identifier.
        '''
        self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
        self.constraints.newSPConstraint(nodeTag,1,0.0)
        self.constraints.newSPConstraint(nodeTag,2,0.0)
        
    def fixNode(self,DOFpattern,nodeTag):
        '''Restrain DOF of a node according to the DOFpattern, which is a given
         string of type '0FF' that matches the DOFs (uX,uY,uZ)
         where 'F' means FREE and '0' means constrained with value=0
         Note: DOFpaterns '0FF','0_FF', ... are equivalent

         :param nodeTag: node identifier.
        '''
        DOFpatclean=DOFpattern.replace('_','')
        DOFtoConstr=[i for i in range(len(DOFpatclean)) if DOFpatclean[i]=='0']
        for nc in DOFtoConstr:
            self.constraints.newSPConstraint(nodeTag,nc,0.0)

def gdls_elasticidad3D(nodes):
    '''Defines the dimension of the space: nodes by three coordinates (x,y,z) 
       and three DOF for each node (Ux,Uy,Uz)

    :param nodes: preprocessor nodes handler
    '''
    lmsg.warning('gdls_elasticidad3D DEPRECATED; use SolidMechanics3D.')
    return SolidMechanics3D(nodes)


class StructuralMechanics3D(StructuralMechanics):
    def __init__(self,nodes):
        '''Define the dimension of the space: nodes by three coordinates (x,y,z) 
        and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

        :param nodes: preprocessor nodes handler
        '''
        super(StructuralMechanics3D,self).__init__(nodes,3,6)
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
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: uX, uY, uZ, rotX, rotY, rotZ')
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
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: epsilon, kappa_z, gamma_y, kappa_y, gamma_z, theta')
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
            lmsg.error('Item '+str(compName) + ' is not a valid component. Available components are: N, Mz, Qy, My, Qz, T.')
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

    def fixNode(self,DOFpattern,nodeTag):
        '''Restrain DOF of a node according to the DOFpattern, which is a given
         string of type '0FF_00F' that matches the DOFs (uX,uY,uZ,rotX,rotY,rotZ)
         where 'F' means FREE and '0' means constrained with value=0
         Note: DOFpaterns '0FF_00F','0FF00F','0_FF_0_0F', ... are equivalent

         :param nodeTag: node identifier.
        '''
        DOFpatclean=DOFpattern.replace('_','')
        DOFtoConstr=[i for i in range(len(DOFpatclean)) if DOFpatclean[i]=='0']
        for nc in DOFtoConstr:
            self.constraints.newSPConstraint(nodeTag,nc,0.0)

    def fixNode000_000(self, nodeTag):
        '''Restrain all six node DOFs (i. e. make them zero).

         :param nodeTag: node identifier.
        '''
        self.fixNode("000_000",nodeTag)

    def fixNode000_FFF(self, nodeTag):
        '''Restrain only displacement DOFs (i. e. Ux= 0, Uy= 0 and Uz= 0).

         :param nodeTag: node identifier.
        '''
        self.fixNode('000_FFF', nodeTag)

    def fixNode000_0FF(self, nodeTag):
        '''Restrain the displacements (Ux,Uy and Uz) and
         the rotation about X axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('000_0FF', nodeTag)

    def fixNode000_F0F(self, nodeTag):
        '''Restrain the displacements (Ux,Uy and Uz) and
         the rotation about Y axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('000_F0F', nodeTag)

    def fixNode000_FF0(self, nodeTag):
        '''Restrain the displacements (Ux,Uy and Uz) and
         the rotation about Z axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('000_FF0', nodeTag)


    def fixNode000_F00(self, nodeTag):
        '''Restrain all DOFs except for the rotation about X axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('000_F00', nodeTag)

    def fixNode000_0F0(self, nodeTag):
        '''Restrain all DOFs except for the rotation about Y axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('000_0F0', nodeTag)

    def fixNode000_00F(self, nodeTag):
        '''Restrain all DOFs except for the rotation about Z axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('000_00F', nodeTag)

    def fixNodeF00_F00(self, nodeTag):
        '''Restrain all DOFs except for X displacement and the
         rotation about X axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('F00_F00', nodeTag)

    def fixNodeF00_0F0(self, nodeTag):
        '''Restrain all DOFs except for X displacement and the
         rotation about Y axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('F00_0F0', nodeTag)

    def fixNodeF00_00F(self, nodeTag):
        '''Restrain all DOFs except for X displacement and the
         rotation about Z axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('F00_00F', nodeTag)

    def fixNodeF00_0FF(self, nodeTag):
        '''Restrain all DOFs except for X displacement and the
         rotations about Y and Z axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('F00_0FF', nodeTag)

    def fixNodeF00_FFF(self, nodeTag):
        '''Restrain all DOFs except for X displacement and the
         rotations about Y and Z axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('F00_FFF', nodeTag)

    def fixNode0F0_F00(self, nodeTag):
        '''Restrain all DOFs except for Y displacement and the
         rotation about X axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('0F0_F00', nodeTag)

    def fixNode0F0_0F0(self, nodeTag):
        '''Restrain all DOFs except for Y displacement and the
         rotation about Y axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('0F0_0F0', nodeTag)

    def fixNode0F0_00F(self, nodeTag):
        '''Restrain all DOFs except for Y displacement and the
         rotation about Z axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('0F0_00F', nodeTag)

    def fixNode0F0_0FF(self, nodeTag):
        '''Restrain all DOFs except for Y displacement and the
         rotations about Y and Z axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('0F0_0FF', nodeTag)

    def fixNode0F0_FFF(self, nodeTag):
        '''Restrain all DOFs except for Y displacement and the
         rotations about Y and Z axis.

         :param nodeTag: node identifier.
        '''
        self.fixNode('0F0_FFF', nodeTag)

    def fixNodeFFF_000(self, nodeTag):
        '''Restrain only rotations (i. e. ThetaX= 0, ThetaY= 0 and ThetaZ= 0).

         :param nodeTag: node identifier.
        '''
        self.fixNode('FFF_000', nodeTag)

    def fixNodeFFF_0F0(self, nodeTag):
        '''Restrain only X and Z rotations (i. e. ThetaX= 0, and ThetaZ= 0).

         :param nodeTag: node identifier.
        '''
        self.fixNode('FFF_0F0', nodeTag)

    def fixNode0FF_FFF(self, nodeTag):
        '''Restrain only X displacement DOF (i. e. Ux= 0).

         :param nodeTag: node identifier.
        '''
        self.fixNode('0FF_FFF', nodeTag)

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
                    self.constraints.newSPConstraint(n.tag,i,constrCond[i])
                    
                    
    def setHugeBeamBetweenNodes(self,nodeTagA, nodeTagB, nmbTransf):
        '''
        Creates a very stiff bar between the two nodes being passed as parameters.
        (it was a workaround to the problem with the reactions values in nodes when
        using multipoint constraints. This problem has been be solved with the
        implementation of MFreedom_ConstraintBase::addResistingForceToNodalReaction).

        :param   nodeTagA: tag of bar's from node.
        :param   nodeTagB: tag of bar's to node.
        :param   nmbTransf: name of the coordinate transformation to use for the new bar.
        '''
        elements= self.preprocessor.getElementHandler
        elements.defaultTransformation= nmbTransf
        # Material definition
        matName= 'bar' + str(nodeTagA) + str(nodeTagB) + nmbTransf
        (A,E,G,Iz,Iy,J)= (10, 1e14 , 1e12 , 10, 10, 10)
        scc= tm.defElasticSection3d(self.preprocessor,matName,A,E,G,Iz,Iy,J)
        elements.defaultMaterial= matName
        elem= elements.newElement("ElasticBeam3d",xc.ID([nodeTagA,nodeTagB]))
        scc= elem.sectionProperties
        return elem

    def setHugeTrussBetweenNodes(self,nodeTagA, nodeTagB):
        '''
        Creates a very stiff bar between the two nodes being passed as parameters.

        :param   nodeTagA: tag of bar's from node.
        :param   nodeTagB: tag of bar's to node.
        '''
        elements= self.preprocessor.getElementHandler
        # Material definition
        matName= 'truss' + str(nodeTagA) + str(nodeTagB)
        (A,E)=( 10 , 1e14)
        mat= tm.defElasticMaterial(self.preprocessor, matName,E)
        elements.dimElem= 3
        elements.defaultMaterial= matName
        elem= elements.newElement("Truss",xc.ID([nodeTagA,nodeTagB]))
        elem.sectionArea=A
        return elem

    def releaseBeamEnd(self, beamElement, stiffnessFactors, nodesToRelease):
        ''' Releases some degrees of fredom at the extremities of the beam element.

        :param beamElement: element that will be released.
        :stiffnessFactors: factors that multiply the element stiffnesses on 
                           each DOF: [KX, KY, KZ, KrotX, KrotY, KrotZ]
                           the axis correspond to the local axis of the element.
        :param nodesToRelease: indexesof the element nodes to release.
        '''
        retval= list()
        if(len(nodesToRelease)>0):
            beamMaterial= beamElement.getPhysicalProperties.getVectorMaterials[0]
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
            print(KX, KY, KZ, KrotX, KrotY, KrotZ)
            matKX= tm.defElasticMaterial(self.preprocessor,'matKX',KX)
            matKY=tm.defElasticMaterial(self.preprocessor,'matKY',KY)
            matKZ=tm.defElasticMaterial(self.preprocessor,'matKZ',KZ)
            matKrotX=tm.defElasticMaterial(self.preprocessor,'matKrotX',KrotX)
            matKrotY=tm.defElasticMaterial(self.preprocessor,'matKropY',KrotY)
            matKrotZ=tm.defElasticMaterial(self.preprocessor,'matKrotZ',KrotZ)
            releaseMats=  [matKX,matKY,matKZ,matKrotX,matKrotY,matKrotZ]
            releaseMatsNames=[mat.name for mat in releaseMats]
            vx= xc.Vector(beamElement.getIVector3d(False))
            vy= xc.Vector(beamElement.getJVector3d(False))
            for iNod in nodesToRelease:
                nodeToRelease= beamElement.getNodes[iNod]
                nodes= self.preprocessor.getNodeHandler
                newNode= nodes.duplicateNode(nodeToRelease.tag) # new node.
                # Connect the beam with the new node.
                beamElement.setIdNode(iNod, newNode.tag)
                print('beam nodes: ', beamElement.getNodes[0].tag, beamElement.getNodes[1].tag)
                print('BEFORE new node: ', newNode.tag, ' connected elements: ', newNode.getNumberOfConnectedElements())
                print('BEFORE node to release: ', nodeToRelease.tag, ' connected elements: ', nodeToRelease.getNumberOfConnectedElements())
                # Put the zero length element between the nodes.
                self.setBearingBetweenNodes( nodeToRelease.tag, newNode.tag, releaseMatsNames,orientation= [vx, vy])
                print('AFTER new node: ', newNode.tag, ' connected elements: ', newNode.getNumberOfConnectedElements())
                print('AFTER node to release: ', nodeToRelease.tag, ' connected elements: ', nodeToRelease.getNumberOfConnectedElements())
                retval.append(newNode)
        return retval
                

def getStructuralMechanics3DSpace(preprocessor):
    '''Return a tStructuralMechanics3DSpace from an
       already defined preprocessor.

       :param preprocessor: preprocessor of the finite element problem.
    '''
    nodes= preprocessor.getNodeHandler
    assert(nodes.dimSpace==3)
    assert(nodes.numDOFs==6)
    return StructuralMechanics3D(nodes)

def getModelSpaceFromPreprocessor(preprocessor):
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

def glueSets(preprocessor,DOF2Glue,masterSet,slaveSet,onCoord=None):
    '''Creates rigid links between nodes in masterSet and their 
    corresponding ones (for reasons of proximity) in slaveSet.
    Each node in masterSet coupled with one and only one node in
    slaveSet. 

    :param preprocessor: preprocessor
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
  

