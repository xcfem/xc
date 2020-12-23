# -*- coding: utf-8 -*-
from __future__ import print_function

import xc_base
import geom
import xc
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


NumDiv= 4
CooMax= NumDiv
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPntIDPos3d(2,geom.Pos3d(CooMax,CooMax,CooMax))

lines= preprocessor.getMultiBlockTopology.getLines
l1= lines.newLine(pt1.tag,pt2.tag)
l1.nDiv= NumDiv
l1.divide()

xcTotalSet= preprocessor.getSets.getSet("total")

# Store positions
positions= dict()
for p in xcTotalSet.points:
    pos= p.getPos
    positions[p.tag]= (pos.x, pos.y, pos.z)

zAxis= geom.Line3d(geom.Pos3d(0.0,0.0,0.0), geom.Pos3d(0.0,0.0,100.0))
rot= xc.Rotation(geom.Rotation3d(zAxis,math.pi))

# Rotate all
xcTotalSet.transforms(rot)

error= 0.0
# Check rotation.
for p in xcTotalSet.points:
    tag= p.tag
    oldPos= positions[tag]
    newPos= p.getPos
    error+= math.sqrt((oldPos[0]+newPos.x)**2+(oldPos[1]+newPos.y)**2+(oldPos[2]-newPos.z)**2)

# print(error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(error<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
