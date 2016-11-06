# -*- coding: utf-8 -*-
# Shell element internal forces.
# Wood-Armer method for the assessment of reinforced concrete shells.

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

class CrossSectionInternalForces:
  def __init__(self,N= 0.0,Vy= 0.0,Vz= 0.0,T= 0.0,My= 0.0,Mz= 0.0):
    '''Internal forces on the shell element

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
  
  def getCSVString(self):
    '''returns a comma separated values string that represents the
       internal forces.'''
    retval= str(self.N)+", "+str(self.Vy)+", "+str(self.Vz)+", "
    retval+= str(self.T)+", "+str(self.My)+", "+str(self.Mz)
    return retval

  def setFromCSVString(self,csvStr,offset):
    '''Sets the internal forces from a CSV string.'''
    self.N= eval(csvStr[0+offset])
    self.Vy= eval(csvStr[1+offset])
    self.Vz= eval(csvStr[2+offset])
    self.T= eval(csvStr[3+offset])
    self.My= eval(csvStr[4+offset])
    self.Mz= eval(csvStr[5+offset])

  def getComponents(self):
    '''Returns the internal forces in a list.'''
    return [self.N,self.Vy,self.Vz,self.T,self.My,self.Mz]

  def __str__(self):
    return self.getCSVString()

