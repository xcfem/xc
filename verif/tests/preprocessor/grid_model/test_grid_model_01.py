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
from model.grid_based import ijkGrid

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
gridA=ijkGrid.ijkGrid(prep,xListA,yList,zList)

gridA.generatePoints()
gA_nbPoints=gridA.pointCounter
ratio0=gA_nbPoints-len(xListA)*len(yList)*len(zList)
# print ratio0


# grid B is = grid A moved 10 units in X direction
xListB=[i+10 for i in range(6)]
gridB=ijkGrid.ijkGrid(prep,xListB,yList,zList)

gridB.generatePoints()
gB_nbPoints=gridB.pointCounter
ratio1=gB_nbPoints-len(xListB)*len(yList)*len(zList)
# print ratio1

# Ranges to be used lately
planesXY_2_3level=ijkGrid.IJKRange((0,0,2),(lastXpos,lastYpos,3)).extractIncludedIJranges()
planesYZ_1_4level=ijkGrid.IJKRange((0,0,0),(lastXpos,lastYpos,4)).extractIncludedJKranges(step=3)
planesXZ_0_floor=ijkGrid.IJKRange((0,0,0),(lastXpos,lastYpos,1)).extractIncludedIKranges(step=1)
planesYZ_3_floor=ijkGrid.IJKRange((0,0,3),(lastXpos,lastYpos,4)).extractIncludedJKranges(step=3)
beams_3_4_floor=ijkGrid.IJKRange((0,0,3),(lastXpos,lastYpos,4)).extractIncludedIranges(stepJ=2,stepK=1)
beams_2_3_floor=ijkGrid.IJKRange((0,0,2),(lastXpos,lastYpos,3)).extractIncludedJranges(stepI=2,stepK=1)
colums_1_4_floor=ijkGrid.IJKRange((0,0,0),(lastXpos,lastYpos,lastZpos)).extractIncludedKranges(stepI=2,stepJ=3)

#Surfaces and lines' generation gridA
gridA.genSurfMultiRegion(lstIJKRange=planesXY_2_3level)
set_planesXY_2_3level=gridA.getSetSurfMultiRegion(lstIJKRange=planesXY_2_3level,nmbrSet='set_planesXY_2_3level')
nsurf01=set_planesXY_2_3level.getSurfaces.size
ratio2=nsurf01-(len(xListA)-1)*(len(yList)-1)*2
# print ratio2

gridA.genSurfMultiRegion(lstIJKRange=planesYZ_1_4level)
set_planesYZ_1_4level=gridA.getSetSurfMultiRegion(lstIJKRange=planesYZ_1_4level,nmbrSet='set_planesYZ_1_4level')
nsurf02=set_planesYZ_1_4level.getSurfaces.size
ratio3=nsurf02-(len(yList)-1)*2*4
# print ratio3

#Lines' generation gridB
gridB.genLinMultiRegion(planesXZ_0_floor)
set_lines_planesXZ_0_floor=gridB.getSetLinMultiRegion(lstIJKRange=planesXZ_0_floor,nmbrSet='set_lines_planesXZ_0_floor')
nlin01=set_lines_planesXZ_0_floor.getLines.size
ratio4=nlin01-(len(xListB)*len(yList)+2*(len(xListB)-1)*len(yList))
# print ratio4

gridB.genLinMultiRegion(beams_3_4_floor)
set_beams_3_4_floor=gridB.getSetLinMultiRegion(lstIJKRange=beams_3_4_floor,nmbrSet='set_beams_3_4_floor')
nlin02=set_beams_3_4_floor.getLines.size
ratio5=nlin02-2*len(xListB)*int(len(yList)/2)
# print ratio5

gridB.genLinMultiRegion(beams_2_3_floor)
set_beams_2_3_floor=gridB.getSetLinMultiRegion(lstIJKRange=beams_2_3_floor,nmbrSet='set_beams_2_3_floor')
nlin03=set_beams_2_3_floor.getLines.size
ratio6=nlin03-2*(len(yList)-1)*int(len(xListB)/2)
# print ratio6

gridB.genLinMultiRegion(colums_1_4_floor)
set_colums_1_4_floor=gridB.getSetLinMultiRegion(lstIJKRange=colums_1_4_floor,nmbrSet='set_colums_1_4_floor')
nlin04=set_colums_1_4_floor.getLines.size
ratio7=nlin04-(len(zList)-1)*int(len(xListB)/2)*(int(len(yList)/3)+1)
# print ratio7

#Checks
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio0<=1e-10) & (ratio1<=1e-10) & (ratio2<=1e-10) & (ratio3<=1e-10) & (ratio4<=1e-10) & (ratio5<=1e-10) & (ratio6<=1e-10) & (ratio7<=1e-10) :
  "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


#Plot
# from xcVtk.malla_cad import vtk_grafico_cad

# defDisplay= vtk_grafico_cad.RecordDefDisplayCAD()
# totalSet= prep.getSets.getSet('total')
# defDisplay.displayBlocks(xcSet=totalSet,fName= None,caption= 'Model grid')
