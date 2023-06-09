# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import os
import json
import math
import geom
import xc
from import_export import neutral_mesh_description as nmd

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

CooMax= 10.0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Problem geometry
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0,0,0))
pt2= points.newPoint(geom.Pos3d(CooMax,CooMax,CooMax))

lines= preprocessor.getMultiBlockTopology.getLines
l= lines.newLine(pt1.tag,pt2.tag)

# Write JSON file.
outputData= nmd.XCImportExportData()
xcTotalSet= preprocessor.getSets.getSet('total')
outputData.readFromXCSet(xcTotalSet)

outputData.outputFileName= os.path.basename(__file__).replace('.py','')
outputData.writeToJSON()

# Read back the JSON file.
jsonBlocksPath= outputData.outputFileName+'.json'
f= open(jsonBlocksPath,'r') # Open JSON file
data= json.load(f) # returns JSON object as a dictionary.
inputData= nmd.XCImportExportData()
inputData.setFromDict(data)

# Check imported values.
ptA= inputData.blockData.points[pt1.tag].coords
ptB= inputData.blockData.points[pt2.tag].coords

lA= inputData.blockData.blocks[l.tag]

ratio1= math.sqrt((ptA[0]-pt1.pos.x)**2+(ptA[1]-pt1.pos.y)**2+(ptA[2]-pt1.pos.z)**2)
ratio2= math.sqrt((ptB[0]-pt2.pos.x)**2+(ptB[1]-pt2.pos.y)**2+(ptB[2]-pt2.pos.z)**2)
ratio3= math.sqrt((lA.nodeIds[0]-l.firstVertex.tag))
ratio4= math.sqrt((lA.nodeIds[1]-l.lastVertex.tag))

'''
print(ptA)
print(ratio1)
print(ratio2)
print(ratio3)
print(ratio4)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-15) and (abs(ratio2)<1e-15) and (abs(ratio3)<1e-15) and (abs(ratio4)<1e-15)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
os.system('rm -f '+jsonBlocksPath) # Your garbage you clean it
