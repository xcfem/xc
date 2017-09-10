# -*- coding: utf-8 -*-

#Test grid model 01. Generation/selection of geometric entities (points, lines and surfaces)
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
FEcase= xc.ProblemaEF()
prep=FEcase.getPreprocessor

# coordinates in global X,Y,Z axes for the generation of grid A
xListA=[i for i in range(6)]
yList=[i for i in range(11)]
zList=[i for i in range(5)]

#auxiliary data
lastXpos=len(xListA)-1
lastYpos=len(yList)-1
lastZpos=len(zList)-1

#Points' generation
gridA=gm.GridModel(prep,xListA,yList,zList)

gridA.generatePoints()
gA_nbPoints=gridA.pointCounter
ratio0=gA_nbPoints-len(xListA)*len(yList)*len(zList)
# print ratio0


# grid B is = grid A moved 10 units in X direction
xListB=[i+10 for i in range(6)]
gridB=gm.GridModel(prep,xListB,yList,zList)

gridB.generatePoints()
gB_nbPoints=gridB.pointCounter
ratio1=gB_nbPoints-len(xListB)*len(yList)*len(zList)
# print ratio1

# Ranges to be used lately
planesXY_2_3level=gm.IJKRange((0,0,2),(lastXpos,lastYpos,3)).extractIncludedIJranges()
planesYZ_1_4level=gm.IJKRange((0,0,0),(lastXpos,lastYpos,4)).extractIncludedJKranges(step=3)
planesXZ_0_floor=gm.IJKRange((0,0,0),(lastXpos,lastYpos,1)).extractIncludedIKranges(step=1)
planesYZ_3_floor=gm.IJKRange((0,0,3),(lastXpos,lastYpos,4)).extractIncludedJKranges(step=3)
beams_3_4_floor=gm.IJKRange((0,0,3),(lastXpos,lastYpos,4)).extractIncludedIranges(stepJ=2,stepK=1)
beams_2_3_floor=gm.IJKRange((0,0,2),(lastXpos,lastYpos,3)).extractIncludedJranges(stepI=2,stepK=1)
colums_1_4_floor=gm.IJKRange((0,0,0),(lastXpos,lastYpos,lastZpos)).extractIncludedKranges(stepI=2,stepJ=3)

#Surfaces and lines' generation gridA
set01=gridA.genSurfMultiRegion(lstIJKRange=planesXY_2_3level,nameSet='set01')
set_planesXY_2_3level=gridA.getSetSurfMultiRegion(lstIJKRange=planesXY_2_3level,nameSet='set_planesXY_2_3level')
nsurf01=(set_planesXY_2_3level.getSurfaces.size+set01.getSurfaces.size)/2
ratio2=nsurf01-(len(xListA)-1)*(len(yList)-1)*2
# print ratio2

set02=gridA.genSurfMultiRegion(lstIJKRange=planesYZ_1_4level,nameSet='set02')
set_planesYZ_1_4level=gridA.getSetSurfMultiRegion(lstIJKRange=planesYZ_1_4level,nameSet='set_planesYZ_1_4level')
nsurf02=(set_planesYZ_1_4level.getSurfaces.size+set02.getSurfaces.size)/2
ratio3=nsurf02-(len(yList)-1)*2*4
# print ratio3

#Lines' generation gridB
set03=gridB.genLinMultiRegion(planesXZ_0_floor,nameSet='set03')
set_lines_planesXZ_0_floor=gridB.getSetLinMultiRegion(lstIJKRange=planesXZ_0_floor,nameSet='set_lines_planesXZ_0_floor')
nlin01=(set_lines_planesXZ_0_floor.getLines.size+set03.getLines.size)/2
ratio4=nlin01-(len(xListB)*len(yList)+2*(len(xListB)-1)*len(yList))
# print ratio4

set04=gridB.genLinMultiRegion(beams_3_4_floor,nameSet='set04')
set_beams_3_4_floor=gridB.getSetLinMultiRegion(lstIJKRange=beams_3_4_floor,nameSet='set_beams_3_4_floor')
nlin02=(set_beams_3_4_floor.getLines.size+set04.getLines.size)/2
ratio5=nlin02-2*len(xListB)*int(len(yList)/2)
# print ratio5

set05=gridB.genLinMultiRegion(beams_2_3_floor,nameSet='set05')
set_beams_2_3_floor=gridB.getSetLinMultiRegion(lstIJKRange=beams_2_3_floor,nameSet='set_beams_2_3_floor')
nlin03=(set_beams_2_3_floor.getLines.size+set05.getLines.size)/2
ratio6=nlin03-2*(len(yList)-1)*int(len(xListB)/2)
# print ratio6

set06=gridB.genLinMultiRegion(colums_1_4_floor,nameSet='set06')
set_colums_1_4_floor=gridB.getSetLinMultiRegion(lstIJKRange=colums_1_4_floor,nameSet='set_colums_1_4_floor')
nlin04=(set_colums_1_4_floor.getLines.size+set06.getLines.size)/2
ratio7=nlin04-(len(zList)-1)*int(len(xListB)/2)*(int(len(yList)/3)+1)
# print ratio7

#Checks
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio0<=1e-10) & (ratio1<=1e-10) & (ratio2<=1e-10) & (ratio3<=1e-10) & (ratio4<=1e-10) & (ratio5<=1e-10) & (ratio6<=1e-10) & (ratio7<=1e-10) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


#Plot
# from postprocess.xcVtk.CAD_model import vtk_CAD_graphic

# defDisplay= vtk_CAD_graphic.RecordDefDisplayCAD()
# totalSet= prep.getSets.getSet('total')
# defDisplay.displayBlocks(xcSet=totalSet,fName= None,caption= 'Model grid')
