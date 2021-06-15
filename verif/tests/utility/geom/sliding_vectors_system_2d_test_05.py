# -*- coding: utf-8 -*-
''' Trivial verification test for sliding vector system 
    load distribution.'''

from __future__ import print_function
import xc_base
import geom
import math

# Sliding vector system.
O= geom.Pos2d(0.0,1.0)
V= geom.Vector2d(1.0,0.0)
svs1= geom.SlidingVectorsSystem2d(O,V,0.0)

# Points and weights.
points= [geom.Pos2d(-1.0,0.0), geom.Pos2d(1.0,0.0)]
weights= [1.0,1.0]

vectors1= svs1.distribute(points,weights)

error= (vectors1[0].getVector2d()-geom.Vector2d(0.5,0.5)).getModulus()
error+= (vectors1[1].getVector2d()-geom.Vector2d(0.5,-0.5)).getModulus()

vectors2= svs1.distribute(points)
error+= (vectors2[0].getVector2d()-geom.Vector2d(0.5,0.5)).getModulus()
error+= (vectors2[1].getVector2d()-geom.Vector2d(0.5,-0.5)).getModulus()


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

