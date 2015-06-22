# -*- coding: utf-8 -*-
# Concrete according to EHE.


# Hormigón

import xc_base
import geom
from materials import typical_materials
from materials.ehe import auxEHE

class HormigonEHE(object):
  """ Concrete model according to EHE"""
  nmbMaterial= "nil" # Nombre del material.
  nmbDiagK= "dgK"+nmbMaterial # Nombre para el diagrama característico.
  tagDiagK= -1 # Tag del material uniaxial con el diagrama característico del material.
  nmbDiagD= "dgD"+nmbMaterial # Nombre para el diagrama de cálculo.
  tagDiagD= -1 # Tag del material uniaxial con el diagrama de cálculo del material.
  fck= 0 # Resistencia característica a compresión (Pa).
  gmmC= 1.5 # Minoración de la resistencia del hormigón.
  epsilon0= -2e-3 # Deformación para tensión máxima (de compresión).
  epsilonU= -3.5e-3 # Deformación para tensión última (de compresión).
  nuc= 0.2 # Coeficiente de Poisson del hormigón.

  # Define las propiedades "derivadas" del material.
  def __init__(self,nmbHormigon, fck, gammaC):
    self.setupName(nmbHormigon)
    self.fck= fck
    self.gmmC= gammaC # Minoración de la resistencia del hormigón.

  def setupName(self,nmbHormigon):
    self.nmbMaterial= nmbHormigon # Nombre del material.
    self.nmbDiagK= "dgK"+self.nmbMaterial # Nombre para el diagrama característico.
    self.tagDiagK= -1 # Tag del material uniaxial con el diagrama característico del material.
    self.nmbDiagD= "dgD"+self.nmbMaterial # Nombre para el diagrama de cálculo.
    self.tagDiagD= -1 # Tag del material uniaxial con el diagrama de cálculo del material.

  def fcd(self):
    return self.fck/self.gmmC
  def fmaxK(self):
    return 0.85*self.fck
  def fmaxD(self):
    return 0.85*self.fcd()
  def fctm(self):
    """Resistencia media a tracción (en Pa)."""
    return auxEHE.fctMedEHE08(self.fck) 
  def fctk(self):
    """Resistencia característica de cálculo a tracción (en Pa)."""
    return 0.7*self.fctm()
  def fctd(self):
    """Resistencia media de cálculo a tracción (en Pa)."""
    return self.fctk()/self.gmmC 
  def Ecm(self):
    """Modulo de deformación longitudinal secante a 28 días."""
    return auxEHE.getEcm(self.fck) 
  def Gcm(self):
    """# Modulo de deformación transversal a 28 días."""
    return self.Ecm()/(2*(1+self.nuc)) 
  def tangDHormigon(self,eps):
    """Diagrama característico tensión deformación del hormigón según EHE."""
    return tangc(eps,self.fcd())
  def defDiagK(self,mdlr):
    hormigon= typical_materials.defConcrete01(mdlr,self.nmbDiagK,self.epsilon0,self.fmaxK(),self.fmaxK(),self.epsilonU)
    self.tagDiagK= hormigon.tag
    return self.tagDiagK
  def defDiagD(self,mdlr):
    hormigon= typical_materials.defConcrete01(mdlr,self.nmbDiagD,self.epsilon0,self.fmaxD(),self.fmaxD(),self.epsilonU)
    self.tagDiagD= hormigon.tag
    return self.tagDiagD
  def getDiagD(self,mdlr):
    return mdlr.getMaterialLoader.getMaterial(self.nmbDiagD)
  def getDiagK(self,mdlr):
    return mdlr.getMaterialLoader.getMaterial(self.nmbDiagK)



def defDiagKHormigon(mdkr, concreteRecord):
  return concreteRecord.defDiagK(mdlr);

def defDiagDHormigon(mdlr, concreteRecord):
  return concreteRecord.defDiagD(mdlr)

# Diagrama tensión deformación del hormigón según EHE.
def sigmac(eps,fcd):
  if(eps<0):
    if(eps>-2e-3):
      return (-850*fcd*eps*(250*eps+1))
    else:
      return (0.85*fcd) 
  else:
    return 0.0 

# Módulo de deformación tangente del hormigón según EHE.
def tangc(eps,fcd):
  if(eps<0):
    if(eps>-2e-3):
      return (-850*fcd*(500*eps+1)) 
    else:
      return 0.0
  else:
    return 0.0 
 
# Diagrama característico tensión deformación del hormigón según EHE.
def sigmaKHormigon(eps,mat):
  return sigmac(eps,mat.fck)

# Diagrama característico tensión deformación del hormigón según EHE.
def sigmaDHormigon(eps,mat):
  return sigmac(eps,mat.fcd())


# Comprueba la corrección de los resultados del diagrama de cálculo del modelo de elementos finitos.
def testDiagDHormigon(mdlr, concreteRecord):
  tag= defDiagDHormigon(mdlr, concreteRecord)
  diagHormigon= mdlr.getMaterialLoader.getMaterial(concreteRecord.nmbDiagD)
  incr= concreteRecord.epsilonU/20
  errMax= 0.0
  e=  -0.1e-8
  while(e>=concreteRecord.epsilonU):
    diagHormigon.setTrialStrain(e,0.0)
    diagHormigon.commitState()
    sg= sigmaDHormigon(e,concreteRecord)
    stress= diagHormigon.getStress()
    err= abs((sg-stress)/sg)
    errMax= max(err,errMax)
    e=(e+incr)
  return errMax

# Comprueba la corrección de los resultados de la tangente al diagrama de cálculo del modelo de elementos finitos.
def testTangDHormigon(mdlr, concreteRecord):
  tag= defDiagDHormigon(mdlr, concreteRecord)
  diagHormigon= mdlr.getMaterialLoader.getMaterial(concreteRecord.nmbDiagD)
  incr= concreteRecord.epsilonU/20
  errMax= 0.0
  e=  -0.1e-8
  while(e>=concreteRecord.epsilonU):
    diagHormigon.setTrialStrain(e,0.0)
    diagHormigon.commitState()
    tg= concreteRecord.tangDHormigon(e)
    tgDiag= diagHormigon.getTangent()
    if(tg!=0.0):
      err= abs((tg-tgDiag)/tg)
    else:
      err= abs(tg-tgDiag)
    errMax= max(err,errMax)
    e=(e+incr)
  return errMax


HA25= HormigonEHE("HA25",-25e6,1.5)
HA30= HormigonEHE("HA30",-30e6,1.5)
HA35= HormigonEHE("HA35",-35e6,1.5)
HA40= HormigonEHE("HA40",-40e6,1.5)
HA45= HormigonEHE("HA45",-45e6,1.5)
HA50= HormigonEHE("HA50",-50e6,1.5)

HP45= HormigonEHE("HP45",-45e6,1.5)
HP50= HormigonEHE("HP50",-50e6,1.5)

