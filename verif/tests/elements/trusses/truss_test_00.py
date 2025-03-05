# -*- coding: utf-8 -*-
''' Checks the orientation of the element's unary vectors.
Home made test'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import xc
from model import predefined_spaces
from materials import typical_materials


# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodeHandler)

## Define mesh.
### Define nodes.
l= 10 #Bar length
n1= nodeHandler.newNodeXY(0,0)
n2= nodeHandler.newNodeXY(0,l)

### Define material.
E= 30e6 #Young modulus
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

### Define elements.
''' We define nodes at the points where loads will be applied.
    We will not compute stresses, so we can use an arbitrary
    cross section of unit area.'''
modelSpace.setDefaultMaterial(elast) # Set the material for the new element.
modelSpace.setElementDimension(2) # Truss defined in a two-dimensional space.
truss= modelSpace.newElement("Truss",nodeTags= [n1.tag,n2.tag])
truss.sectionArea= 1

vI= truss.getIVector3d(True)
vJ= truss.getJVector3d(True)
vK= truss.getKVector3d(True)

# Orientation of the element's unary vectors.
ratio= (vI-geom.Vector3d(0,1,0)).getModulus()
ratio+= (vJ-geom.Vector3d(-1,0,0)).getModulus()
ratio+= (vK-geom.Vector3d(0,0,1)).getModulus()

#print('vI= ', vI, ' vJ= ',vJ, ' vK= ',vK)
#print("ratio= ",ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-15:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayDispRot(itemToDisp='uY')


