# -*- coding: utf-8 -*-
''' Base plate design according to document
    Base Plate and Anchor Rod Design Second Edition
    American Institute of Steel Construction, Inc.
'''
from __future__ import division
from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from postprocess.reports import common_formats as fmt
from misc_utils import log_messages as lmsg

# Base plates under concentric axial compressive loads.

def getConcreteStrength(A, fc, phi= 0.65):
    ''' Return the bearing strength of the concrete
        surface according to expression in section 
        3.1 of the guide for A1==A2.

    :param A: concrete bearing area.
    :param fc: minimum concrete compressive strength.
    :param phi: resistance factor for bearing on 
                concrete (defaults to 0.65).
    '''
    return phi*0.85*fc*A

def computeBasePlateArea(Pu, fc, phi= 0.65):
    ''' Return the required base plate area according
        to expression in section 3.1 of the guide
        for A1==A2.

    :param Pu: axial load (LRFD).
    :param fc: minimum concrete compressive strength.
    :param phi: resistance factor for bearing on 
                concrete (defaults to 0.65).
    '''
    return Pu/(phi*0.85*fc)

def getDelta(steelShape):
    ''' Return the Deltadimension of the base plate according
        to expression in section 3.1 of the guide.

    :param steelShape: steel shape of the shaft.
    '''
    d= steelShape.get('h') # Overall depth of member.
    bf= steelShape.get('b') # Flange width.
    return (0.95*d-0.8*bf)/2.0
  
def getNDimension(steelShape, Areq):
    ''' Return the N dimension of the base plate according
        to expression in section 3.1 of the guide.

    :param steelShape: steel shape of the shaft.
    :param Areq: base plate required area
    '''
    d= steelShape.get('h') # Overall depth of member.
    bf= steelShape.get('b') # Flange width.
    delta= getDelta(steelShape)
    Nmin= max(math.sqrt(Areq),d)
    return Nmin+delta

def getBDimension(steelShape, N, Areq):
    ''' Return the B dimension of the base plate.

    :param steelShape: steel shape of the shaft.
    :param Areq: base plate required area
    '''
    B=Areq/N
    bf= steelShape.get('b') # Flange width.
    delta= getDelta(steelShape)
    B= max(B,bf+delta)
    return B
  
def computePlateThickness(steelShape, N, B, Pu, Pp, Fy, phi= 0.9):
    ''' Return the required thickness of the base plate 
        according to expression in section 3.1 of the guide.

    :param steelShape: steel shape of the shaft.
    :param N: dimension parallel to the web of the shaft.
    :param B: dimension parallel to the flange of the shaft.
    :param Pu: axial load (LRFD).
    :param Pp: bearing strength of the concrete area (LRFD).
    :param Fy: base plate steel yield stress
    :param phi: resistance factor for base plate steel (defaults to 0.9).
    '''
    d= steelShape.get('h') # Overall depth of member.
    bf= steelShape.get('b') # Flange width.
    m= (N-0.95*d)/2.0
    n= (B-0.8*bf)/2.0
    X= ((4.0*d*bf)/(d+bf)**2)*Pu/Pp
    lmbd= 2.0*math.sqrt(X)/(1+math.sqrt(1-X))
    lmbd= min(lmbd,1.0)
    lmbd_np= lmbd*math.sqrt(d*bf)/4.0
    l= max(m,n,lmbd_np)
    retval= l*math.sqrt(2.0*Pu/(phi*Fy*B*N))
    return retval

# Design for shear

def computeFrictionStrength(Ac,fc,Pu, frictionCoef= 0.55, phi= 0.75):
    ''' Return the shear resistance of the base plate.

    :param Ac: concrete bearing area.
    :param fc: minimum concrete compressive strength.
    :param Pu: axial load (LRFD).
    :param frictionCoef: friction coefficient μ is 0.55 for steel on 
                         grout, and 0.7 for steel on concrete.
    :param phi: resistance factor for concrete on shear (defaults to 0.75).
    '''
    retval= phi*frictionCoef*Pu
    retval= min(retval, 0.2*fc*Ac)
    return retval
  
  
