# -*- coding: utf-8 -*-
from __future__ import print_function

import xc_base
import geom
import xc
import os

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDiv= 25
CooMax= 10

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPoint(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPoint(2,geom.Pos3d(CooMax,CooMax,CooMax))
pt= points.newPoint(3,geom.Pos3d(CooMax/2,CooMax/2,CooMax/2))

lines= preprocessor.getMultiBlockTopology.getLines
lines.defaultTag= 1
l1= lines.newLine(1,2)
l1.nDiv= NumDiv

l1.splitAtPoint(points.get(3))
kpoints1= l1.getKPoints()
l2= lines.get(2)
kpoints2= l2.getKPoints()

ratio1= (kpoints1[0]-1)+(kpoints1[1]-3)
ratio2= (kpoints2[0]-3)+(kpoints2[1]-2)

''' 
print("K points linea 1: ",kpoints1)
print("ratio1= ",ratio1)
print("K points linea 2: ",kpoints2)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1 < 1e-20) & (ratio2 < 1e-20)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


