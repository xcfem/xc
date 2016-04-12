# -*- coding: utf-8 -*-

'''reinforcingSteel.py: reinforcing steel mechanical properties.'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import xc_base
import geom
from materials import typical_materials
from materials import materialGraphics as mg
import matplotlib.pyplot as plt

class ReinforcingSteel(object):
  """Reinforcing steel parameters 
  Attributes:
    nmbAcero: Name identifying the material.
    fyk:      Characteristic value of the yield strength.
    emax:     maximum strain in tension
    gammaS:   Partial factor for material.
    k:        fmaxk/fyk ratio (Annex C of EC2: class A k>=1,05 , class B k>=1,08)
  """
  nmbMaterial= "nil" # Name identifying the material.
  nmbDiagK= "dgK"+nmbMaterial # Name identifying the characteristic stress-strain diagram.
  matTagK= -1 # Tag of the uniaxial material with the characteristic stress-strain diagram.
  nmbDiagD= "dgD"+nmbMaterial # Name identifying the design stress-strain diagram.
  matTagD= -1 # Tag of the uniaxial material with the design stress-strain diagram .
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
    self.matTagK= -1 # Tag of the uniaxial material with the characteristic stress-strain diagram.
    self.steelDiagramK= None # Characteristic stress-strain diagram.
    self.nmbDiagD= "dgD"+nmbAcero # Name identifying the design stress-strain diagram.
    self.matTagD= -1 # Tag of the uniaxial material with the design stress-strain diagram .
    self.steelDiagramD= None # Design stress-strain diagram.
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
    self.steelDiagramK= typical_materials.defSteel01(preprocessor,self.nmbDiagK,self.Es,self.fyk,self.bsh())
    self.matTagK= self.steelDiagramK.tag
    return self.matTagK
  def defDiagD(self,preprocessor):
    """ Returns XC uniaxial material (design values). """
    self.steelDiagramD= typical_materials.defSteel01(preprocessor,self.nmbDiagD,self.Es,self.fyd(),self.bsh())
    self.matTagD= self.steelDiagramD.tag
    return self.matTagD
  def getDiagK(self,preprocessor):
    return preprocessor.getMaterialLoader.getMaterial(self.nmbDiagK)
  def getDiagD(self,preprocessor):
    return preprocessor.getMaterialLoader.getMaterial(self.nmbDiagD)
  def plotDesignStressStrainDiagram(self,preprocessor):
    self.defDiagD(preprocessor)
    retval= mg.UniaxialMaterialDiagramGraphic(-0.016,0.016, self.nmbMaterial + ' design stress-strain diagram')
    retval.setupGraphic(plt,self.steelDiagramD)
    fileName= self.nmbMaterial+'_design_stress_strain_diagram'
    retval.savefig(plt,fileName+'.jpeg')
    retval.savefig(plt,fileName+'.eps')
    return retval

# Characteristic stress-strain diagram
def defDiagKAcero(preprocessor, steelRecord):
  return steelRecord.defDiagK(preprocessor)

# Design stress-strain diagram
def defDiagDAcero(preprocessor, steelRecord):
  return steelRecord.defDiagD(preprocessor)

