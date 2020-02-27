# -*- coding: utf-8 -*-
from __future__ import division
# Wind according to SIA 261

import math

def Zg(catTerrain):
  ''' value for Zg (expression 12 of SIA 261 6.2.1.2) as defined in table 4'''
  retval= 300
  if(catTerrain=="II"):
    retval= 300
  elif(catTerrain=="IIa"):
    retval= 380
  elif(catTerrain=="III"):
    retval= 450
  elif(catTerrain=="IV"):
    retval= 526
  else:
    sys.stderr.write("Terrain category: '"+ catTerrain +"' unknown.\n")
  return retval    

def alphaR(catTerrain):
  ''' value for alpha_r (expression 12 of SIA 261 6.2.1.2) as defined in table 4'''
  retval= 0.16
  if(catTerrain=="II"):
    retval= 0.16
  elif(catTerrain=="IIa"):
    retval= 0.19
  elif(catTerrain=="III"):
    retval= 0.23
  elif(catTerrain=="IV"):
    retval= 0.30
  else:
    sys.stderr.write("Terrain category: '"+ catTerrain +"' unknown.\n")
  return retval

def Ch(z,catTerrain):
  ''' factor c_h as defined in expression 12 of SIA 261 6.2.1.2'''
  ar= alphaR(catTerrain)
  zg= Zg(catTerrain)
  return 1.6*(math.pow(z/zg,ar)+0.375)**2

def qp(qp0,z,catTerrain):
  '''Dynamic wind pressure as defined in expression 11 of SIA 261 6.2.1.2'''
  zcorr= max(z,5.0)
  if(catTerrain=='IV'):
    zcorr= max(z,10.0)
  return Ch(zcorr,catTerrain)*qp0
