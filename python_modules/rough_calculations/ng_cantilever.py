# -*- coding: utf-8 -*-
# Cantilerver formulas

from __future__ import division
import math
import ng_beam as bm

#
#  |----> x
#
#  |--------------------
#

#Sign convention: see ng_simple_beam.py

class Cantilever(bm.Beam):

  def getShearUnderUniformLoad(self,q,x):
    return -q*(self.l-x)
  def getReactionUnderUniformLoad(self,q):
    return self.getShearUnderUniformLoad(self,q,0.0)
  def getBendingMomentUnderUniformLoad(self,q,x):
    return q/2.0*(self.l-x)**2
# WRITE CODE PENDING.
#   def getDeflectionUnderUniformLoad(self,q,x):
#     return q*x/(24.0*self.E*self.I)*(self.l**3-2*self.l*x**2+x**3)

# #          l
# # |<---------------->|
# #    a      b     c
# # |<-->|<------>|<--->|
# #      ||||||||||
# #  ___________________
# # ^                   ^
#   def getReactionUnderUniformLoadPartiallyDistributed(self,q,a,b):
#     c= self.l-a-b
#     retval= q*b/2.0/self.l*(2*c+b)
#     return -retval

#   def getShearUnderUniformLoadPartiallyDistributed(self,q,a,b,x):
#     R1= getReaction1UnderUniformLoadPartiallyDistributed(self,q,a,b)
#     retval= R1
#     if(x>a):
#       if(x<(a+b)):
#         retval-= q*(x-a)
#       else:
#         retval= getReaction2UnderUniformLoadPartiallyDistributed(self,q,a,b)
#     return retval

#   def getBendingMomentUnderUniformLoadPartiallyDistributed(self,q,a,b,x):
#     R1= getReaction1UnderUniformLoadPartiallyDistributed(self,q,a,b)
#     retval= R1*x
#     if(x>a):
#       if(x<(a+b)):
#         retval-= q/2.0*(x-a)**2
#       else:
#         retval= getReaction2UnderUniformLoadPartiallyDistributed(self,q,a,b)*(self.l-x)
#     return retval

#          l
# |<---------------->|
#    a         b
# |<--->|<---------->|
#       |
#  _____|______________
# ^                   ^
  def getReactionUnderConcentratedLoad(self,P,a):
    return -P

  def getShearUnderConcentratedLoad(self,P,a,x):
    retval= self.getReactionUnderConcentratedLoad(P,a)
    if(x>a):
      retval= 0.0
    return retval

  def getBendingMomentUnderConcentratedLoad(self,P,a,x):
    R1= self.getReactionUnderConcentratedLoad(P,a)
    retval= R1*(x-a)
    if(x>a):
      retval= 0.0
    return retval

  def getDeflectionUnderConcentratedLoad(self,P,a,x):
    b= self.l-a
    retval= P*x**2/(6.0*self.EI())*(3*a-x)
    if(x>a):
      retval= P*a**2/(6.0*self.EI())*(3*x-a)
    return retval
