# -*- coding: utf-8 -*-
from __future__ import division

def simpson(f, a, b, n):
  """Approximates the definite integral of f from a to b by
  the composite Simpson's rule, using n subintervals"""
  h= (b - a) / n
  s= f(a) + f(b)
 
  for i in range(1, n, 2):
    s+= 4 * f(a + i * h)
  for i in range(2, n-1, 2):
    s+= 2 * f(a + i * h)
 
  return s * h / 3

def getIntervals(abscissae):
  sz= len(abscissae)
  retval= []
  retval.append(abscissae[0])
  for i in range(1,sz):
    retval.append((abscissae[i-1]+abscissae[i])/2.0)
  retval.append(abscissae[-1])
  return retval

def getIntegSimpson(abscissae,func,n):
  intervals= getIntervals(abscissae)
  sz= len(intervals)
  retval= []
  for i in range(1,sz):
    s0= intervals[i-1]
    s1= intervals[i]
    retval.append(simpson(func,s0,s1,n))
  return retval

def getSimpsonWeights(abscissae,func,n):
  return getIntegSimpson(abscissae,func,n)
