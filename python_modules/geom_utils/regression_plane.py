# -*- coding: utf-8 -*-
''' Compute regreesion plane from a set of points.'''

import scipy.optimize
import functools

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2022 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

def plane(x, y, params):
    a = params[0]
    b = params[1]
    c = params[2]
    z = a*x + b*y + c
    return z

def error(params, points):
    result = 0
    for (x,y,z) in points:
        plane_z = plane(x, y, params)
        diff = abs(plane_z - z)
        result += diff**2
    return result

def getRegressionPlane(points):
    '''Ax+By+Cz+D= 0'''
    fun = functools.partial(error, points=points)
    params0 = [0, 0, 0]
    res = scipy.optimize.fmin_cg(fun, params0)
    A= -res[0]
    B= -res[1]
    C= 1.0
    D = -res[2]
    return [A,B,C,D]




