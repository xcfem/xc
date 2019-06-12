# -*- coding: utf-8 -*-

from __future__ import print_function
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import xc
import numpy as np
from miscUtils import LogMessages as lmsg
import geom

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

    def setPreprocessor(self,preprocessor):
        '''Sets suitable values for the members from the dimension of the space 
         and the number of DOFs for each node obtained from the argument.

         :param preprocessor: preprocessor of the finite element problem.
        '''
        self.preprocessor= preprocessor
        self.constraints= self.preprocessor.getBoundaryCondHandler

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
        '''Create a rigid rod between the nodes passed as parameters.

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

    def setFulcrumBetweenNodes(self,nodeTagA, pivotNode):
        '''Create a fulcrum between the nodes passed as parameters.

        Creates a rigid link between the nodes.
        It's called fulcrumb because it's pinned on pivotNode.

        :param   nodeTagA: tag of the master node.
        :param   nodeTagB: tag of the pivot (slave node).
        '''
        nodes= self.preprocessor.getNodeHandler
        coordNodeB= nodes.getNode(pivotNode).getCoo
        fulcrumNode= nodes.newNodeFromVector(coordNodeB)
        rb= self.constraints.newRigidBeam(nodeTagA,fulcrumNode.tag)
        ed= self.constraints.newEqualDOF(fulcrumNode.tag,pivotNode,xc.ID(self.getDisplacementDOFs()))
        return fulcrumNode

    def setBearingBetweenNodes(self,iNodA,iNodB,bearingMaterialNames,orientation= None):
        '''Modelize a bearing between the nodes

          Args:
              :param iNodA: (int) first node identifier (tag).
              :param iNodB: (int) second node identifier (tag).
              :param bearingMaterialNames: (list) material names for the zero length
                 element.
              :param orientation: (list) vectors used to orient the zero length
                 element; (x,yp) x: axis of the element, yp: vector that lies 
                 on the xy plane of the element.
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

          Args:
              :param iNod: (int) node identifier (tag).
              :param bearingMaterialNames (list): material names for the zero length
                 element.
              :param orientation: (list) vectors used to orient the zero length
                 element; (x,yp) x: axis of the element, yp: vector that lies 
                 on the xy plane of the element.
          Returns:
              :rtype: (int, int) new node tag, new element tag.
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

          Args:
              iNod (int): node identifier (tag).
              bearingMaterial (string): material name for the zero length
                 element.
        '''
        return setBearing(iNod,[bearingMaterial])

    def setBearingOnXYRigZ(self,iNod,bearingMaterialNames):
        '''Modelize a non rigid on X and Y directions and rigid on Z bearing.

          Args:
              iNod (int): node identifier (tag).
              bearingMaterialNames (string): material names for the zero length
                 element.
        '''
        newNode, newElement= self.setBearing(iNod,bearingMaterialNames)
        eDofs= self.constraints.newEqualDOF(newNode.tag,iNod,xc.ID([2]))
        return newNode, newElement

    def setUniaxialBearing2D(self,iNod,bearingMaterial,direction):
        '''Modelize an uniaxial bearing on the defined direction.

          Args:
              iNod (int): node identifier (tag).
              bearingMaterial (str): material name for the zero length
                 element.
          Returns:
              :rtype: (int, int) new node tag, new element tag.
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
        self.Ux= 0
        self.Uy= 1
        
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

def gdls_elasticidad2D(nodes):
    '''Defines the dimension of the space: nodes by two coordinates (x,y) and two DOF for each node (Ux,Uy)

    :param nodes: nodes handler
    '''
    lmsg.warning('gdls_elasticidad2D DEPRECATED; use SolidMechanics2D.')
    return SolidMechanics2D(nodes)

class StructuralMechanics2D(PredefinedSpace):
    def __init__(self,nodes):
        '''Defines the dimension of the space: nodes by two coordinates (x,y) 
         and three DOF for each node (Ux,Uy,theta)

         :param nodes: preprocessor nodes handler
        '''
        super(StructuralMechanics2D,self).__init__(nodes,2,3)
        self.Ux= 0
        self.Uy= 1
        self.Theta= 2
        
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

def gdls_resist_materiales2D(nodes):
    '''Defines the dimension of the space: nodes by two coordinates (x,y) and three DOF for each node (Ux,Uy,theta)

    :param nodes: preprocessor nodes handler
    '''
    lmsg.warning('gdls_resist_materiales2D DEPRECATED; use StructuralMechanics2D.')
    return StructuralMechanics2D(nodes)

class SolidMechanics3D(PredefinedSpace):
    def __init__(self,nodes):
        '''Defines the dimension of the space: nodes by three coordinates (x,y,z) 
         and two DOF for each node (Ux,Uy,Uz)

         :param nodes: preprocessor nodes handler
        '''
        super(SolidMechanics3D,self).__init__(nodes,3,3)
        self.Ux= 0
        self.Uy= 1
        self.Uz= 2

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

