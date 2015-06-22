# -*- coding: utf-8 -*-

# Acero
'''
   nmbAcero: Nombre del acero.
   fyk: Resistencia característica a tracción (Pa).
   fyd: Resistencia de cálculo a tracción (Pa).
   emax: Deformación máxima
'''
import xc_base
import geom
from materials import typical_materials

class ReinforcingSteel(object):
  """Reinforcing steel parameters"""
  nmbMaterial= "nil" # Nombre del material.
  nmbDiagK= "dgK"+nmbMaterial # Nombre para el diagrama característico.
  tagDiagK= -1 # Tag del material uniaxial con el diagrama característico del material.
  nmbDiagD= "dgD"+nmbMaterial # Nombre para el diagrama de cálculo.
  tagDiagD= -1 # Tag del material uniaxial con el diagrama de cálculo del material.
  fyk= 0.0 # Valor característico del límite elástico.
  gammaS= 1.15 # Minoración del material.
  Es= 2e11 # Módulo elástico del material.
  emax= 0.0 # Deformación de rotura.
  fmaxk= 0.0 # Tensión máxima para diagrama característico.
  Esh= 0.0 # Pendiente del tramo de cedencia.
  bsh= 0.0 # Relación entre pendientes (rama cedencia/rama elástica)

  def __init__(self,nmbAcero, fyk, emax, gammaS):
    self.nmbMaterial= nmbAcero # Nombre del material.
    self.nmbDiagK= "dgK"+nmbAcero # Nombre para el diagrama característico.
    self.tagDiagK= -1 # Tag del material uniaxial con el diagrama característico del material.
    self.nmbDiagD= "dgD"+nmbAcero # Nombre para el diagrama de cálculo.
    self.tagDiagD= -1 # Tag del material uniaxial con el diagrama de cálculo del material.
    self.fyk= fyk # Valor característico del límite elástico.
    self.gammaS= gammaS
    self.emax= emax # Deformación de rotura.
    self.fmaxk= 1.05*fyk # Tensión máxima para diagrama característico.

  def fyd(self):
    """ Yield stress. """
    return self.fyk/self.gammaS
  def eyk(self):
    """ Deformación de cedencia para diagrama característico. """
    return self.fyk/self.Es
  def eyd(self):
    """ Deformación de cedencia para diagrama de cálculo. """
    return self.fyd()/self.Es
  def Esh(self):
    """ Pendiente del tramo de cedencia. """
    return (self.fmaxk-self.fyk)/(self.emax-self.eyk())
  def bsh(self):
    """ Relación entre pendientes (rama cedencia/rama elástica). """
    return self.Esh()/self.Es
  def defDiagK(self,mdlr):
    """ Returns XC uniaxial material (characteristic values). """
    acero= typical_materials.defSteel01(mdlr,self.nmbDiagK,self.Es,self.fyk,self.bsh())
    self.tagDiagK= acero.tag
    return self.tagDiagK
  def defDiagD(self,mdlr):
    """ Returns XC uniaxial material (design values). """
    acero= typical_materials.defSteel01(mdlr,self.nmbDiagD,self.Es,self.fyd(),self.bsh())
    self.tagDiagD= acero.tag
    return self.tagDiagD
  def getDiagK(self,mdlr):
    return mdlr.getMaterialLoader.getMaterial(self.nmbDiagK)
  def getDiagD(self,mdlr):
    return mdlr.getMaterialLoader.getMaterial(self.nmbDiagD)


# Diagrama tensión-deformación característico.
def defDiagKAcero(mdlr, steelRecord):
  return steelRecord.defDiagK(mdlr)


# Diagrama tensión-deformación de cálculo.
def defDiagDAcero(mdlr, steelRecord):
  return steelRecord.defDiagD(mdlr)

