# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import math

# Sliding vector system.
O= geom.Pos2d(0,0)
V= geom.Vector2d(0,22.0)
svs1= geom.SlidingVectorsSystem2d(O,V,0.0)

# Points and weights.
points= list()
weights= list()
for i in range(11):
    points.append(geom.Pos2d(i-5.0,0.0))
    weights.append(1.0)

vectors1= svs1.distribute(points,weights)

error= 0.0
for v in vectors1:
    error+= (v.getVector2d()-geom.Vector2d(0,2)).getModulus()
  
vectors2= svs1.distribute(points)
error= 0.0
for v in vectors2:
    error+= (v.getVector2d()-geom.Vector2d(0,2)).getModulus()
  
'''
print(points)
print(weights, len(weights))
print(error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if error<1e-12:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

