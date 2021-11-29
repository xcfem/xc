# -*- coding: utf-8 -*-
''' Formulas for fixed-fixed beams.'''

from __future__ import print_function

from __future__ import division
import math
from rough_calculations import ng_beam as bm
from misc_utils import log_messages as lmsg

#
# | ___________________|
# |                    |

class FixedFixedBeam(bm.Beam):

    def getShearUnderUniformLoad(self,q,x):
        print('not implemented')
        return None
    def getReactionUnderUniformLoad(self,q):
        print('not implemented')
        return None
    def getBendingMomentUnderUniformLoad(self,q,x):
        print('not implemented')
        return None
    def getDeflectionUnderUniformLoad(self,q,x):
        print('not implemented')
        return None

#          l
# |<---------------->|
#    a      b     c
# |<-->|<------>|<--->|
#      ||||||||||
#  ___________________|
# ^                   |
    def getReaction1UnderUniformLoadPartiallyDistributed(self,q,a,b):
        print('not implemented')
        return None

    def getReaction2UnderUniformLoadPartiallyDistributed(self,q,a,b):
        print('not implemented')
        return None

    def getShearUnderUniformLoadPartiallyDistributed(self,q,a,b,x):
        print('not implemented')
        return None

    def getBendingMomentUnderUniformLoadPartiallyDistributed(self,q,a,b,x):
        print('not implemented')
        return None

#          l
# |<---------------->|
#    a         b
# |<--->|<---------->|
#       |
#  _____|_____________|
# ^                   |
    def getReaction1UnderConcentratedLoad(self,P,a):
        print('not implemented')
        return None

    def getReaction2UnderConcentratedLoad(self,P,a):
        print('not implemented')
        return None

    def getShearUnderConcentratedLoad(self,P,a,x):
        print('not implemented')
        return None

    def getBendingMomentUnderConcentratedLoad(self,P,a,x):
        print('not implemented')
        return None

    def getDeflectionUnderConcentratedLoad(self,P,a,x):
        print('not implemented')
        return None

    def getFundamentalBendingFrequencies(self, numModes, linearRho):
        ''' Return the fundamental bending frequencies of the beam.

        :param numModes: number of oscillation modes to compute.
        :param linearRho: mass density per unit length.
        '''
        f1= 1/2.0/math.pi*22.373/self.l**2*math.sqrt(self.E*self.I/linearRho)
        retval= list()
        if(numModes>0):
            retval.append(f1)
        if(numModes>1):
            retval.append(2.757*f1)
        if(numModes>2):
            retval.append(5.404*f1)
        if(numModes>3):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+': computation for modes greater than 3 not implemented.')
        return retval
