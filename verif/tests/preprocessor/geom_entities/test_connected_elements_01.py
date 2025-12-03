# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Define problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Problem geometry
L= 10.0
pt1= modelSpace.newKPoint(-L,0,0)
pt2= modelSpace.newKPoint(0, 0, 0)
pt3= modelSpace.newKPoint(L, 0, 0)

l12= modelSpace.newLine(pt1, pt2)
l23= modelSpace.newLine(pt2, pt3)

# Mesh generation
## Define material
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
## Define seed element.
modelSpace.setDefaultMaterial(elast)
modelSpace.setElementDimension(2) # Truss defined in a two-dimensional space.
truss= modelSpace.newSeedElement('Truss')
truss.sectionArea= 10
## Generate mesh.
setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

mesh= feProblem.getDomain.getMesh
nnod= mesh.getNumNodes()
nelem= mesh.getNumElements()

centralNode= pt2.getNode()
leftElements= centralNode.getConnectedElements(l12)
testOK= (len(leftElements)==1)
leftElement= leftElements[0]
leftElementCentroidX= leftElement.getPosCentroid(True).x
err= (leftElementCentroidX+L/8)**2
rightElements= centralNode.getConnectedElements(l23)
testOK= testOK and (len(rightElements)==1)
rightElement= rightElements[0]
rightElementCentroidX= rightElement.getPosCentroid(True).x
err= (rightElementCentroidX-L/8)**2
err= math.sqrt(err)
testOK= testOK and (err<1e-10)

'''
print("number of nodes: ",nnod)
print("number of elements: ",nelem)
print('left element centroid: ', leftElementCentroidX)
print('right element centroid: ', rightElementCentroidX)
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (testOK):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')


#Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayBlocks()#setToDisplay= )
# oh.displayFEMesh()#setsToDisplay=[])
# oh.displayLocalAxes()


