# -*- coding: utf-8 -*-
# SIA 262 section 4.4.2

from materials.sia262 import concreteSIA262
from materials.sia262 import steelSIA262

def AsMinContrainteLimiteeTraction(concrete,sgAdm,t):
  fctm= concrete.fctm()
  kt= concreteSIA262.reductionFactorKT(t)
  return kt*fctm*t/sgAdm

def AsMinTraction(concrete,exigence,ecartement,t):
  fctm= concrete.fctm()
  kt= concreteSIA262.reductionFactorKT(t)
  sgAdm= steelSIA262.limitationContraintes(exigence,ecartement)
  return kt*fctm*t/sgAdm

def AsMinFlexion(concrete,enrobage,exigence,ecartement,t):
  fctd= concrete.fctm()*concreteSIA262.reductionFactorKT(t/3)
  z= 0.9*(t-enrobage)
  w= 1/6.0*t**2
  sgAdm= steelSIA262.limitationContraintes(exigence,ecartement)
  return fctd*w/sgAdm/z

