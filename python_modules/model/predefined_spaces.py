# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import xc
from miscUtils import LogMessages as lmsg

class PredefinedSpace(object):
  def __init__(self,nodes,dimSpace,numGdls):
    '''Defines the dimension of the space and the number 
       of DOFs for each node.

       :param nodes: preprocessor nodes loader
       :param dimSpace: dimension of the space (1, 2 or 3)
       :param numGdls: number of degrees of freedom for each node.
    '''
    self.setPreprocessor(nodes.getPreprocessor)
    nodes.dimSpace= dimSpace
    nodes.numGdls= numGdls
    
  def setPreprocessor(self,preprocessor):
    '''Sets suitable values for the members from the dimension of the space 
       and the number of DOFs for each node obtained from the argument.

       :param preprocessor: preprocessor of the finite element problem.
    '''
    self.preprocessor= preprocessor
    self.constraints= self.preprocessor.getConstraintLoader


  def setBearingBetweenNodes(self,iNodA,iNodB,bearingMaterials):
    '''Modelize a bearing between the nodes

        Args:
            :param iNodA: (int) first node identifier (tag).
            :param iNodB: (int) second node identifier (tag).
            :param bearingMaterials: (list) material names for the zero length
               element.
        :return: newly created zero length element that represents the bearing.
            
    '''
    # Element definition
    elems= self.preprocessor.getElementLoader
    elems.dimElem= self.preprocessor.getNodeLoader.dimSpace # space dimension.
    elems.defaultMaterial= next((item for item in bearingMaterials if item is not None), 'All are Nones')
    zl= elems.newElement("zero_length",xc.ID([iNodA,iNodB]))
    zl.clearMaterials()
    numMats= len(bearingMaterials)
    for i in range(0,numMats):
      material= bearingMaterials[i]
      if(material!=None):
        zl.setMaterial(i,material)
    return zl

  def setBearing(self,iNod,bearingMaterials):
    '''Modelize a bearing on X, XY or XYZ directions.

        Args:
            iNod: (int) node identifier (tag).
            bearingMaterials (list): material names for the zero length
               element.
        Returns:
            :rtype: (int, int) new node tag, new element tag.
    '''
    nodos= self.preprocessor.getNodeLoader
    newNode= nodos.duplicateNode(iNod) # new node.

    # Element definition
    newElement= self.setBearingBetweenNodes(newNode.tag,iNod,bearingMaterials)
    # Boundary conditions
    constraints= self.preprocessor.getConstraintLoader
    numGdls= self.preprocessor.getNodeLoader.numGdls
    for i in range(0,numGdls):
      spc= constraints.newSPConstraint(newNode.tag,i,0.0)
    return newNode.tag, newElement.tag


  def setBearingOnX(self,iNod,bearingMaterial):
    '''Modelize a bearing on X direction.

        Args:
            iNod (int): node identifier (tag).
            bearingMaterial (string): material name for the zero length
               element.
    '''
    return setBearing(iNod,[bearingMaterial])

  def setBearingOnXYRigZ(self,iNod,bearingMaterials):
    '''Modelize a non rigid on X and Y directions and rigid on Z bearing.

        Args:
            iNod (int): node identifier (tag).
            bearingMaterial (string): material name for the zero length
               element.
    '''
    newNodeTag, newElementTag= self.setBearing(iNod,bearingMaterials)
    eDofs= self.constraints.newEqualDOF(newNodeTag,iNod,xc.ID([2]))
    return newNodeTag, newElementTag

  def setUniaxialBearing2D(self,iNod,bearingMaterial,direction):
    '''Modelize an uniaxial bearing on the defined direction.

        Args:
            iNod (int): node identifier (tag).
            bearingMaterial (str): material name for the zero length
               element.
        Returns:
            :rtype: (int, int) new node tag, new element tag.
    '''
    nodos= self.preprocessor.getNodeLoader
    newNode= nodos.duplicateNode(iNod) # new node.

    # Element definition
    elems= self.preprocessor.getElementLoader
    elems.dimElem= self.preprocessor.getNodeLoader.dimSpace # space dimension.
    if(elems.dimElem>2):
      lmsg.warning("Not a bidimensional space.")
    elems.defaultMaterial= bearingMaterial
    zl= elems.newElement("zero_length",xc.ID([newNode.tag,iNod]))
    zl.setupVectors(xc.Vector([direction[0],direction[1],0]),xc.Vector([-direction[1],direction[0],0]))
    zl.clearMaterials()
    zl.setMaterial(0,bearingMaterial)
    # Boundary conditions
    numGdls= self.preprocessor.getNodeLoader.numGdls
    for i in range(0,numGdls):
      spc= self.constraints.newSPConstraint(newNode.tag,i,0.0)
    return newNode.tag, zl.tag

