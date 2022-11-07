# -*- coding: utf-8 -*-
''' Test curvatureVector function.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

# Points.
#            A
#           +
#          /\
#         / \
#        /  \
#       /   \
#      /    \
#     /     \
#    +------+
#   B        C

# Vertices (isosceles triangle).
O= geom.Pos2d(0,0)
angle= math.radians(-30)
c= math.cos(angle)
s= math.sin(angle)
A= geom.Pos2d(0, 1)
B= geom.Pos2d(-c, s)
C= geom.Pos2d(c, s)

vA= geom.curvatureVector(A,B,C)
ratio1= (vA-(O-A)).getModulus()
vB= geom.curvatureVector(B,C,A)
ratio2= (vB-(O-B)).getModulus()
vC= geom.curvatureVector(C,A,B)
ratio3= (vC-(O-C)).getModulus()
curvature= geom.curvature(A,B,C)
ratio4= abs(curvature-1.0)
radius= geom.curvatureRadius(A,B,C)
ratio5= abs(radius-1.0)

'''
print(c)
print(s)
print(vA, A-O)
print('ratio1= ', ratio1)
print(vB, B-O)
print('ratio2= ', ratio2)
print(vC, C-O)
print('ratio3= ', ratio3)
print(radius)
print('ratio4= ', ratio4)
print(curvature)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10 and abs(ratio5)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
