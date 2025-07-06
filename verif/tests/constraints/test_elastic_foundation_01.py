# -*- coding: utf-8 -*-
''' Check ElasticFoundation class. Test based on equilibrium considerations.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from scipy.constants import g
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.ec2 import EC2_materials
from model.boundary_cond import spring_bound_cond

# Problem geometry
foundationWidth= 10.0
foundationLength= 10.0
elementSize= 1.0

# Problem type
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

pt0= modelSpace.newKPoint(-foundationLength/2.0, -foundationWidth/2.0, 0)
pt1= modelSpace.newKPoint(foundationLength/2.0, -foundationWidth/2.0, 0)
pt2= modelSpace.newKPoint(foundationLength/2.0, foundationWidth/2.0, 0)
pt3= modelSpace.newKPoint(-foundationLength/2.0, foundationWidth/2.0, 0)

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
elasticFoundation= spring_bound_cond.ElasticFoundation(wModulus= 10e6, cRoz= 0.01)

elasticFoundation.generateSprings(xcSet= xcTotalSet)

foundationSet= modelSpace.defSet(setName= 'foundationSet', surfaces= [slab])
foundationSet.fillDownwards()

# Create loads.
## Self weight.
gravityVector= xc.Vector([0.0,0.0,g])
cLC= modelSpace.newLoadPattern(name= 'G', setCurrent= True)
cLC.description= 'Self weight'
loadedElements= modelSpace.createSelfWeightLoad(xcSet= foundationSet, gravityVector= gravityVector)
modelSpace.addLoadCaseToDomain(cLC.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

# The tributary areas are also computed during the analysis. It will be useful
# to investigate where and why. Uncomment the next loop to see them.
# for n in foundationSet.nodes:
#     print(n.tag, 'D tributary area a= ', n.getTributaryArea())

maxPressure, maxPressureNode, minPressure, minPressureNode, avgPressure= elasticFoundation.getMaxMinAvgPressure(component= 2)

refValue= 2500*0.6*g
ratio1= abs(maxPressure-refValue)/refValue
ratio2= abs(minPressure-refValue)/refValue
ratio3= abs(avgPressure-refValue)/refValue

'''
print('refValue= ', refValue/1e3, 'kPa')
print('maxPressure= ', maxPressure/1e3, 'kPa', ratio1)
print('minPressure= ', minPressure/1e3, 'kPa', ratio2)
print('avgPressure= ', avgPressure/1e3, 'kPa', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) & (abs(ratio2)<1e-6) & (abs(ratio3)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# # oh.displayLocalAxes()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)

