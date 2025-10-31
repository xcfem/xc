# -*- coding: utf-8 -*-
''' Analysis of a foundation slab.

Test inspired on the example shown in: 
https://www.structuremag.org/article/correlation-between-soil-bearing-capacity-and-modulus-of-subgrade-reaction/
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.ec2 import EC2_materials
from model.boundary_cond import spring_bound_cond

# Problem geometry
foundationWidth= 5.0
foundationLength= 5.0
elementSize= 0.5

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
slabDepth= 0.55 # thickness.
slabMaterial= concrete.defElasticMembranePlateSection(preprocessor= preprocessor, name= "slabMaterial", thickness= slabDepth)
## Generate mesh.
modelSpace.setDefaultMaterial(slabMaterial)
modelSpace.newSeedElement('ShellMITC4')
xcTotalSet.genMesh(xc.meshDir.I)

## Boundary conditions.
elasticFoundation= spring_bound_cond.ElasticFoundation(wModulus= 10.858e6, cRoz= 0.01)

elasticFoundation.generateSprings(xcSet= xcTotalSet)

# Define loads.
F= 1700e3
loadedNode= slab.getNearestNode(geom.Pos3d(0,0,0))
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(loadedNode.tag,xc.Vector([0,0,-F,0,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Test nodes.
test_nodes= list()
x0= -foundationLength/2.0
xi= [x0]
for i in range(0, 5):
    x0+= elementSize
    xi.append(x0)
for x in xi:
    test_node= slab.getNearestNode(geom.Pos3d(x,x,0))
    test_nodes.append(test_node)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

# Check results.
svd= elasticFoundation.calcPressures() # Compute pressures.

uZi= list() # Vertical displacements.
supportingNodes= list() # Attached nodes.
pi= list() # pressures.
for test_node in test_nodes:
    uZi.append(test_node.getDisp[2])
    supportingNodes.append(elasticFoundation.getSupportingNode(test_node).tag)
    pi.append(test_node.getProp('soilPressure'))

uZiRef= [-0.005607304701299697, -0.00588728592715833, -0.006174104114979693, -0.006466132034458766, -0.006749310460262539, -0.006986261708320184]
supportingNodesRef= [121, 161, 171, 181, 191, 201]
piRef= [[0.0, 0.0, 60884.11440539463], [0.0, 0.0, 63924.15042356304], [0.0, 0.0, 67038.42229847371], [0.0, 0.0, 70209.26143957024], [0.0, 0.0, 73284.01277860119], [0.0, 0.0, 75856.8294230272]]

errorU= 0
for uZ, uZRef in zip(uZi, uZiRef):
    errorU+= (uZ-uZRef)**2
errorU= math.sqrt(errorU)

errorP= 0
for p, pRef in zip(pi, piRef):
    vP= xc.Vector(p)
    vPRef= xc.Vector(pRef)
    errorP+= (vP-vPRef).Norm2()
errorP= math.sqrt(errorP)

errorF= (svd.getResultant()-geom.Vector3d(0,0,F)).getModulus()
errorM= svd.getMoment(geom.Pos3d(0,0,0)).getModulus()


testOK= (errorU<1e-8) and (supportingNodes==supportingNodesRef) and (errorP<1e-7) and (errorF<1e-5) and (errorM<1e-5)


'''
print('uZi= ', uZi)
print('errorU= ', errorU)
print('supportingNodes= ', supportingNodes)
print('pi= ', pi)
print('errorP= ', errorP)
print('svd= ', svd)
print('errorF= ', errorF)
print('errorM= ', errorM)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic output.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks(displayLocalAxes= True)
# # oh.displayFEMesh()
# # oh.displayLoads()
# # oh.displayReactions()
# oh.displayDispRot(itemToDisp='uZ')
