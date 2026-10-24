# -*- coding: utf-8 -*-
''' Check ShallowInelasticFoundation class. Test based on equilibrium considerations.

The problem with the friction materials in this model is that the friction force is independent of the normal reaction in the contact, and that does not correspond to the physical reality.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import math
import geom
from scipy.constants import g
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.ec2 import EC2_materials
from model.boundary_cond import shallow_inelastic_foundation as sif
from misc_utils import log_messages as lmsg

# Problem geometry
foundationWidth= 10.0
foundationLength= 10.0
elementSize= 1.0

# Problem type
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

halfLength= foundationLength/2.0
halfWidth= foundationWidth/2.0
pt0= modelSpace.newKPoint(-halfLength, -halfWidth, 0)
pt1= modelSpace.newKPoint(halfLength, -halfWidth, 0)
pt2= modelSpace.newKPoint(halfLength, halfWidth, 0)
pt3= modelSpace.newKPoint(-halfLength, halfWidth, 0)

slab= modelSpace.newQuadSurface(pt0, pt1, pt2, pt3)
slab.setElemSizeIJ(elementSize, elementSize)

## Mesh generation.
xcTotalSet= modelSpace.getTotalSet()
xcTotalSet.conciliaNDivs()
### Define materials.
concrete= EC2_materials.C30
slabDepth= 0.60 # thickness.
slabMaterial= concrete.defElasticMembranePlateSection(preprocessor= preprocessor, name= "slabMaterial", thickness= slabDepth)
## Generate mesh.
modelSpace.setDefaultMaterial(slabMaterial)
modelSpace.newSeedElement('ShellMITC4')
xcTotalSet.genMesh(xc.meshDir.I)

## Boundary conditions.
wModulus= 1e8 # Winkler modulus of the foundation F/L^3
soilContactPressure= 200e3 # 2 kp/cm2= 200 kN/m2
soilPhi= math.radians(20)
frictionYieldStress= soilContactPressure*math.tan(soilPhi)
foundation= sif.ShallowInelasticFoundation(wModulus= wModulus, frictionYieldStress= frictionYieldStress)
foundation.generateSprings(xcSet= xcTotalSet)

foundationSet= modelSpace.defSet(setName= 'foundationSet', surfaces= [slab])
foundationSet.fillDownwards()

# Create loads.
## Self weight.
gravityVector= xc.Vector([0.0,0.0,g])
cLC= modelSpace.newLoadPattern(name= 'G', setCurrent= True)
cLC.description= 'Self weight'
loadedElements= modelSpace.createSelfWeightLoad(xcSet= foundationSet, gravityVector= gravityVector)
modelSpace.addLoadCaseToDomain(cLC.name)
## Horizontal load.
cLC= modelSpace.newLoadPattern(name= 'Q', setCurrent= True)
cLC.description= 'Horizontal load'
foundationArea= slab.getArea()
horizontalForce= 1.999*foundationArea*frictionYieldStress
loadVector= xc.Vector([horizontalForce/2.0, horizontalForce/2.0, 0.0, 0.0, 0.0, 0.0])
loadedNode= slab.getNearestNode(geom.Pos3d(0,0,0))
cLC.newNodalLoad(loadedNode.tag, loadVector)
modelSpace.addLoadCaseToDomain(cLC.name)
    
# Solution
analysis= predefined_solutions.plain_newton_raphson(feProblem, maxNumIter= 10, convergenceTestTol= 1e-5, printFlag= 0)
result= analysis.analyze(1)
if(result!=0):
    lmsg.error("Can't solve.")
    sys.exit(1)

# The tributary areas are also computed during the analysis. It will be useful
# to investigate where and why. Uncomment the next loop to see them.
# for n in foundationSet.nodes:
#     print(n.tag, 'D tributary area a= ', n.getTributaryArea())

maxPressure, maxPressureNode, minPressure, minPressureNode, avgPressure= foundation.getMaxMinAvgPressure(component= 2)

# Check reactions.
refValue= 2500*0.6*g
ratio1= abs(maxPressure-refValue)/refValue
ratio2= abs(minPressure-refValue)/refValue
ratio3= abs(avgPressure-refValue)/refValue

modelSpace.calculateNodalReactions()
reaction= foundation.getReaction()
Rx= reaction[0]
refRx= -horizontalForce/2.0
ratio4= abs(Rx-refRx)/refRx
Ry= reaction[1]
refRy= -horizontalForce/2.0
ratio5= abs(Ry-refRy)/refRy
Rz= reaction[2]
refRz= refValue*foundationWidth*foundationLength
ratio6= abs(Rz-refRz)/refRz

testOK= (abs(ratio1)<1e-6) & (abs(ratio2)<1e-6) & (abs(ratio3)<1e-6) & (abs(ratio4)<1e-6) & (abs(ratio5)<1e-6) & (abs(ratio6)<1e-6)

# Check displacements.
displacement= loadedNode.getDisp
tangentStiffness= foundation.hModulusFraction*foundation.wModulus*foundationArea
residualTangent= 0.0
for nodeTag in foundation.springs:
    springElement= foundation.springs[nodeTag]
    xMaterial= springElement.getMaterials()[0]
    residualTangent+= xMaterial.getTangent()

Ux= displacement[0]
elasticUx= -refRx/tangentStiffness
dragUx= Ux-elasticUx
Uy= displacement[0]
elasticUy= -refRy/tangentStiffness
dragUy= Uy-elasticUy

testOK&= (dragUx>0) and (dragUy>0) and (residualTangent<0.25*tangentStiffness)

'''
print('refValue= ', refValue/1e3, 'kPa')
print('maxPressure= ', maxPressure/1e3, 'kPa', ratio1)
print('minPressure= ', minPressure/1e3, 'kPa', ratio2)
print('avgPressure= ', avgPressure/1e3, 'kPa', ratio3)
print('Rx= ', Rx, refRx, ratio4)
print('Ry= ', Ry, refRy, ratio5)
print('Rz= ', Rz, refRz, ratio6)
print('Ux= ', Ux, elasticUx, dragUx)
print('Uy= ', Uy, elasticUy, dragUy)
print('initial tangent stiffness: ', tangentStiffness/1e6)
print('residual tangent stiffness: ', residualTangent/1e6)
'''

import os
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLocalAxes()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)

