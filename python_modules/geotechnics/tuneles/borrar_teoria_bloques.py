# -*- coding: utf-8 -*-

# Macros that are useful in block theory calculations (ver Goodman & Shi, Block theory and its application to rock engineering 1985)

import geom
import math

def computeDipPlane(alpha,beta,p):
    '''
    Return the dip plane defined by:
    alpha: angle of dip.
    beta: Dirección del buzamiento.
    p: point for which the plane passes through.
    '''
    A=math.sin(alpha)*math.sin(beta)
    B=math.sin(alpha)*math.cos(beta)
    C=math.cos(alpha)
    D=-(A*p.x+B*p.y+C*p.z)

    retval=geom.Plane(geom.GeneralEquationOfPlane(A,B,C,D))
    return retval

#este módulo no está terminado
