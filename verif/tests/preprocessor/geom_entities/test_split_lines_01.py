from __future__ import print_function

import xc_base
import geom
import xc
import os
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

CooMax= 10

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
# Hexahedron vertices
## Bottom
pt0= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt1= points.newPoint(geom.Pos3d(CooMax,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(CooMax, CooMax,0.0))
pt3= points.newPoint(geom.Pos3d(0.0, CooMax,0.0))
## Top
pt4= points.newPoint(geom.Pos3d(0.0,0.0, CooMax))
pt5= points.newPoint(geom.Pos3d(CooMax,0.0, CooMax))
pt6= points.newPoint(geom.Pos3d(CooMax, CooMax, CooMax))
pt7= points.newPoint(geom.Pos3d(0.0, CooMax, CooMax))

lines= preprocessor.getMultiBlockTopology.getLines
# Bottom lines
l0= lines.newLine(pt0.tag,pt1.tag)
l1= lines.newLine(pt1.tag,pt2.tag)
l2= lines.newLine(pt2.tag,pt3.tag)
l3= lines.newLine(pt3.tag,pt0.tag)
# Top lines
l4= lines.newLine(pt4.tag,pt5.tag)
l5= lines.newLine(pt5.tag,pt6.tag)
l6= lines.newLine(pt6.tag,pt7.tag)
l7= lines.newLine(pt7.tag,pt4.tag)
# Up
l8= lines.newLine(pt0.tag, pt4.tag)
l9= lines.newLine(pt1.tag, pt5.tag)
l10= lines.newLine(pt2.tag, pt6.tag)
l11= lines.newLine(pt3.tag, pt7.tag)
# Lateral faces diagonals
l12= lines.newLine(pt0.tag, pt5.tag)
l13= lines.newLine(pt1.tag, pt6.tag)
l14= lines.newLine(pt2.tag, pt7.tag)
l15= lines.newLine(pt3.tag, pt4.tag)
l16= lines.newLine(pt1.tag, pt4.tag)
l17= lines.newLine(pt2.tag, pt5.tag)
l18= lines.newLine(pt3.tag, pt6.tag)
l19= lines.newLine(pt0.tag, pt7.tag)
# Interior diagonals
l20= lines.newLine(pt0.tag, pt6.tag)
l21= lines.newLine(pt1.tag, pt7.tag)
l22= lines.newLine(pt2.tag, pt4.tag)
l23= lines.newLine(pt3.tag, pt5.tag)
# Bottom diagonals
l24= lines.newLine(pt0.tag, pt2.tag)
l25= lines.newLine(pt1.tag, pt3.tag)
# Top diagonals
l26= lines.newLine(pt4.tag, pt6.tag)
l27= lines.newLine(pt5.tag, pt7.tag)

# Middle points
## Bottom
pt8= points.newPoint(l0.getCentroid())
pt9= points.newPoint(l1.getCentroid())
pt10= points.newPoint(l2.getCentroid())
pt11= points.newPoint(l3.getCentroid())
## Top
pt12= points.newPoint(l4.getCentroid())
pt13= points.newPoint(l5.getCentroid())
pt14= points.newPoint(l6.getCentroid())
pt15= points.newPoint(l7.getCentroid())
## Up
pt16= points.newPoint(l8.getCentroid())
pt17= points.newPoint(l9.getCentroid())
pt18= points.newPoint(l10.getCentroid())
pt19= points.newPoint(l11.getCentroid())

# Middle lines
## Lateral faces
### Vertical
l28= lines.newLine(pt8.tag,pt12.tag)
l29= lines.newLine(pt9.tag,pt13.tag)
l30= lines.newLine(pt10.tag,pt14.tag)
l31= lines.newLine(pt11.tag,pt15.tag)
### Horizontal
l32= lines.newLine(pt16.tag, pt17.tag)
l33= lines.newLine(pt17.tag, pt18.tag)
l34= lines.newLine(pt18.tag, pt19.tag)
l35= lines.newLine(pt19.tag, pt16.tag)
### Bottom
l36= lines.newLine(pt8.tag,pt10.tag)
l37= lines.newLine(pt9.tag,pt11.tag)
### Top
l36= lines.newLine(pt12.tag,pt14.tag)
l37= lines.newLine(pt13.tag,pt15.tag)

numberOfLinesBeforeSplitting= len(lines)
numberOfPointsBeforeSplitting= len(points)
xcTotalSet= preprocessor.getSets.getSet('total')
xcTotalSet.getEntities.splitLinesAtIntersections(1e-3)
numberOfLinesAfterSplitting= len(lines)
numberOfPointsAfterSplitting= len(points)

ratio1= abs(numberOfLinesBeforeSplitting-40)
ratio2= abs(numberOfPointsBeforeSplitting-20)
ratio3= abs(numberOfLinesAfterSplitting-80)
ratio4= abs(numberOfPointsAfterSplitting-27)

'''
print('number of lines before splitting: ', numberOfLinesBeforeSplitting)
print('number of points before splitting: ', numberOfPointsBeforeSplitting)
print('number of lines after splitting: ', numberOfLinesAfterSplitting)
print('number of points after splitting: ', numberOfPointsAfterSplitting)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1==0) and (ratio2==0) and (ratio3==0) and (ratio4==0)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# nodes= preprocessor.getNodeHandler
# modelSpace= predefined_spaces.StructuralMechanics3D(nodes) 
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayBlocks()


