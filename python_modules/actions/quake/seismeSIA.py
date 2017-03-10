# -*- coding: utf-8 -*-
from __future__ import division
# Earthquake loads according to SIA-261 (2003).

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, A_OO   LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import math
import sys

def S(soilClass):
  '''
  parameter to obtain the elastic response spectrum.
  according SIA 261 2014 (16.2 tableau 24).
  '''
  retval= 1.4
  if(soilClass=='A'):
    retval= 1.0
  elif(soilClass== 'B'):
    retval= 1.2
  elif(soilClass== 'C'):
    retval= 1.15
  elif(soilClass== 'D'):
    retval= 1.35
  elif(soilClass== 'E'):
    retval= 1.40
  else:
    sys.stderr.write("soil class: '"+soilClass+"' unknown.\n")
  return retval

def Tb(soilClass):
  '''
  wave pertiod Tb to obtain the elastic response spectrum.
  according SIA 261 2003 (16.2 tableau 25).
  '''
  retval= 0.15
  if((soilClass=='A') or (soilClass== 'B')):
    retval= 0.15
  elif(soilClass== 'C'):
    retval= 0.20
  elif(soilClass== 'D'):
    retval= 0.20
  elif(soilClass== 'E'):
    retval= 0.15
  else:
    sys.stderr.write("soil class: '"+soilClass+"' unknown.\n")
  return retval

def Tc(soilClass):
  '''
  wave pertiod Tc to obtain the elastic response spectrum.
  according SIA 261 2003 (16.2 tableau 25).
  '''
  retval= 0.4
  if(soilClass== 'A'):
    retval= 0.4
  elif(soilClass== 'B'):
    retval= 0.5
  elif(soilClass== 'C'):
    retval= 0.6
  elif(soilClass== 'D'):
    retval= 0.8
  elif(soilClass== 'E'):
    retval= 0.5
  else:
    sys.stderr.write("soil class: '"+soilClass+"' unknown.\n")
  return retval

def Td(soilClass):
  '''
  wave pertiod Td to obtain the elastic response spectrum.
  according SIA 261 2003 (16.2 tableau 25).
  '''
  return 2.0

def eta(viscousDamping):
  '''
  correction coefficient "eta" to obtain the elastic response spectrum.
  according SIA 261 2003 (16.2.3.1).
  '''
  return math.max(mat.sqrt(1/(0.5+10*viscousDamping),0.55))

def elasticReponseSpectrum(soilClass,viscousDamping,accelTerrain,T):
  '''
  spectra de réponse élastica
  according SIA 261 2003 (16.2.3.1).
  '''
  s= S(soilClass)
  et= eta(viscousDamping)
  tb= Tb(soilClass)
  tc= Tc(soilClass)
  td= Td(soilClass)
  retval= accelTerrain*s
  if(T<=tb):
    retval*= (1+(2.5*et-1)*T/tb)
  elif(T<=tc):
    retval*= 2.5*et
  elif(T<=td):
    retval*= 2.5*et*tc/T
  else:
    retval*= 2.5*et*tc*td/(T**2)
  return retval

def gammaF(CO):
  '''
  importance factor for buildings and bridges
  according SIA 261 2003 (16.3.2 tableau 26).
  '''
  retval= 1.4
  if(CO==1):
    retval= 1.0
  elif(CO==2):
    retval= 1.2
  elif(CO==3):
    retval= 1.4
  else:
    sys.stderr.write("importance class: '"+str(CO)+"' unknown.\n")
  return retval

def designSpectrum(soilClass,accelTerrain,CO,T,q):
  '''
  design spectrum for elastic response analysis
  according SIA 261 2014 (16.2.4.1).
  '''
  s= S(soilClass)
  tb= Tb(soilClass)
  tc= Tc(soilClass)
  td= Td(soilClass)
  gf= gammaF(CO)
  retval= accelTerrain*s*gf
  factor= 10.0
  if(T<=tb):
    factor= (0.67+(2.5/q-0.67)*T/tb)
  elif(T<=tc):
    factor= 2.5/q
  elif(T<=td):
    factor= 2.5*tc/T/q
  else:
    factor= max(2.5*tc*td/(T**2)/q,0.1/s)
  retval*= factor
  print 'S= ', s, ' gf= ', gf, ' factor= ', factor, 'retval= ', retval
  return retval


def alpha(soilClass):
  '''
  alpha correction factor
  according SIA 261 2003 (16.4.4).
  '''
  retval= 2
  if((soilClass== 'B') or (soilClass== 'C')):
    retval= 1.5
  elif((soilClass== 'D') or (soilClass== 'D')):
    retval= 2
  elif(soilClass=='A'):
    retval= 1
  else:
    sys.stderr.write("soil class: '"+soilClass+"' unknown.\n")
  return retval

def grounDisplacement(soilClass,accelTerrain,CO):
  '''
  design ground displacement.
  according SIA 261 2003 (16.2.4.3).
  '''
  s= S(soilClass)
  tc= Tc(soilClass)
  td= Td(soilClass)
  gf= gammaF(CO)
  retval= 0.05*gf*accelTerrain*s*tc*td
  return retval

def bLowerLimitAppuiFixe(soilClass,accelTerrain,CO,a12,l):
  a= alpha(soilClass)
  u= grounDisplacement(soilClass,accelTerrain,CO)
  retval= 0.2+a12+a*l*u/1600
  return retval

def bUpperLimitAppuiFixe(soilClass,accelTerrain,CO,a12,l):
  a= alpha(soilClass)
  u= grounDisplacement(soilClass,accelTerrain,CO)
  retval= 0.2+a12+2*u
  return retval

def b1LowerLimitAppuiFixe(soilClass,accelTerrain,CO,a2,l):
  '''
  minimal dimension for b1 in bridge support
  according SIA 261 2003 (16.4.4 et Figure 15).
  '''
  return bLowerLimitAppuiFixe(soilClass,accelTerrain,CO,a2,l)

def b1UpperLimitAppuiFixe(soilClass,accelTerrain,CO,a2,l):
  '''
  maximal dimension for b1 in bridge support
  according SIA 261 2003 (16.4.4 et Figure 15).
  '''
  return bUpperLimitAppuiFixe(soilClass,accelTerrain,CO,a2,l)


def b2LowerLimitAppuiFixe(soilClass,accelTerrain,CO,a1,l):
  '''
  minimal dimension for b2 in bridge support
  according SIA 261 2003 (16.4.4 et Figure 15).
  '''
  return bLowerLimitAppuiFixe(soilClass,accelTerrain,CO,a1,l)

def b2UpperLimitAppuiFixe(soilClass,accelTerrain,CO,a1,l):
  '''
  maximal dimension for b2 in bridge support
  according SIA 261 2003 (16.4.4 et Figure 15).
  '''
  return bUpperLimitAppuiFixe(soilClass,accelTerrain,CO,a1,l)

def b2LowerLimitPontFlottant(soilClass,accelTerrain,CO,l):
  a= alpha(soilClass)
  u= grounDisplacement(soilClass,accelTerrain,CO)
  retval= 0.2+(1.3+a*l/1600)*u
  return retval

def b2UpperLimitPontFlottant(soilClass,accelTerrain,CO,l):
  a= alpha(soilClass)
  u= grounDisplacement(soilClass,accelTerrain,CO)
  retval= 0.2+a+3.3*u
  return retval
