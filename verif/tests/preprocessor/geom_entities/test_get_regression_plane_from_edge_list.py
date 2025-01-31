# -*- coding: utf-8 -*-
from __future__ import print_function

import math
import geom
import xc
from model.geometry import geom_utils

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Create FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define some points.
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p1= pointHandler.newPoint(geom.Pos3d(0.0,0.0,0.0))
p2= pointHandler.newPoint(geom.Pos3d(1.0,0.0,0.0))
p3= pointHandler.newPoint(geom.Pos3d(1.0,1.0,1.0))
p4= pointHandler.newPoint(geom.Pos3d(0.0,1.0,1.0))

# Define some lines.
lineHandler= preprocessor.getMultiBlockTopology.getLines
lineList= list()
lineList.append(lineHandler.newLine(p1.tag, p2.tag))
lineList.append(lineHandler.newLine(p2.tag, p3.tag))
lineList.append(lineHandler.newLine(p3.tag, p4.tag))
lineList.append(lineHandler.newLine(p4.tag, p1.tag))

## Check get_line_sequence_from_point_sequence.
plane= geom_utils.get_regression_plane_from_edge_list(lineList)
kVector= plane.getNormal()
half_sqrt2= math.sqrt(2)/2
kVectorRef= geom.Vector3d(0.0, -half_sqrt2, half_sqrt2)
error= (kVector-kVectorRef).getModulus()

'''
print(kVector)
print(error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(error<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')



