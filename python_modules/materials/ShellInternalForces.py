# -*- coding: utf-8 -*-
# Shell element internal forces.
# Wood-Armer method for the assessment of reinforced concrete shells.

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials import CrossSectionInternalForces as csif

def transformInternalForces(iForces,theta):
  cos2T= math.cos(2*theta)
  sin2T= math.sin(2*theta)
  tmpA= (iForces[0]+iForces[1])/2.0
  tmpB= (iForces[0]-iForces[1])/2.0*cos2T+iForces[2]*sin2T
  retval= [0.0,0.0,0.0]
  retval[0]= tmpA+tmpB 
  retval[2]= -(iForces[0]-iForces[1])/2.0*sin2T+iForces[2]*cos2T
  retval[1]= tmpA-tmpB
  return retval

class ShellElementInternalForces:
  def __init__(self,n1= 0.0,n2= 0.0,n12= 0.0,m1= 0.0,m2= 0.0,m12= 0.0,q13= 0.0,q23= 0.0):
    '''Internal forces on the shell element

    :ivar n1: axial force parallel to axis 1.
    :ivar n2: axial force parallel to axis 2.
    :ivar n12: in plane shear force.
    :ivar m1: bending moment parallel to axis 1.
    :ivar m2: bending moment parallel to axis 2.
    :ivar m12: twisting stress resultant.
    :ivar q13: out of plane shear force normal to axis 1 and parallel to axis 3.
    :ivar q23: out of plane shear force normal to axis 1 and parallel to axis 3.
    '''
    self.n1= n1
    self.n2= n2
    self.n12= n12
    self.m1= m1
    self.m2= m2
    self.m12= m12
    self.q13= q13
    self.q23= q23

  def setFromAverageInShellElement(self,element,fConv= 1.0):
    '''Extracts the average internal forces from the element.'''
    element.getResistingForce()
    mat= element.getPhysicalProperties.getVectorMaterials
    self.n1= mat.getMeanGeneralizedStressByName("n1")*fConv
    self.n2= mat.getMeanGeneralizedStressByName("n2")*fConv
    self.n12= mat.getMeanGeneralizedStressByName("n12")*fConv
    self.m1= mat.getMeanGeneralizedStressByName("m1")*fConv
    self.m2= mat.getMeanGeneralizedStressByName("m2")*fConv
    self.m12= mat.getMeanGeneralizedStressByName("m12")*fConv
    self.q13= mat.getMeanGeneralizedStressByName("q13")*fConv
    self.q23= mat.getMeanGeneralizedStressByName("q23")*fConv
    if(element.hasProp('theta')):
      theta= element.getProp('theta')
      self.transform(theta)
   
  def transform(self,theta):
    '''Calculates the components for a reference system rotated 
       the angle being passed as parameter.'''
    N= transformInternalForces([self.n1,self.n2,self.n12],theta)
    self.n1= N[0]; self.n2= N[1]; self.n12= N[2]
    M= transformInternalForces([self.m1,self.m2,self.m12],theta)
    self.m1= M[0]; self.m2= M[1]; self.m12= M[2]
    Q= transformInternalForces([self.q13,self.q23,0.0],theta)
    self.q13= Q[0]; self.q23= Q[1]

  def getCSVString(self):
    '''returns a comma separated values string that represents the
       internal forces.'''
    retval= str(self.n1)+", "+str(self.n2)+", "+str(self.n12)+", "
    retval+= str(self.m1)+", "+str(self.m2)+", "+str(self.m12)+", "
    retval+= str(self.q13)+", "+str(self.q23)
    return retval

  def setFromCSVString(self,csvStr,offset):
    '''Sets the internal forces from a CSV string.'''
    self.n1= eval(csvStr[0+offset])
    self.n2= eval(csvStr[1+offset])
    self.n12= eval(csvStr[2+offset])
    self.m1= eval(csvStr[3+offset])
    self.m2= eval(csvStr[4+offset])
    self.m12= eval(csvStr[5+offset])
    self.q13= eval(csvStr[6+offset])
    self.q23= eval(csvStr[7+offset])
    
  def getWoodArmer1(self):
    '''returns wood-archer method internal forces for axis 1.'''
    T= 0.0
    Mx= self.m1+ math.copysign(self.m12,self.m1)
    My= 0.0
    return csif.CrossSectionInternalForces(self.n1,self.q13,self.n12,T,Mx,My)

  def getWoodArmer2(self):
    '''returns wood-archer method internal forces for axis 2.'''
    T= 0.0
    Mx= self.m2+ math.copysign(self.m12,self.m2)
    My= 0.0
    return csif.CrossSectionInternalForces(self.n2,self.q23,self.n12,T,Mx,My)

  def getWoodArmer(self):
    '''returns wood-archer method internal forces.'''
    return [self.getWoodArmer1(),self.getWoodArmer2()]

    

