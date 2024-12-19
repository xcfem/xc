# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
import os
from model import predefined_spaces
from materials import typical_materials

CooMaxX= 10
CooMaxY= 10
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density

numNodes= 0
numElem= 0
# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= elast2d.name
elem= seedElemHandler.newElement("FourNodeQuad")

pt0= modelSpace.newKPoint(0.0,0.0,0.0)
pt1= modelSpace.newKPoint(CooMaxX,0.0,0.0)
pt2= modelSpace.newKPoint(CooMaxX,CooMaxY,0.0)
pt3= modelSpace.newKPoint(0.0,CooMaxY,0.0)

s1= modelSpace.newQuadSurface(pt0, pt1, pt2, pt3)
s1.nDivI= 4
s1.nDivJ= 2

s1.genMesh(xc.meshDir.I)

xcTotalSet= modelSpace.getTotalSet()
xcTotalSet.fillDownwards()

# Check results.
## Check nodes on perimeter.
numNodesOnPerimeter= -4
for l in s1.getSides:
    edge= l.getEdge
    numNodesOnPerimeter+= len(edge.nodes)

numNodesOnPerimeterRef= 2*s1.nDivI+2*s1.nDivJ
ratio= abs(numNodesOnPerimeter-numNodesOnPerimeterRef)

testOK= (ratio<=1e-15)

## Check surface orientation.
refIVector= (pt1.getPos-pt0.getPos).normalized()
refJVector= geom.Vector3d(-refIVector.y, refIVector.x, refIVector.z)
sIVector= s1.getIVector
sJVector= s1.getJVector
surfErrI= (sIVector-refIVector).getModulus()
surfErrJ= (sJVector-refJVector).getModulus()

testOK= testOK and (surfErrI<1e-10) and (surfErrJ<1e-10)


## Check element orientation.
elemErrI= 0.0
elemErrJ= 0.0
for e in xcTotalSet.elements:
    iVector= e.getIVector3d(True)
    elemErrI+= (iVector-sIVector).getModulus2()
    jVector= e.getJVector3d(True)
    elemErrJ+= (jVector-sJVector).getModulus2()
elemErrI= math.sqrt(elemErrI)
elemErrJ= math.sqrt(elemErrJ)

testOK= testOK and (elemErrI<1e-10) and (elemErrJ<1e-10)

'''
print(numNodesOnPerimeter)
print(numNodesOnPerimeterRef)
print(elemErrI)
print(elemErrJ)
print(refIVector)
print(refJVector)
print(surfErrI)
print(surfErrJ)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLocalAxes()
