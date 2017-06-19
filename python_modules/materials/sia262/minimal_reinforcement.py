# -*- coding: utf-8 -*-
# SIA 262 section 4.4.2

from materials.sia262 import SIA262_materials

def AsMinContrainteLimiteeTraction(concrete,sgAdm,t):
  fctm= concrete.fctm()
  kt= SIA262_materials.reductionFactorKT(t)
  return kt*fctm*t/sgAdm

def AsMinTraction(concrete,exigence,ecartement,t):
  fctm= concrete.fctm()
  kt= SIA262_materials.reductionFactorKT(t)
  sgAdm= SIA262_materials.limitationContraintes(exigence,ecartement)
  return kt*fctm*t/sgAdm

def AsMinFlexion(concrete,concreteCover,exigence,ecartement,t):
  fctd= concrete.fctm()*SIA262_materials.reductionFactorKT(t/3)
  z= 0.9*(t-concreteCover)
  w= 1/6.0*t**2
  sgAdm= SIA262_materials.limitationContraintes(exigence,ecartement)
  return fctd*w/sgAdm/z

