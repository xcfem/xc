# -*- coding: utf-8 -*-

import math
from materials import reinforcingSteel

# Stress-strain diagram of reinforcing steel, according to EC2 
# (the same one is adopted by EHE and SIA).
def sigmas(eps, fy, ey, Es, Esh):
  if(eps>0):
    if(eps<ey):
      return (Es*eps)
    else:
      return (fy+(eps-ey)*Esh)
  else:
    if(eps>-(ey)):
      return (Es*eps)
    else:
      return (-fy+(eps-ey)*Esh) 


# Characteristic stress-strain diagram for reinforcing steel, according to EC2.
def sigmaKAceroArmar(eps,matRecord):
  return sigmas(eps,matRecord.fyk,matRecord.eyk(),matRecord.Es,matRecord.Esh())

# Design stress-strain diagram for reinforcing steel, according to EC2.
def sigmaDAceroArmar(eps,matRecord):
  return sigmas(eps,matRecord.fyd(),matRecord.eyd(),matRecord.Es,matRecord.Esh())

# Checking of characteristic stress-strain diagram
def testDiagKAceroArmar(preprocessor, matRecord):
  tag= reinforcingSteel.defDiagKAcero(preprocessor, matRecord)
  diagAcero= preprocessor.getMaterialLoader.getMaterial(matRecord.nmbDiagK)
  incr= matRecord.emax/20
  errMax= 0.0
  e= 0.1e-8
  while(e < matRecord.emax+1):
    diagAcero.setTrialStrain(e,0.0)
    diagAcero.commitState()
    sg= sigmaKAceroArmar(e,matRecord)
    stress= diagAcero.getStress()
    err= abs((sg-stress)/sg)
    #print "e= ",e," strain= ",diagAcero.getStrain()," stress= ",stress," sg= ", sg," err= ", err,"\n"
    errMax= max(err,errMax)
    e= e+incr
  return errMax

# Checking of design stress-strain diagram
def testDiagDAceroArmar(preprocessor, matRecord):
  tag= reinforcingSteel.defDiagDAcero(preprocessor, matRecord)
  diagAcero= preprocessor.getMaterialLoader.getMaterial(matRecord.nmbDiagD)
  incr= matRecord.emax/20
  errMax= 0.0
  e= 0.1e-8
  while(e < matRecord.emax+1):
    diagAcero.setTrialStrain(e,0.0)
    diagAcero.commitState()
    sg= sigmaDAceroArmar(e,matRecord)
    err= abs((sg-diagAcero.getStress())/sg)
# print("e= ",(e)," stress= ",stress," sg= ", (sg)," err= ", (err),"\n")
    errMax= max(err,errMax)
    e= e+incr
  return errMax
