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
    nodes.dimEspace= dimSpace
    nodes.numGdls= numGdls

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

def gdls_elasticidad3D(nodes):
  '''Defines the dimension of the space: nodes by three coordinates (x,y,z) 
     and three DOF for each node (Ux,Uy,Uz)

  :param nodes: preprocessor nodes loader
  '''
  lmsg.warning('gdls_elasticidad3D DEPRECATED; use SolidMechanics3D.')
  return SolidMechanics3D(nodes)


class StructuralMechanics3D(PredefinedSpace):
  def __init__(self,nodes):
    '''Defines the dimension of the space: nodes by three coordinates (x,y,z) and 
       six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

       :param nodes: preprocessor nodes loader
    '''
    super(StructuralMechanics3D,self).__init__(nodes,3,6)
    self.Ux= 0
    self.Uy= 1
    self.Uz= 2
    self.ThetaX= 3
    self.ThetaY= 4
    self.ThetaZ= 5
    
def gdls_resist_materiales3D(nodes):
  '''Defines the dimension of the space: nodes by three coordinates (x,y,z) and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

  :param nodes: preprocessor nodes loader
  '''
  lmsg.warning('gdls_resist_materiales3D DEPRECATED; use StructuralMechanics3D.')
  return StructuralMechanics3D(nodes)
