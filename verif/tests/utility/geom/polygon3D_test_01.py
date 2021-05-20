# -*- coding: utf-8 -*-
'''Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom

pointsA= [geom.Pos3d(0,0,0), geom.Pos3d(0,1,0), geom.Pos3d(0,1,1), geom.Pos3d(0,0,1)]


plgA= geom.Polygon3d(pointsA)
centerA= plgA.getCenterOfMass()

dcA= plgA.dist(centerA)
ratio1= dcA

dvA= -1e6
for p in pointsA:
    dvA= max(dvA, plgA.dist(p))

ratio2= dvA

pointsB= [geom.Pos3d(1.97898,-2.045,0.271598), geom.Pos3d(2.045,-2.045,0.271598), geom.Pos3d(2.045,-2.045,0), geom.Pos3d(1.97471,-2.045,-1.31561e-17), geom.Pos3d(1.8876,-2.045,0.230975)]

plgB= geom.Polygon3d(pointsB)
centerB= plgB.getCenterOfMass()

dcB= plgB.dist(centerB)
ratio3= dcB

dvB= -1e6
for p in pointsB:
    dvB= max(dvB, plgB.dist(p))

ratio4= dvB

pointsC= [geom.Pos3d(-2.2305,-1.8744,0), geom.Pos3d(-2.045,-1.8744,0), geom.Pos3d(-2.045,-1.8744,1.5), geom.Pos3d(-2.2305,-1.8744,1.5)]

plgC= geom.Polygon3d(pointsC)
p= geom.Pos3d(-2.045,-1.8744,0.607318)
dcC= plgC.dist(p)
ratio5= dcC

'''
print('center A: ', centerA, 'distance: ', dcA)
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('ratio3= ', ratio3)
print('ratio4= ', ratio4)
print('ratio5= ', ratio5)
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) and (abs(ratio2)<1e-12) and (abs(ratio3)<1e-12) and (abs(ratio4)<1e-12)and (abs(ratio5)<1e-12) :
  print("test "+fname+": ok.")
else:
  print("test "+fname+": ERROR.")
