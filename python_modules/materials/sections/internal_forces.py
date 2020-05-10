# -*- coding: utf-8 -*-
'''Material internal forces (generalized stresses).'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from misc_utils import log_messages as lmsg

class CrossSectionInternalForces:
  def __init__(self,N= 0.0,Vy= 0.0,Vz= 0.0,T= 0.0,My= 0.0,Mz= 0.0):
    '''Internal forces on a 3D section (6 degrees of freedom).

    :ivar N: axial force.
    :ivar Vy: shear force parallel to axis y.
    :ivar Vz: shear force parallel to axis z.
    :ivar T: Torque.
    :ivar My: bending moment parallel to axis y.
    :ivar Mz: bending moment parallel to axis z.
    '''
    self.N= N
    self.Vy= Vy
    self.Vz= Vz
    self.T= T
    self.My= My
    self.Mz= Mz

  def __iadd__(self,other):
    '''+= operator.'''
    self.N+= other.N
    self.Vy+= other.Vy
    self.Vz+= other.Vz
    self.T+= other.T
    self.My+= other.My
    self.Mz+= other.Mz
    return self

  def __add__(self,other):
    '''+ operator.'''
    return CrossSectionInternalForces(self.N+other.N,self.Vy+other.Vy,self.Vz+other.Vz,self.T+other.T,self.My+other.My,self.Mz+other.Mz)

  def __isub__(self,other):
    '''-= operator.'''
    self.N-= other.N
    self.Vy-= other.Vy
    self.Vz-= other.Vz
    self.T-= other.T
    self.My-= other.My
    self.Mz-= other.Mz
    return self

  def __sub__(self,other):
    '''- operator.'''
    return CrossSectionInternalForces(self.N-other.N,self.Vy-other.Vy,self.Vz-other.Vz,self.T-other.T,self.My-other.My,self.Mz-other.Mz)

  def __imul__(self,number):
    '''*= operator.'''
    self.N*= number
    self.Vy*= number
    self.Vz*= number
    self.T*= number
    self.My*= number
    self.Mz*= number
    return self

  def __mul__(self,number):
    '''* operator.'''
    return CrossSectionInternalForces(self.N*number,self.Vy*number,self.Vz*number,self.T*number,self.My*number,self.Mz*number)

  def getModulus(self):
    '''modulus of the corresponding [N,Vy,Vz,T,My,Mz] vector.'''
    retval= self.N*self.N
    retval+= self.Vy*self.Vy
    retval+= self.Vz*self.Vz
    retval+= self.T*self.T
    retval+= self.My*self.My
    retval+= self.Mz*self.Mz
    return retval
  
  def getDict(self):
    '''returns a dictionary whith the values of the internal forces.'''
    retval= dict()
    retval['N']= self.N; retval['Vy']= self.Vy; retval['Vz']= self.Vz
    retval['T']= self.T; retval['My']= self.My; retval['Mz']= self.Mz
    return retval
  
  def setFromDict(self,dct):
    '''Sets the internal forces from the dictionary argument.'''
    self.N= dct['N']
    self.Vy= dct['Vy']
    self.Vz= dct['Vz']
    self.T= dct['T']
    self.My= dct['My']
    self.Mz= dct['Mz']
    if('chiLT' in dct.keys()):
        self.chiLT= dct['chiLT']
    if('chiN' in dct.keys()):
        self.chiN= dct['chiN']
      
  def getCSVString(self):
    '''returns a comma separated values string that represents the
       internal forces.'''
    lmsg.log('DEPRECATED')
    retval= str(self.N)+", "+str(self.Vy)+", "+str(self.Vz)+", "
    retval+= str(self.T)+", "+str(self.My)+", "+str(self.Mz)
    return retval

  def setFromCSVString(self,csvStr,offset):
    '''Sets the internal forces from a CSV string.'''
    #lmsg.log('DEPRECATED')
    self.N= eval(csvStr[0+offset])
    self.Vy= eval(csvStr[1+offset])
    self.Vz= eval(csvStr[2+offset])
    self.T= eval(csvStr[3+offset])
    self.My= eval(csvStr[4+offset])
    self.Mz= eval(csvStr[5+offset])
    if len(csvStr) > (6+offset):   #steel beam
      self.chiLT=eval(csvStr[6+offset])

  def getComponents(self):
    '''Returns the internal forces in a list.'''
    return [self.N,self.Vy,self.Vz,self.T,self.My,self.Mz]

  def __str__(self):
    return str(self.getDict())

def transformInternalForces(iForces,theta):
  '''Computes internal forces in a system rotated theta
     degrees with respect to the z(3) axis.'''
  cos2T= math.cos(2*theta)
  sin2T= math.sin(2*theta)
  tmpA= (iForces[0]+iForces[1])/2.0
  tmpB= (iForces[0]-iForces[1])/2.0*cos2T+iForces[2]*sin2T
  retval= [0.0,0.0,0.0]
  retval[0]= tmpA+tmpB 
  retval[2]= -(iForces[0]-iForces[1])/2.0*sin2T+iForces[2]*cos2T
  retval[1]= tmpA-tmpB
  return retval

class ShellMaterialInternalForces:
  def __init__(self,n1= 0.0,n2= 0.0,n12= 0.0,m1= 0.0,m2= 0.0,m12= 0.0,q13= 0.0,q23= 0.0):
    '''Internal forces on a material point of a shell element

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
      physProp= element.getPhysicalProperties
      self.n1= physProp.getMeanGeneralizedStressByName("n1")*fConv
      self.n2= physProp.getMeanGeneralizedStressByName("n2")*fConv
      self.n12= physProp.getMeanGeneralizedStressByName("n12")*fConv
      self.m1= physProp.getMeanGeneralizedStressByName("m1")*fConv
      self.m2= physProp.getMeanGeneralizedStressByName("m2")*fConv
      self.m12= physProp.getMeanGeneralizedStressByName("m12")*fConv
      self.q13= physProp.getMeanGeneralizedStressByName("q13")*fConv
      self.q23= physProp.getMeanGeneralizedStressByName("q23")*fConv
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

  def getDict(self):
    '''returns a dictionary whith the values of the internal forces.'''
    retval= dict()
    retval['n1']= self.n1; retval['n2']= self.n2; retval['n12']= self.n12
    retval['m1']= self.m1; retval['m2']= self.m2; retval['m12']= self.m12
    retval['q13']= self.q13; retval['q23']= self.q23
    return retval
  
  def setFromDict(self,dct):
    '''Sets the internal forces from the dictionary argument.'''
    self.n1= dct['n1']
    self.n2= dct['n2']
    self.n12= dct['n12']
    self.m1= dct['m1']
    self.m2= dct['m2']
    self.m12= dct['m12']
    self.q13= dct['q13']
    self.q23= dct['q23']

    
  def getCSVString(self):
    '''returns a comma separated values string that represents the
       internal forces.'''
    lmsg.log('DEPRECATED')
    retval= str(self.n1)+", "+str(self.n2)+", "+str(self.n12)+", "
    retval+= str(self.m1)+", "+str(self.m2)+", "+str(self.m12)+", "
    retval+= str(self.q13)+", "+str(self.q23)
    return retval

  def setFromCSVString(self,csvStr,offset):
    '''Sets the internal forces from a CSV string.'''
    #lmsg.log('DEPRECATED')
    self.n1= eval(csvStr[0+offset])
    self.n2= eval(csvStr[1+offset])
    self.n12= eval(csvStr[2+offset])
    self.m1= eval(csvStr[3+offset])
    self.m2= eval(csvStr[4+offset])
    self.m12= eval(csvStr[5+offset])
    self.q13= eval(csvStr[6+offset])
    self.q23= eval(csvStr[7+offset])

  # Wood-Armer method for the assessment of reinforced concrete shells.
  def getWoodArmer1(self):
    '''returns wood-archer method internal forces for axis 1.''' 
    T= 0.0
    Mx= self.m1+ math.copysign(self.m12,self.m1)
    My= 0.0
    return CrossSectionInternalForces(self.n1,self.q13,self.n12,T,Mx,My)

  def getWoodArmer2(self):
    '''returns wood-archer method internal forces for axis 2.'''
    T= 0.0
    Mx= self.m2+ math.copysign(self.m12,self.m2)
    My= 0.0
    return CrossSectionInternalForces(self.n2,self.q23,self.n12,T,Mx,My)

  def getWoodArmer(self):
    '''returns wood-archer method internal forces.'''
    return [self.getWoodArmer1(),self.getWoodArmer2()]
