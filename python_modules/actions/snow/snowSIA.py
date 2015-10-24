# -*- coding: utf-8 -*-
from __future__ import division
# Snow loads according to SIA-261 (2003).

import math

def Sk(h0):
  '''
  characteristic value of snow load.
  according SIA 261 2014 (5.2.6).
  h0: reference altitude (annex D)
  '''
  return max(((h0/350)**2+1)*400,900) #N/m

def Ce(windExposition):
  '''
  wind exposition coefficient for snow load.
  according SIA 261 2014 (5.2.4).
  '''
  retval= 1.0
  if(windExposition == "high"):
    retval= 0.8
  elif(windExposition == "low"):
    retval= 1.2
  elif(windExposition != "normal"):
    sys.stderr.write("wind exposition: '"+windExposition+"' unknown.\n")
  return retval

def qkRoof(mui,Ct,h0,windExposition):
  '''
  characteristic value of snow load on roofs.
  according SIA 261 2014 (5.2.2).
  mui: coefficient that represents shape of the roof (see 5.2.3).
  Ct: thermal coefficient (sell 5.2.5)
  '''
  retval= mui*Ce(windExposition)*Ct*Sk(h0)
  return retval


def qkPont(h0,windExposition):
  '''
  characteristic value of snow load on bridges (horizontal deck).
  according SIA 261 2014 (5.2.2).
  '''
  retval= 0.8*Ce(windExposition)*Sk(h0)
  return retval
  
