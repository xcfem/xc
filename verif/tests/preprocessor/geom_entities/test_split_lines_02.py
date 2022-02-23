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
interval= CooMax/10.0

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
lines= preprocessor.getMultiBlockTopology.getLines
# Ladder
## Sidepieces
pt0= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt1= points.newPoint(geom.Pos3d(CooMax,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(0.0, interval,0.0))
pt3= points.newPoint(geom.Pos3d(CooMax, interval,0.0))
l0= lines.newLine(pt0.tag,pt1.tag)
l1= lines.newLine(pt2.tag,pt3.tag)

## Crosspieces
for i in range(0,9):
    x= (i+1)*interval
    ptA= points.newPoint(geom.Pos3d(x,0.0,0.0))
    ptB= points.newPoint(geom.Pos3d(x,interval,0.0))
    l= lines.newLine(ptA.tag,ptB.tag)
    
numberOfLinesBeforeSplitting= len(lines)
numberOfPointsBeforeSplitting= len(points)
xcTotalSet= preprocessor.getSets.getSet('total')
xcTotalSet.getEntities.splitLinesAtIntersections(1e-3)
numberOfLinesAfterSplitting= len(lines)
numberOfPointsAfterSplitting= len(points)

ratio1= abs(numberOfLinesBeforeSplitting-11)
ratio2= abs(numberOfPointsBeforeSplitting-22)
ratio3= abs(numberOfLinesAfterSplitting-29)
ratio4= abs(numberOfPointsAfterSplitting-22)

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


