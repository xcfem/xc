# -*- coding: utf-8 -*-
'''Selection of elements inside a polygon.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import xc
from model import predefined_spaces
from materials import typical_materials

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Problem geometry.
pt0= modelSpace.newKPoint(0.0,0.0,0.0)
pt1= modelSpace.newKPoint(1.0,0.0,0.0)
pt2= modelSpace.newKPoint(1.0,1.0,0.0)
pt3= modelSpace.newKPoint(0.0,1.0,0.0)
s= modelSpace.newQuadSurface(pt0,pt1,pt2, pt3)

# Material.
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E= 1.0, nu= 0.3, rho= 0.0)

# Mesh generation.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= elast2d.name
elem= seedElemHandler.newElement("FourNodeQuad")
s.setElemSizeIJ(0.1, 0.1)
s.genMesh(xc.meshDir.I)

# Element picking.
## Polygon to pick elements inside.
pickPolygon= geom.Polygon3d([geom.Pos3d(0.5, 0.5,0.0), geom.Pos3d(1.0, 0.5, 0.0), geom.Pos3d(1,1,0), geom.Pos3d(0.5,1,0)])
## Set to store the result.
pickedElements= modelSpace.defSet('pickedElements')
## Pick the elements from the total set.
modelSpace.pickElementsInZone(zone= pickPolygon, resultSet= pickedElements)

numElements= len(modelSpace.getTotalSet().elements)
refNumPickedElements= numElements/4
numPickedElements= len(pickedElements.elements)
ratio1= (numPickedElements-refNumPickedElements)/refNumPickedElements

ok= True
for e in pickedElements.elements:
    center= e.getPosCentroid(True)
    ok= ok and (center.x>0.5) and (center.y>0.5)

'''
print(numElements)
print(numPickedElements)
print(refNumPickedElements)
print('ratio1= ', ratio1)
print(ok)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ok and (abs(ratio1)<1e-9):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