def getModelSpace(preprocessor):
    '''Return a PredefinedSpace from the dimension of the space 
       and the number of DOFs for each node obtained from the preprocessor.

       :param preprocessor: preprocessor of the finite element problem.
    '''
    nodes= preprocessor.getNodeLoader
    dimSpace= nodes.dimSpace
    numGdls= nodes.numGdls
    return PredefinedSpace(nodes,dimSpace,numGdls)
  

class SolidMechanics2D(PredefinedSpace):
  def __init__(self,nodes):
    '''Defines the dimension of the space: nodes by two coordinates (x,y) 
       and two DOF for each node (Ux,Uy)

       :param nodes: preprocessor nodes loader
    '''
    super(SolidMechanics2D,self).__init__(nodes,2,2)
    self.Ux= 0
    self.Uy= 1

def gdls_elasticidad2D(nodes):
  '''Defines the dimension of the space: nodes by two coordinates (x,y) and two DOF for each node (Ux,Uy)

  :param nodes: nodes loader
  '''
  lmsg.warning('gdls_elasticidad2D DEPRECATED; use SolidMechanics2D.')
  return SolidMechanics2D(nodes)

class StructuralMechanics2D(PredefinedSpace):
  def __init__(self,nodes):
    '''Defines the dimension of the space: nodes by two coordinates (x,y) 
       and three DOF for each node (Ux,Uy,theta)

       :param nodes: preprocessor nodes loader
    '''
    super(StructuralMechanics2D,self).__init__(nodes,2,3)
    self.Ux= 0
    self.Uy= 1
    self.Theta= 2
    
  def fixNode000(self, nodeTag):
    '''Restrain all three node DOFs (i. e. make them zero).'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)
    self.constraints.newSPConstraint(nodeTag,2,0.0)

  def fixNode00F(self, nodeTag):
    '''Restrain only displacement DOFs (i. e. Ux= 0 and Uy= 0).'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)

  def fixNode0FF(self, nodeTag):
    '''Restrain only X displacement DOF (i. e. Ux= 0).'''
    self.preprocessor.getConstraintLoader.newSPConstraint(nodeTag,0,0.0) 

  def fixNodeF0F(self, nodeTag):
    '''Restrain only Y displacement DOF (i. e. Uy= 0).'''
    self.preprocessor.getConstraintLoader.newSPConstraint(nodeTag,1,0.0) 

  def fixNodeFF0(self, nodeTag):
    '''Restrain only rotation DOF (i. e. Theta= 0).'''
    self.preprocessor.getConstraintLoader.newSPConstraint(nodeTag,2,0.0)

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
    nodes= preprocessor.getNodeLoader
    assert(nodes.dimSpace==2)
    assert(nodes.numGdls==3)
    return StructuralMechanics2D(nodes)

def gdls_resist_materiales2D(nodes):
  '''Defines the dimension of the space: nodes by two coordinates (x,y) and three DOF for each node (Ux,Uy,theta)

  :param nodes: preprocessor nodes loader
  '''
  lmsg.warning('gdls_resist_materiales2D DEPRECATED; use StructuralMechanics2D.')
  return StructuralMechanics2D(nodes)

