# -*- coding: utf-8 -*-
'''Simplified routines for design reinforced concrete elements.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from misc_utils import log_messages as lmsg

def neutralFiberDepth(M,fcd,b,d, c_depth= 0.85):
    '''Return the depth of the neutral fiber in a rectangular section.

    :param M: bending moment.
    :param fcd: concrete design compressive strength.
    :param b: section width.
    :param d: section effective depth.
    :param c_depth: depth of the concrete rectangular compression block
                    (see figure 12 SIA 262:2013  or figure 39.5b or 
                     clause 39.5 b of EHE)
    '''
    c= c_depth*fcd*b
    T= c*(d-math.sqrt(d**2-2*M/c))
    xpl= T/c
    assert xpl<=d, "xpl bigger than section depth."
    return xpl

def Mu(As, fcd, fsd, b, d, z= None, c_depth= 0.85):
    '''Return the ultimate bending moment of a rectangular section.

    :param As: reinforcement area.
    :param fcd: concrete design compressive strength.
    :param fsd: steel design yield strength.
    :param b: section width.
    :param d: section effective depth.
    :param z: inner lever arm (if None z= 0.9*d).
    :param c_depth: depth of the concrete rectangular compression block (see figure 12 SIA 262:2013  or figure 39.5b or clause 39.5 b of EHE)
    '''
    T= As*fsd
    if(z is None):
        z= 0.9*d
    c= c_depth*fcd*b
    xpl= T/c
    assert xpl<=d, "xpl bigger than section depth."
    return T*z

def Mlim(fcd,b,d):
    return 0.33*fcd*b*d**2 

def AsSimpleBending(M,fcd,fsd,b,d, c_depth= 0.85):
    ''' Return the required reinforcement for a rectangular section
        subjected to simple bending.

    :param M: bending moment to resist.
    :param fcd: concrete design compressive strength (absolute value).
    :param fsd: steel design yield strength.
    :param b: section width.
    :param d: section depth.
    :param c_depth: depth of the concrete rectangular compression block (see figure 12 SIA 262:2013 or figure 39.5b of clause 39.5 b of EHE).
    '''
    if(fcd<0.0):
        lmsg.warning('positive value expected for concrete design strength fcd= '+ str(fcd/1e6) + ' MPa.') 
    Ml= Mlim(fcd,b,d)
    if(M>Ml):
        lmsg.warning('compression reinforcement needed Ml= '+ str(Ml/1e3) + ' kN m < '+ str(M/1e3)+ ' kN m') 
        T= 1e9
    else:
        c= c_depth*fcd*b
        T= c*(d-math.sqrt(d**2-2*M/c))
        xpl= T/c
        assert xpl<=d, "xpl bigger than section depth."
    return T/fsd
