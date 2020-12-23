# -*- coding: utf-8 -*-
from __future__ import print_function

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import sys
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
points.defaultTag= 1
pt= points.newPntFromPos3d(geom.Pos3d(0,0,0))
pt= points.newPntFromPos3d(geom.Pos3d(1,1,1))
pt= points.newPntFromPos3d(geom.Pos3d(2,2,2))

setTotal= preprocessor.getSets.getSet("total")
ptsSet= setTotal.getPoints
numPoints= ptsSet.size
count= 0
#Point iterator
for p in ptsSet:
  count+= 1

cumple= (count == numPoints)
  

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if cumple:
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')



