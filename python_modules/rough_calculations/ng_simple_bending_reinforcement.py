# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from miscUtils import LogMessages as lmsg




def neutralFiberDepth(M,fcd,b,d):
  c= 0.85*fcd*b
  T= c*(d-math.sqrt(d**2-2*M/c))
  xpl= T/c
  assert xpl<=d, "xpl bigger than section depth."
  return xpl

def Mu(As,fcd,fsd,b,d):
  T= As*fsd
  z= 0.9*d
  c= 0.85*fcd*b
  xpl= T/c
  assert xpl<=d, "xpl bigger than section depth."
  return T*z

def Mlim(fcd,b,d):
  return 0.33*fcd*b*d**2 

def AsSimpleBending(M,fcd,fsd,b,d):
  ''' Return the required reinforcement for a rectangular section
      subjected to simple bending.

  :param M: bending moment to resist.
  :param fcd: concrete design strength.
  :param b: section width.
  :param d: section depth.
  '''
  Ml= Mlim(fcd,b,d)
  if(M>Ml):
    lmsg.warning('compression reinforcement needed Ml= '+ str(Ml/1e3) + ' kN m < '+ str(M/1e3)+ ' kN m') 
    T= 1e9
  else:
    c= 0.85*fcd*b
    T= c*(d-math.sqrt(d**2-2*M/c))
    xpl= T/c
    assert xpl<=d, "xpl bigger than section depth."
  return T/fsd
