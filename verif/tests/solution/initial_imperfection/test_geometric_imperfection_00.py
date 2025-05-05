# -*- coding: utf-8 -*-
''' Check elastic beam 3D element update after a "manual" change in the
position of the nodes.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
import os
import geom
import xc
from materials.ec3 import EC3_materials
from model import predefined_spaces
from solution import predefined_solutions

#Materials
## Steel material
steel= EC3_materials.S235JR
steel.gammaM= 1.00
## Profile geometry
shape= EC3_materials.HEShape(steel= steel, name= 'HE_100_B')

## Problem geometry
height= 10.0 # column height.

# Finite element model.
## Problem type
steelColumn= xc.FEProblem()
steelColumn.title= 'Test geometry imperfection 01.'
preprocessor= steelColumn.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Model geometry

### Nodes
n0= nodes.newNodeXY(0,0)
n1= nodes.newNodeXY(0,height)

### Material.
xcSection= shape.defElasticShearSection2d(preprocessor)
### Mesh generation

### Geometric transformations
cooTrf= modelSpace.newLinearCrdTransf("lin") # Linear transformation.
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= cooTrf.name
elements.defaultMaterial= xcSection.name
#  syntax: beam2d_02[<tag>] 

elemA= elements.newElement("ElasticBeam2d",xc.ID([n0.tag,n1.tag]))

### Imperfection
e0= 10
pos= n1.getInitialPos3d
newX= e0
pos.x+= newX
n1.setPos(pos)
newCoo= n1.getCoo
err0= (newCoo-xc.Vector([10,10])).Norm()

# Update the coordinate transformation after a change in the node position.
elemA.resetNodalCoordinates()
elemB= elements.newElement("ElasticBeam2d",xc.ID([n0.tag,n1.tag]))

iVectorA= elemA.getIVector3d(True)
iVectorB= elemB.getIVector3d(True)
err1= (iVectorA-iVectorB).getModulus()

lengthA= elemA.getLength(True)
lengthB= elemB.getLength(True)
err2= abs(lengthA-lengthB)/lengthB

'''
print(newCoo)
print('err0= ', err0)
print(iVectorA, lengthA)
print(iVectorB, lengthB)
print('err1= ',err1)
print('err2= ',err2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(err0)<1e-10 and abs(err1)<1e-10 and abs(err2)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

