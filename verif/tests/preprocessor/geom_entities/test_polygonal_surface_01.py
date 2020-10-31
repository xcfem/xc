# -*- coding: utf-8 -*-
''' There was a problem when obtaining a 3D polygon from the
    polygonal surface (the last vertex was duplicated)
    this is a regression test to check that this doesn't
    happen again. Home made test.'''

from __future__ import division
from __future__ import print_function

import sys
import math
import xc_base
import geom
import xc

from model import predefined_spaces

TestProblem= xc.FEProblem()
TestProblem.title= 'Test model'
preprocessor= TestProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
elements= preprocessor.getElementHandler
points= preprocessor.getMultiBlockTopology.getPoints
lines= preprocessor.getMultiBlockTopology.getLines
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
groups= preprocessor.getSets

pt12= points.newPntIDPos3d(12,geom.Pos3d(-2.045,-1.80837510901,0.655344163406)); pt12.setProp("labels",['gusset_plate', 'flange_gusset', 'joint_1800'])
pt13= points.newPntIDPos3d(13,geom.Pos3d(-2.045,-1.8744,0.607318379568)); pt13.setProp("labels",['gusset_plate', 'flange_gusset', 'joint_1800'])
pt14= points.newPntIDPos3d(14,geom.Pos3d(-2.045,-1.8744,0.383745721271)); pt14.setProp("labels",['gusset_plate', 'flange_gusset', 'joint_1800'])
pt15= points.newPntIDPos3d(15,geom.Pos3d(-2.045,-1.8744,0.0)); pt15.setProp("labels",['gusset_plate', 'flange_gusset', 'joint_1800'])
pt16= points.newPntIDPos3d(16,geom.Pos3d(-2.045,-1.7169980504,0.614721101481)); pt16.setProp("labels",['gusset_plate', 'flange_gusset', 'joint_1800'])

f5= surfaces.newPolygonalFacePts([12, 13, 14, 15, 16]); f5.setProp("labels",['gusset_plate', 'flange_gusset', 'joint_1800']); f5.setProp("thickness",0.015); f5.setProp('matId','A36')

plg= f5.getPolygon();
d= plg.dist(geom.Pos3d(0,0,0))

ratio= (d-2.045)/2.045

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-12):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

