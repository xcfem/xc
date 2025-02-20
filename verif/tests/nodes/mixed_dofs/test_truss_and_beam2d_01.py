# -*- coding: utf-8 -*-
''' Test a model mixing a brick and a shell element. The shell element
    expects 6 DOF by node and the brick element needs only 3.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from misc_utils import log_messages as lmsg

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics2D(nodeHandler)

## Define mesh.
### Define nodes.
l= 5 # Bar length
n1= nodeHandler.newNodeXY(0,0)
n2= nodeHandler.newNodeXY(l,0)

### Change the number of DOFs per node.
modelSpace.setNumberOfDOFs(3)
n11= modelSpace.newNodeXY(0,0)
n12= modelSpace.newNodeXY(l,0)

### Define materials.
E= 30E6 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
barWidth= .01
barThickness= barWidth
barArea= barWidth*barThickness
barInertia= 1/12.0*barWidth*barThickness**3

trussMaterial= typical_materials.defElasticMaterial(preprocessor, "trussMaterial", E= E)

### Cross section properties (IPE-80)
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= barArea; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= barInertia
beamMaterial= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "beamMaterial",sectionProperties)

### Define elements.
#### Truss
modelSpace.setDefaultMaterial(trussMaterial) # Set the material for the new element.
modelSpace.setElementDimension(2) # Truss defined in a two-dimensional space.
truss= modelSpace.newElement("Truss",nodeTags= [n1.tag,n2.tag])
truss.sectionArea= barArea
#### 2D beam.
##### Coordinate transformation.
lin= modelSpace.getTransfCooHandler().newLinearCrdTransf2d("lin")
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(beamMaterial)
beam= modelSpace.getElementHandler().newElement("ElasticBeam2d",xc.ID([n11.tag,n12.tag]))


### Define constraints.
modelSpace.fixNode('00', n1.tag)
modelSpace.fixNode('F0', n2.tag)

#### Attach the nodes.
for nA, nB in zip([n1, n2], [n11, n12]):
    modelSpace.newEqualDOF(nA.tag, nB.tag, dofs= xc.ID([0, 1]))


## Define loads.
lp0= modelSpace.newLoadPattern(name= '0')
F= 1e3
lp0.newNodalLoad(n2.tag,xc.Vector([F,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
# The penalty constraint handler gives results that are not very accurate.
# analysis= predefined_solutions.simple_static_linear(feProblem)
# result= analysis.analyze(1)
# The transformation constraint handler gives far more accurate results.
solProc= predefined_solutions.SimpleTransformationStaticLinear(feProblem, printFlag= 0)
result= solProc.solveComb(lp0.name, calculateNodalReactions= True)

if(result!=0):
    lmsg.error("Can't solve.")

modelSpace.calculateNodalReactions()
trussN= truss.getN()
beamN= beam.getN()

dispN2= n2.getDisp
dispN12= n12.getDisp

halfF= F/2.0
ratio1= abs(trussN-halfF)/halfF
ratio2= abs(beamN-halfF)/halfF
ratio3= abs(beamN+trussN-F)/F
err= math.sqrt((dispN2[0]-dispN12[0])**2+(dispN2[1]-dispN12[1])**2+dispN12[2]**2)

'''
print(ratio1)
print(ratio2)
print(ratio3)
print(dispN2)
print(dispN12)
print(err)
'''

import os
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-10) and (abs(ratio2)<1e-10) and (abs(ratio3)<1e-10) and (err<1e-10)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # # Output stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# ## Uncomment to display the mesh
# oh.displayFEMesh()
# oh.displayLoads()
