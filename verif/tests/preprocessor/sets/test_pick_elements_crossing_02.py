# -*- coding: utf-8 -*-
''' Pick the element crossing (i.e. having nodes inside and outside) a
 polygon. Home made test.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import geom
import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

## Problem geometry.
p1= modelSpace.newKPoint(-1, -1, 0)
p2= modelSpace.newKPoint(1, -1, 0)
p3= modelSpace.newKPoint(1, 1, 0)
p4= modelSpace.newKPoint(-1, 1, 0)

s= modelSpace.newQuadSurface(p1, p2, p3, p4)
s.setElemSizeIJ(.25, .25)

## Define material.
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E= 1e6, nu= 0.3, rho= 0.0)

## Generate mesh.
modelSpace.setDefaultMaterial(elast2d)
modelSpace.newSeedElement("FourNodeQuad")
s.genMesh(xc.meshDir.I)

xcTotalSet= modelSpace.getTotalSet()

## Critical polygon (sort of, there is no punching shear).
criticalPolygon= geom.Polygon2d([geom.Pos2d(-.625, 0.625), geom.Pos2d(.625, 0.625), geom.Pos2d(.625, -0.625), geom.Pos2d(-.625, -0.625)])
## Elements crossing critical polygon.
elementsOnCriticalPolygon= xcTotalSet.pickElemsCrossing('elementsOnCriticalContour', criticalPolygon, 0.0)

numberOfCrossingElements= len(elementsOnCriticalPolygon.elements)
ratio1= abs(numberOfCrossingElements-5*4)

'''
print(numberOfCrossingElements, ratio1)
for e in elementsOnCriticalPolygon.elements:
    print(e.tag, e.Crosses(criticalPolygon, 1.0, 0.0))
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# oh.displayFEMesh()
# # oh.displayLocalAxes()
# # oh.displayLoads()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