class SolidMechanics3D(PredefinedSpace):
  def __init__(self,nodes):
    '''Defines the dimension of the space: nodes by three coordinates (x,y,z) 
       and two DOF for each node (Ux,Uy,Uz)

       :param nodes: preprocessor nodes loader
    '''
    super(SolidMechanics3D,self).__init__(nodes,3,3)
    self.Ux= 0
    self.Uy= 1
    self.Uz= 2

  def fixNode000(self, nodeTag):
    '''Restrain all three node DOFs (i. e. make them zero).'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)
    self.constraints.newSPConstraint(nodeTag,2,0.0)

def gdls_elasticidad3D(nodes):
  '''Defines the dimension of the space: nodes by three coordinates (x,y,z) 
     and three DOF for each node (Ux,Uy,Uz)

  :param nodes: preprocessor nodes loader
  '''
  lmsg.warning('gdls_elasticidad3D DEPRECATED; use SolidMechanics3D.')
  return SolidMechanics3D(nodes)


class StructuralMechanics3D(PredefinedSpace):
  def __init__(self,nodes):
    '''Define the dimension of the space: nodes by three coordinates (x,y,z) 
    and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

    :param nodes: preprocessor nodes loader
    '''
    super(StructuralMechanics3D,self).__init__(nodes,3,6)
    self.Ux= 0
    self.Uy= 1
    self.Uz= 2
    self.ThetaX= 3
    self.ThetaY= 4
    self.ThetaZ= 5

  def fixNode000_000(self, nodeTag):
    '''Restrain all six node DOFs (i. e. make them zero).'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,3,0.0)
    self.constraints.newSPConstraint(nodeTag,4,0.0)
    self.constraints.newSPConstraint(nodeTag,5,0.0)

  def fixNode000_FFF(self, nodeTag):
    '''Restrain only displacement DOFs (i. e. Ux= 0, Uy= 0 and Uz= 0).'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    
  def fixNode000_0FF(self, nodeTag):
    '''Restrain the displacements (Ux,Uy and Uz) and
       the rotation about X axis.'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,3,0.0)

  def fixNode000_F0F(self, nodeTag):
    '''Restrain the displacements (Ux,Uy and Uz) and
       the rotation about Y axis.'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,4,0.0)

  def fixNode000_FF0(self, nodeTag):
    '''Restrain the displacements (Ux,Uy and Uz) and
       the rotation about Z axis.'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,5,0.0)
    
  def fixNode000_F00(self, nodeTag):
    '''Restrain all DOFs except for the rotation about X axis.'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,4,0.0)
    self.constraints.newSPConstraint(nodeTag,5,0.0)   

  def fixNode000_0F0(self, nodeTag):
    '''Restrain all DOFs except for the rotation about Y axis.'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,3,0.0)
    self.constraints.newSPConstraint(nodeTag,5,0.0)   

  def fixNode000_00F(self, nodeTag):
    '''Restrain all DOFs except for the rotation about Z axis.'''
    self.constraints.newSPConstraint(nodeTag,0,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,1,0.0)
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,3,0.0)
    self.constraints.newSPConstraint(nodeTag,4,0.0)

  def fixNodeF00_F00(self, nodeTag):
    '''Restrain all DOFs except for X displacement and the
       rotation about X axis.'''
    self.constraints.newSPConstraint(nodeTag,1,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,4,0.0)
    self.constraints.newSPConstraint(nodeTag,5,0.0)

  def fixNodeF00_0F0(self, nodeTag):
    '''Restrain all DOFs except for X displacement and the
       rotation about Y axis.'''
    self.constraints.newSPConstraint(nodeTag,1,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,3,0.0)
    self.constraints.newSPConstraint(nodeTag,5,0.0)

  def fixNodeF00_00F(self, nodeTag):
    '''Restrain all DOFs except for X displacement and the
       rotation about Z axis.'''
    self.constraints.newSPConstraint(nodeTag,1,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,3,0.0)
    self.constraints.newSPConstraint(nodeTag,4,0.0)

  def fixNodeF00_0FF(self, nodeTag):
    '''Restrain all DOFs except for X displacement and the
       rotations about Y and Z axis.'''
    self.constraints.newSPConstraint(nodeTag,1,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,2,0.0)
    self.constraints.newSPConstraint(nodeTag,3,0.0)

  def fixNodeFFF_000(self, nodeTag):
    '''Restrain only rotations (i. e. ThetaX= 0, ThetaY= 0 and ThetaZ= 0).'''
    self.constraints.newSPConstraint(nodeTag,3,0.0) # nodeTag, DOF, constrValue
    self.constraints.newSPConstraint(nodeTag,4,0.0)
    self.constraints.newSPConstraint(nodeTag,5,0.0)

  def LstNodes6DOFConstr(lstNodes,constrCond):
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

def getStructuralMechanics3DSpace(preprocessor):
    '''Return a PredefinedSpace from the dimension of the space 
       and the number of DOFs for each node obtained from the preprocessor.

       :param preprocessor: preprocessor of the finite element problem.
    '''
    nodes= preprocessor.getNodeLoader
    assert(nodes.dimSpace==3)
    assert(nodes.numGdls==6)
    return StructuralMechanics3D(nodes)

def gdls_resist_materiales3D(nodes):
  '''Define the dimension of the space: nodes by three coordinates (x,y,z) and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

  :param nodes: preprocessor nodes loader
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