def gdls_elasticidad3D(nodes):
    '''Defines the dimension of the space: nodes by three coordinates (x,y,z) 
       and three DOF for each node (Ux,Uy,Uz)

    :param nodes: preprocessor nodes handler
    '''
    lmsg.warning('gdls_elasticidad3D DEPRECATED; use SolidMechanics3D.')
    return SolidMechanics3D(nodes)


class StructuralMechanics3D(PredefinedSpace):
    def __init__(self,nodes):
        '''Define the dimension of the space: nodes by three coordinates (x,y,z) 
        and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

        :param nodes: preprocessor nodes handler
        '''
        super(StructuralMechanics3D,self).__init__(nodes,3,6)
        self.Ux= 0
        self.Uy= 1
        self.Uz= 2
        self.ThetaX= 3
        self.ThetaY= 4
        self.ThetaZ= 5
        
    def getDisplacementDOFs(self):
        ''' Return the indices of the displacement DOFs.'''
        return [self.Ux,self.Uy,self.Uz]

    def getRotationalDOFs(self):
        ''' Return the indices of the rotational DOFs.'''
        return [self.ThetaX,self.ThetaY,self.ThetaZ]

    def getDisplacementVector(self,nodeTag):
        ''' Return a vector with the displacement components of the node motion.

          :param nodeTag: node identifier.
        '''
        nod= self.preprocessor.getNodeHandler.getNode(nodeTag)
        disp= nod.getDisp
        return xc.Vector([disp[self.Ux],disp[self.Uy],disp[self.Uz]])

    def newLinearCrdTransf(self, trfName,xzVector):
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

        :param preprocessor: preprocessor
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
                if(constrCond[i] <> 'free'):
                    self.constraints.newSPConstraint(n.tag,i,constrCond[i])
                    
    def setHugeBarBetweenNodes(self,nodeTagA, nodeTagB, nmbTransf):
        '''
        Creates a very stiff bar between the two nodes being passed as parameters.
        (it was a workaround to the problem with the reactions values in nodes when
        using multipoint constraints. This problem has been be solved with the
        implementation of MFreedom_ConstraintBase::addResistingForceToNodalReaction).

        :param   nodeTagA: tag of bar's from node.
        :param   nodeTagB: tag of bar's to node.
        :param   nmbTransf: name of the coordinate transformation to use for the new bar.
        '''
        elementos= preprocessor.getElementHandler
        elementos.defaultTransformation= nmbTransf
        # Material definition
        matName= 'bar' + str(nodeTagA) + str(nodeTagB) + nmbTransf
        A= 10
        E= 1e14
        G= 1e12
        Iz= 10
        Iy= 10
        J= 10
        scc= typical_materials.defElasticSection3d(preprocessor,matName,A,E,G,Iz,Iy,J)
        defMat= elementos.defaultMaterial
        #print "defMat= ", defMat
        elementos.defaultMaterial= matName
        elem= elementos.newElement("ElasticBeam3d",xc.ID([nodeTagA,nodeTagB]))
        elementos.defaultMaterial= defMat
        scc= elem.sectionProperties
        #print "A= ", elem.sectionProperties.A
        return elem

def getStructuralMechanics3DSpace(preprocessor):
    '''Return a PredefinedSpace from the dimension of the space 
       and the number of DOFs for each node obtained from the preprocessor.

       :param preprocessor: preprocessor of the finite element problem.
    '''
    nodes= preprocessor.getNodeHandler
    assert(nodes.dimSpace==3)
    assert(nodes.numDOFs==6)
    return StructuralMechanics3D(nodes)

def gdls_resist_materiales3D(nodes):
    '''Define the dimension of the space: nodes by three coordinates (x,y,z) and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

    :param nodes: preprocessor nodes handler
    '''
    lmsg.warning('gdls_resist_materiales3D DEPRECATED; use StructuralMechanics3D.')
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
        nodSlave=np.array([n for n in slaveSet.getNodes])
        cooSlave=np.array([n.get3dCoo[indCoo] for n in nodSlave])
        slSize=len(nodSlave)
        for n in masterSet.getNodes:
            aux=np.full((slSize),n.get3dCoo[indCoo])
            nSlave=nodSlave[abs(aux-cooSlave).argmin()]
            mdlSpace.constraints.newEqualDOF(n.tag,nSlave.tag,xc.ID(DOF2Glue))
    else:
        for n in masterSet.getNodes:
            nSlave=slaveSet.getNodes.getNearestNode(geom.Pos3d(n.get3dCoo[0],n.get3dCoo[1],n.get3dCoo[2]))
            mdlSpace.constraints.newEqualDOF(n.tag,nSlave.tag,xc.ID(DOF2Glue))
  
