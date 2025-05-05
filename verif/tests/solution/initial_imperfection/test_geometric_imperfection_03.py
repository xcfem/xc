# -*- coding: utf-8 -*-
''' Check internal forces on imperfect member.'''

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
steelColumn.title= 'Test geometric imperfection.'
preprocessor= steelColumn.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Model geometry

### Points.
p0= modelSpace.newKPoint(0.0,0.0,0.0)
p1= modelSpace.newKPoint(0.0,0.0,height)

### Lines
l1= modelSpace.newLine(p0,p1)
l1.nDiv= 6

### Material.
xcSection= shape.defElasticShearSection3d(preprocessor)
### Mesh generation

### Geometric transformations
cooTrf= modelSpace.newCorotCrdTransf("corot", xzVector= xc.Vector([1,0,0])) # Corotational transformation.

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 2 # Bars defined in a two-dimensional space.
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= cooTrf.name
elem= seedElemHandler.newElement("ElasticBeam3d")
xcTotalSet= preprocessor.getSets.getSet('total')
mesh= xcTotalSet.genMesh(xc.meshDir.I)


### Constraints
modelSpace.fixNode('000_FF0', p0.getNode().tag)
modelSpace.fixNode('00F_FFF', p1.getNode().tag)

### Imperfection
e0= 0.1
for n in xcTotalSet.nodes:
    pos= n.getInitialPos3d
    newX= e0*math.sin(pos.z*math.pi/height)
    pos.x+= newX
    n.setPos(pos)

# Update the coordinate transformations after a change in the position of
# the nodes.
for e in xcTotalSet.elements:
    e.resetNodalCoordinates()
    #print('I=', e.getIVector3d(True))
    #print('J=', e.getJVector3d(True))

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
## Point load.
cLC= modelSpace.setCurrentLoadPattern(lp0.name)
P= 10e3
pLoad= xc.Vector([0.0, 0.0, -P, 0.0, 0.0, 0.0])
p1.getNode().newLoad(pLoad)
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.plain_newton_raphson(steelColumn, maxNumIter= 20, convergenceTestTol= 1e-6)
result= analysis.analyze(1)

# Compute maximum bending moment.
mMax= 0.0
for e in xcTotalSet.elements:
    M= abs(e.getMy1)
    if(M>mMax):
        mMax= M
    M= abs(e.getMy2)
    if(M>mMax):
        mMax= M

# Compute maximum bending displacement.
mxDisp= 0.0
for n in xcTotalSet.nodes:
    xDisp= abs(n.getDisp[0])
    if(xDisp>mxDisp):
        mxDisp= xDisp
leverArm= e0+mxDisp

# Check results
err= abs(mMax-leverArm*P)

'''
print('mMax= ', mMax/1e3, 'kN m')        
print('mxDisp= ', mxDisp*1e3, 'mm')
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(err)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # #########################################################
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayLoads()
# # oh.displayFEMesh()
# oh.displayDispRot(itemToDisp='uX')
# # oh.displayDispRot(itemToDisp='uY')
# oh.displayIntForcDiag(itemToDisp='N')
# oh.displayIntForcDiag(itemToDisp='Mz')
# oh.displayIntForcDiag(itemToDisp='Vy')
