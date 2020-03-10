# -*- coding: utf-8 -*-

#Test grid model 02. Generation/selection of geometric entities (points, lines and surfaces)
__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import os
import xc_base
import geom
import xc
from model.geometry import grid_model as gm

# *** GEOMETRY ***
FEcase= xc.FEProblem()
prep=FEcase.getPreprocessor

# coordinates in global X,Y,Z axes for the generation of grid A
xList=[i for i in range(6)]
yList=[i for i in range(11)]
zList=[i for i in range(2)]
#Points' generation
gridModel=gm.GridModel(prep,xList,yList,zList)
gridModel.generatePoints()

#Sets of points
set01=gridModel.getSetPntXYZRange(xyzRange=((0,0,0),(5,10,0)),setName='set01')
np_set01=set01.getPoints.size
ratio1=np_set01-(6*11)
set02=gridModel.getSetPntMultiXYZRegion(lstXYZRange=[((0,0,0),(5,5,0)),((0,6,0),(5,10,0))],setName='set02')
np_set02=set02.getPoints.size
ratio2=np_set02-(6*11)

#Sets of surfaces
set03=gridModel.genSurfMultiXYZRegion(lstXYZRange=[((0,0,0),(5,5,0)),((0,5,0),(5,10,0))], nameSet='set03')
ns_set03=set03.getSurfaces.size
ratio3=ns_set03-(10*5)
set04=gridModel.genSurfOneXYZRegion(xyzRange=((0,0,0),(5,10,0)), nameSet='set04')
ns_set04=set04.getSurfaces.size
ratio4=ns_set04-(10*5)
set05=gridModel.getSetSurfMultiXYZRegion(lstXYZRange=[((0,0,0),(5,5,0)),((0,5,0),(5,10,0))], nameSet='set05')
ns_set05=set05.getSurfaces.size
ratio5=ns_set05-(10*5)
set06=gridModel.getSetSurfOneXYZRegion(xyzRange=((0,0,0),(5,10,0)), nameSet='set06')
ns_set06=set06.getSurfaces.size
ratio6=ns_set06-(10*5)

#Sets of Lines
set07=gridModel.genLinMultiXYZRegion(lstXYZRange=[((0,0,1),(5,5,1)),((0,5,1),(5,10,1))], nameSet='set07')
ns_set07=set07.getLines.size
ratio7=ns_set07-(5*11+6*10)
set08=gridModel.genLinOneXYZRegion(xyzRange=((0,0,1),(5,10,1)), nameSet='set08')
ns_set08=set08.getLines.size
ratio8=ns_set08-(5*11+6*10)
set09=gridModel.getSetLinMultiXYZRegion(lstXYZRange=[((0,0,1),(5,5,1)),((0,5,1),(5,10,1))], nameSet='set09')
ns_set09=set09.getLines.size
ratio9=ns_set09-(5*11+6*10)
set10=gridModel.getSetLinOneXYZRegion(xyzRange=((0,0,1),(5,10,1)), nameSet='set10')
ns_set10=set10.getLines.size
ratio10=ns_set10-(5*11+6*10)

#Checks
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<=1e-10) & (ratio2<=1e-10) & (ratio3<=1e-10) & (ratio4<=1e-10) & (ratio5<=1e-10) & (ratio6<=1e-10) & (ratio7<=1e-10)  & (ratio8<=1e-10) & (ratio9<=1e-10) & (ratio10<=1e-10) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
