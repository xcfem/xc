# -*- coding: utf-8 -*-
'''Simple beam formulas.'''

from __future__ import print_function
from __future__ import division

import sys
from rough_calculations import ng_beam as bm
from misc_utils import log_messages as lmsg

#Macaulay functions.
def macaulay(x,a,n=0):
    if(x<a):
        return 0.0
    else:
        if(n==0):
            return 1.0
        else:
            return (x-a)**n
  

#  ___________________|
# ^                   |

class PinnedFixedBeam(bm.Beam):
    def getShearUnderUniformLoad(self,q,x):
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented.')
        return None
    def getReactionUnderUniformLoad(self,q):
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented.')
        return None
    def getBendingMomentUnderUniformLoad(self,q,x):
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented.')
        return None
    def getDeflectionUnderUniformLoad(self,q,x):
        return -q*x/(48.0*self.E*self.I)*(self.l**3-3*self.l*x**2+2*x**3)

#          l
# |<---------------->|
#    a      b     c
# |<-->|<------>|<--->|
#      ||||||||||
#  ___________________|
# ^                   |
    def getReaction1UnderUniformLoadPartiallyDistributed(self,q,a,b):
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented.')
        return None

    def getReaction2UnderUniformLoadPartiallyDistributed(self,q,a,b):
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented.')
        return None

    def getShearUnderUniformLoadPartiallyDistributed(self,q,a,b,x):
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented.')
        return None

    def getBendingMomentUnderUniformLoadPartiallyDistributed(self,q,a,b,x):
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented.')
        return None

#          l
# |<---------------->|
#    a         b
# |<--->|<---------->|
#       |
#  _____|_____________|
# ^                   |
    def getReaction1UnderConcentratedLoad(self,P,a):
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented.')
        return None

    def getReaction2UnderConcentratedLoad(self,P,a):
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented.')
        return None

    def getShearUnderConcentratedLoad(self,P,a,x):
        b= self.l-a
        return P*(b**2/(2*self.l**3)*(2*self.l+a)-macaulay(x,a,0))

    def getBendingMomentUnderConcentratedLoad(self,P,a,x):
        b= self.l-a
        return P*(b**2/(2*self.l**3)*x*(2*self.l+a)-macaulay(x,a,1))

    def getDeflectionUnderConcentratedLoad(self,P,a,x):
        b= self.l-a
        sum1= b**2*x**3/(2.0*self.l**3)*(2*self.l+a)
        sum2= -3*a*b**2*x/(2*self.l)
        sum3= -macaulay(x,a,3)
        return P/(6.0*self.E*self.I)*(sum1+sum2+sum3)
