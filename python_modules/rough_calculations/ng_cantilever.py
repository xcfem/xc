# -*- coding: utf-8 -*-
# Cantilerver formulas

from __future__ import division
import math
from rough_calculations import ng_beam as bm
from misc_utils import log_messages as lmsg

#
#  |----> x
#
#  |--------------------
#

#Sign convention: see ng_simple_beam.py

class Cantilever(bm.Beam):
    ''' Cantilever formulas.'''
    
    def getShearUnderUniformLoad(self,q,x):
        ''' Return the shear value at x.

        :param q: value of the uniform load.
        :param x: position of the section.
        '''
        return -q*(self.l-x)
      
    def getReactionUnderUniformLoad(self,q):
        ''' Return the reaction under uniform load.

        :param q: value of the uniform load.
        '''
        return self.getShearUnderUniformLoad(self,q,0.0)
      
    def getBendingMomentUnderUniformLoad(self,q,x):
        ''' Return the bending moment under uniform load.

        :param q: value of the uniform load.
        :param x: position of the section.
        '''
        return q/2.0*(self.l-x)**2
      
    def getDeflectionUnderUniformLoad(self,q,x):
        ''' Return the deflection under uniform load.

        :param q: value of the uniform load.
        :param x: position of the section.
        '''
        return q*x**2/(24.0*self.EI())*(x**2+6*self.l**2-4*self.l*x)
      
    #          l
    # |<---------------->|
    #    a         b
    # |<--->|<---------->|
    #       |
    #  _____|______________
    # ^                   ^
    def getReactionUnderConcentratedLoad(self,P):
        ''' Return the reaction under concentrated load.

        :param P: value of the concentrated load.
        '''
        return -P

    def getShearUnderConcentratedLoad(self,P,a,x):
        ''' Return the shear under concentrated load.

        :param P: value of the concentrated load.
        :param a: position of the load.
        :param x: position of the section.
        '''
        retval= 0.0
        if(x<a):
          retval= self.getReactionUnderConcentratedLoad(P)
        return retval

    def getBendingMomentUnderConcentratedLoad(self,P,a,x):
        ''' Return the bending moment under concentrated load.

        :param P: value of the concentrated load.
        :param a: position of the load.
        :param x: position of the section.
        '''
        retval= 0.0
        if(x<a):
            R1= self.getReactionUnderConcentratedLoad(P)
            retval= R1*(x-a)
        return retval

    def getDeflectionUnderConcentratedLoad(self,P,a,x):
        ''' Return the deflection under concentrated load.

        :param P: value of the concentrated load.
        :param a: position of the load.
        :param x: position of the section.
        '''
        b= self.l-a
        retval= P*x**2/(6.0*self.EI())*(3*a-x)
        if(x>a):
          retval= P*a**2/(6.0*self.EI())*(3*x-a)
        return retval
    
    def getFundamentalBendingFrequencies(self, numModes, linearRho):
        ''' Return the fundamental bending frequencies of the beam.

        :param numModes: number of oscillation modes to compute.
        :param linearRho: mass density per unit length.
        '''
        tmp= 1/2.0/math.pi*math.sqrt(self.E*self.I/linearRho)
        retval= list()
        if(numModes>=1):
            f1= tmp*3.5156/self.l**2
            retval.append(f1)
        if(numModes>=2):
            f2= 6.268*f1
            retval.append(f2)
        if(numModes>=3):
            f3= 17.456*f1
            retval.append(f3)
        if(numModes>3):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+': computation for modes greater than 4 not implemented.')
        return retval
      
# WRITE CODE PENDING.

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

