# -*- coding: utf-8 -*-

'''reinforcingSteel.py: reinforcing steel mechanical properties.'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import xc_base
import geom
from materials import typical_materials
from materials import materialGraphics as mg
from materials import materialWithDKDiagrams as matWDKD
import matplotlib.pyplot as plt

class ReinforcingSteel(matWDKD.MaterialWithDKDiagrams):
  """Reinforcing steel parameters 
  Attributes:
    fyk:      Characteristic value of the yield strength.
    emax:     maximum strain in tension
    gammaS:   Partial factor for material.
    k:        fmaxk/fyk ratio (Annex C of EC2: class A k>=1,05 , class B k>=1,08)
  """
  Es= 2e11 # Elastic modulus of the material.
#  fmaxk= 0.0 # Characteristic ultimate stress
  Esh= 0.0 # Slope of the line in the yielding region.
  bsh= 0.0 # Ratio between post-yield tangent and initial elastic tangent
  k=1.05   # fmaxk/fyk ratio (Annex C of EC2: class A k>=1,05 B , class B k>=1,08)
  def __init__(self,nmbAcero, fyk, emax, gammaS,k=1.05):
    super(ReinforcingSteel,self).__init__(nmbAcero)
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
    self.materialDiagramK= typical_materials.defSteel01(preprocessor,self.nmbDiagK,self.Es,self.fyk,self.bsh())
    self.matTagK= self.materialDiagramK.tag
    return self.matTagK
  def defDiagD(self,preprocessor):
    """ Returns XC uniaxial material (design values). """
    self.materialDiagramD= typical_materials.defSteel01(preprocessor,self.nmbDiagD,self.Es,self.fyd(),self.bsh())
    self.matTagD= self.materialDiagramD.tag
    return self.matTagD
  def plotDesignStressStrainDiagram(self,preprocessor):
    '''Draws the steel design diagram.'''
    if self.materialDiagramD== None:
      self.defDiagD(preprocessor)
    retval= mg.UniaxialMaterialDiagramGraphic(-0.016,0.016, self.materialName + ' design stress-strain diagram')
    retval.setupGraphic(plt,self.materialDiagramD)
    fileName= self.materialName+'_design_stress_strain_diagram'
    retval.savefig(plt,fileName+'.jpeg')
    retval.savefig(plt,fileName+'.eps')
    return retval

# Characteristic stress-strain diagram
def defDiagKAcero(preprocessor, steelRecord):
  return steelRecord.defDiagK(preprocessor)

# Design stress-strain diagram
def defDiagDAcero(preprocessor, steelRecord):
  return steelRecord.defDiagD(preprocessor)

