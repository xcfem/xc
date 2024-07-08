# -*- coding: utf-8 -*-
''' Check element "Crosses" method. Home made test.'''

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

## Define nodes.
n1= nodes.newNodeXY(2,2)
n2= nodes.newNodeXY(3,2)
n3= nodes.newNodeXY(3,3)
n4= nodes.newNodeXY(2,3)

## Define material.
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E= 1e6, nu= 0.3, rho= 0.0)

### Define elements.
modelSpace.setDefaultMaterial(elast2d)
elem= modelSpace.newElement("FourNodeQuad", [n1.tag, n2.tag, n3.tag, n4.tag])

### Define test polygons.
testPolygonA= geom.Polygon2d([geom.Pos2d(0, 0), geom.Pos2d(1, 0), geom.Pos2d(1, 1), geom.Pos2d(0, 1)])
testPolygonB= geom.Polygon2d([geom.Pos2d(0, 0), geom.Pos2d(2.5, 0), geom.Pos2d(2.5, 2.5), geom.Pos2d(0, 2.5)])

# Check results.
crossesPolygonA= elem.Crosses(testPolygonA, 1.0, 0.0)
crossesPolygonB= elem.Crosses(testPolygonB, 1.0, 0.0)

'''
print(elem.tag, crossesPolygonA)
print(elem.tag, crossesPolygonB)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((not crossesPolygonA) and crossesPolygonB):
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
