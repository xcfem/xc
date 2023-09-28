# -*- coding: utf-8 -*-
''' Key-point setNode test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import xc

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

pos= geom.Pos3d(0,0,0)

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(pos)

nodes= preprocessor.getNodeHandler
n1= nodes.newNodeXYZ(pos.x, pos.y, pos.z)

pt1.setNode(n1)

ok= (pt1.getTagNode==n1.tag)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ok:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')



