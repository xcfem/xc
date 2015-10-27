# -*- coding: utf-8 -*-

import xc_base
import geom
from materials import typical_materials

class ReinforcingSteel(object):
  """Reinforcing steel parameters"""
  nmbMaterial= "nil" # Name identifying the material.
  nmbDiagK= "dgK"+nmbMaterial # Name identifying the characteristic stress-strain diagram.
  tagDiagK= -1 # Tag of the uniaxial material with the characteristic stress-strain diagram.
  nmbDiagD= "dgD"+nmbMaterial # Name identifying the design stress-strain diagram.
  tagDiagD= -1 # Tag of the uniaxial material with the design stress-strain diagram .
  fyk= 0.0 # Characteristic value of the yield strength.
  gammaS= 1.15 # Partial factor for material.
  Es= 2e11 # Elastic modulus of the material.
  emax= 0.0 # maximum strain in tension
#  fmaxk= 0.0 # Characteristic ultimate stress
  Esh= 0.0 # Slope of the line in the yielding region.
  bsh= 0.0 # Ratio between post-yield tangent and initial elastic tangent
  k=1.05   # fmaxk/fyk ratio (Annex C of EC2: class A k>=1,05 B , class B k>=1,08)
  def __init__(self,nmbAcero, fyk, emax, gammaS,k=1.05):
    self.nmbMaterial= nmbAcero # Name identifying the material.
    self.nmbDiagK= "dgK"+nmbAcero # Name identifying the characteristic stress-strain diagram.
    self.tagDiagK= -1 # Tag of the uniaxial material with the characteristic stress-strain diagram.
    self.nmbDiagD= "dgD"+nmbAcero # Name identifying the design stress-strain diagram.
    self.tagDiagD= -1 # Tag of the uniaxial material with the design stress-strain diagram .
    self.fyk= fyk # Characteristic value of the yield strength
    self.gammaS= gammaS
    self.emax= emax # Ultimate strain (rupture strain)
    self.k=k        # fmaxk/fyk ratio
 
  def fmaxk(self):
    """ Characteristic ultimate strength. """
    return self.k*self.fyk
  def fyd(self):
    """ Design yield stress. """
    return self.fyk/self.gammaS
  def eyk(self):
    """ Caracteristic strain at yield point. """
    return self.fyk/self.Es
  def eyd(self):
    """ Design strain at yield point. """
    return self.fyd()/self.Es
  def Esh(self):
    """ Slope of the curve in the yielding region. """
    return (self.fmaxk()-self.fyk)/(self.emax-self.eyk())
  def bsh(self):
    """ Ratio between post-yield tangent and initial elastic tangent. """
    return self.Esh()/self.Es
  def defDiagK(self,preprocessor):
    """ Returns XC uniaxial material (characteristic values). """
    acero= typical_materials.defSteel01(preprocessor,self.nmbDiagK,self.Es,self.fyk,self.bsh())
    self.tagDiagK= acero.tag
    return self.tagDiagK
  def defDiagD(self,preprocessor):
    """ Returns XC uniaxial material (design values). """
    acero= typical_materials.defSteel01(preprocessor,self.nmbDiagD,self.Es,self.fyd(),self.bsh())
    self.tagDiagD= acero.tag
    return self.tagDiagD
  def getDiagK(self,preprocessor):
    return preprocessor.getMaterialLoader.getMaterial(self.nmbDiagK)
  def getDiagD(self,preprocessor):
    return preprocessor.getMaterialLoader.getMaterial(self.nmbDiagD)


# Characteristic stress-strain diagram
def defDiagKAcero(preprocessor, steelRecord):
  return steelRecord.defDiagK(preprocessor)


# Design stress-strain diagram
def defDiagDAcero(preprocessor, steelRecord):
  return steelRecord.defDiagD(preprocessor)

