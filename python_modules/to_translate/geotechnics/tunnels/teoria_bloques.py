# -*- coding: utf-8 -*-

''' Macros that are useful in block theory calculations (see Goodman & Shi, Block theory and its application to rock engineering 1985)'''

from __future__ import print_function

import math
import geom

def computeDipPlane(alpha,beta, p):
    '''Return the dip plane defined by:
       alpha: dip angle.
       beta: dip direction.
       p: point for which the plane passes through.'''
    sAlpha= math.sin(alpha)
    A= sAlpha*math.sin(beta)
    B= sAlpha*math.cos(beta)
    C= math.cos(alpha)
    D= -(A*p.x+B*p.y+C*p.z)
    retval= geom.Plane()
    retval.ecuacion_general([A,B,C,D])
    return retval


def computePointsOfTangency(sectionName, vt1, vt2):
    '''Computes the points of tangency of the external vectors with the 
       section.'''
    retval= sectionName(retval= getPosTangAprox(vt1))
    pt0= retval.at(1)
    retval.clear()
    sectionName(retval= getPosTangAprox(vt2))
    pt1= retval.at(1)
    retval.clear()
    retval.insert(pt0)
    retval.insert(pt1)
    return retval
  
