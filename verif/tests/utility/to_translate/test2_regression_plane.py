# -*- coding: utf-8 -*-
''' Regression plane computation test.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import regression_plane

points = [(0,0,900),
          (1,0,900),
          (0,1,900)]

result= regression_plane.getRegressionPlane(points)

print("A,B,C,D= ", result)

errMax= 0.0
for (x,y,z) in points:
  err= result[0]*x+result[1]*y+result[2]*z+result[3]
  errMax= max(abs(err),errMax)

print("err maximale: ", errMax)
