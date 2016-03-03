# -*- coding: utf-8 -*-
''' Lateral torsional buckling of steel beams. '''
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from miscUtils import LogMessages as lmsg
import scipy.interpolate
import numpy

class MomentGradientFactorC1(object):
  ''' Calculation of the C1 moment gradient factor as defined
      in: A. López, D. J. Yong, M. A. Serna,
      Lateral-torsional buckling of steel beams: a general expression for
      the moment gradient factor.
      (Lisbon, Portugal: Stability and ductility of steel structures, 2006). '''
  def __init__(self,xi,Mi):
    self.xi= xi
    self.Mi= Mi
    self.momentDiagram= scipy.interpolate.interp1d(xi, Mi)
  
  def getL(self):
    ''' Returns the length of the moment diagram. '''
    return self.xi[-1]-self.xi[0]
  
  def getExtremeMoment(self):
    ''' Return the extreme of the bending moments (maximum or minimum). '''
    mMax= max(self.Mi)
    mMin= min(self.Mi)
    retval= mMax
    if(abs(retval)<abs(mMin)):
      retval= mMin
    return retval

  def getMi(self):
    ''' returns the five moment values that are needed for C1 calculation. '''
    nDiv= 4
    step= self.getL()/nDiv
    retval=list()
    xi= 0.0
    Mi= 0.0
    for i in range(0,nDiv+1):
      Mi= self.momentDiagram(xi)
      retval.append(float(Mi))
      xi+= step
    return retval

  def getAlphaI(self,k1,k2):
    ''' returns the five alpha values that are needed for C1 calculation. 
       k1: warping AND lateral bending coefficient at left end
                               k1= 1.0 => free warping AND lateral bending
                               k1= 0.5 => prevented warp. AND lateral bending
       k2: warping AND lateral bending coefficient at right end
                               k2= 1.0 => free warping AND lateral bending
                               k2= 0.5 => prevented warp. AND lateral bending'''
    return [(1.0-k2),5*k1**3/k2**2,5*(1.0/k1+1.0/k2),5*k2**3/k1**2,(1.0-k1)]

  def getA2(self):
    ''' return the value for the A2 coefficient. '''
    Mi= self.getMi()
    return (Mi[0]+2*Mi[1]+3*Mi[2]+2*Mi[3]+Mi[4])/(9*self.getExtremeMoment())

  def getA1(self,k1,k2):
    ''' return the value for the A1 coefficient. 
       k1: warping AND lateral bending coefficient at left end
                               k1= 1.0 => free warping AND lateral bending
                               k1= 0.5 => prevented warp. AND lateral bending
       k2: warping AND lateral bending coefficient at right end
                               k2= 1.0 => free warping AND lateral bending
                               k2= 0.5 => prevented warp. AND lateral bending'''
    Mi= self.getMi()
    ai= self.getAlphaI(k1,k2)
    Mmax2= self.getExtremeMoment()**2
    return (Mmax2+ai[0]*Mi[0]**2+ai[1]*Mi[1]**2+ai[2]*Mi[2]**2+ai[3]*Mi[3]**2+ai[4]*Mi[4]**2)/((1+ai[0]+ai[1]+ai[2]+ai[3]+ai[4])*Mmax2)

  def getC1(self,k1,k2):
    ''' return the value for the C1 coefficient. 
       k1: warping AND lateral bending coefficient at left end
                               k1= 1.0 => free warping AND lateral bending
                               k1= 0.5 => prevented warp. AND lateral bending
       k2: warping AND lateral bending coefficient at right end
                               k2= 1.0 => free warping AND lateral bending
                               k2= 0.5 => prevented warp. AND lateral bending'''
    k= math.sqrt(k1*k2)
    rootK= math.sqrt(k)
    A1= self.getA1(k1,k2)
    A2= self.getA2()
    B1= rootK*A1+((1-rootK)/2.0*A2)**2
    return (math.sqrt(B1)+(1-rootK)/2.0*A2)/A1

  
