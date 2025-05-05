# -*- coding: utf-8 -*-
''' Check NLDKGQ shell elements update after a "manual" change in the position 
of the nodes.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
import os
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

# Materials definition
E= 2.1e9 # Steel Young's modulus [MPa].
nu= 0.3 # Poisson's ratio.
h= 0.1 # thickness.
dens= 7850 # specific mass [kg/m3].

## Problem geometry
height= 10.0 # column height.
width= 1.0 # column width.

# Finite element model.
## Problem type
steelColumn= xc.FEProblem()
steelColumn.title= 'Test geometry imperfection 05.'
preprocessor= steelColumn.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Model geometry

### Points.
halfWidth= width/2.0
p0= modelSpace.newKPoint(-halfWidth,0,0)
p1= modelSpace.newKPoint( halfWidth,0,0)
p2= modelSpace.newKPoint( halfWidth,0,height)
p3= modelSpace.newKPoint(-halfWidth,0,height)

### Lines
s1= modelSpace.newQuadSurface(p0,p1, p2, p3)
s1.nDivI= 1
s1.nDivJ= 7

### Material.
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat",E= E, nu= nu, rho= dens, h= h)
### Mesh generation
modelSpace.setDefaultMaterial(shellMat)
elem= modelSpace.newSeedElement("ShellNLDKGQ")

xcTotalSet= preprocessor.getSets.getSet('total')
mesh= xcTotalSet.genMesh(xc.meshDir.I)

### Imperfection
e0= 0.1
for n in xcTotalSet.nodes:
    pos= n.getInitialPos3d
    newY= e0*math.sin(pos.z*math.pi/height)
    pos.y+= newY
    n.setPos(pos)

# Update the coordinate transformations after a change in the position of
# the nodes.
for e in xcTotalSet.elements:
    e.resetNodalCoordinates()

## Replicate the model
replicantNodes= list()
for n in s1.nodes:
    pos= n.getInitialPos3d
    tag= n.tag
    replicantNodes.append((n.tag, [pos.x+5.0, pos.y, pos.z]))
replicantElements= list()
for e in s1.elements:
    nTags= list()
    for n in e.nodes:
        nTags.append(n.tag)
    replicantElements.append((e.tag, nTags))
nodeMap= dict()
for rNode in replicantNodes:
    tag= rNode[0]
    [x, y ,z]= rNode[1]
    newNode= modelSpace.newNode(x, y, z)
    nodeMap[tag]= newNode.tag
newElements= list()
for rElement in replicantElements:
    tag= rElement[0]
    nTags= rElement[1]
    newNodeTags= [nodeMap[i] for i in nTags]
    newElement= modelSpace.newElement("ShellNLDKGQ", newNodeTags)
    newElements.append(newElement)

### Constraints
modelSpace.fixNode('000_FFF', p0.getNode().tag)
modelSpace.fixNode('000_FFF', p1.getNode().tag)
modelSpace.fixNode('00F_FFF', p2.getNode().tag)
modelSpace.fixNode('00F_FFF', p3.getNode().tag)
modelSpace.fixNode('000_FFF', nodeMap[p0.getNode().tag])
modelSpace.fixNode('000_FFF', nodeMap[p1.getNode().tag])
modelSpace.fixNode('00F_FFF', nodeMap[p2.getNode().tag])
modelSpace.fixNode('00F_FFF', nodeMap[p3.getNode().tag])

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
## Point load.
cLC= modelSpace.setCurrentLoadPattern(lp0.name)
P= 10e3
pLoad= xc.Vector([0.0, 0.0, -P/2.0, 0.0, 0.0, 0.0])
loadedNodes= [p2.getNode(), p3.getNode()]
### Loaded nodes in replicant model
loadedNodes+= modelSpace.getNodes(tags= [nodeMap[p2.getNode().tag], nodeMap[p3.getNode().tag]])
for n in loadedNodes:
    n.newLoad(pLoad)
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.plain_newton_raphson(steelColumn, maxNumIter= 20, convergenceTestTol= 1e-6)
result= analysis.analyze(1)

# Compute maximum bending moment.
m2MaxS1= 0.0
for e in s1.elements:
    e.getResistingForce()
    M= abs(e.getMeanInternalForce("m2"))
    if(abs(M)>m2MaxS1):
        m2MaxS1= M
m2MaxR= 0.0        
for e in newElements:
    e.getResistingForce()
    M= abs(e.getMeanInternalForce("m2"))
    if(abs(M)>m2MaxR):
        m2MaxR= M

# Compute maximum bending displacement.
mxDisp= 0.0
for n in xcTotalSet.nodes:
    yDisp= abs(n.getDisp[1])
    if(yDisp>mxDisp):
        mxDisp= yDisp
leverArm= e0+mxDisp # account for second order effects.

# Check results
m2Ref= leverArm*P
ratio1= abs(m2MaxS1-m2MaxR)/m2Ref
ratio2= abs(m2MaxS1-m2Ref)/m2Ref

'''
print('m2MaxS1= ', m2MaxS1/1e3, 'kN m')        
print('m2MaxR= ', m2MaxR/1e3, 'kN m')        
print('m2Ref= ', m2Ref/1e3, 'kN m')        
print('mxDisp= ', mxDisp*1e3, 'mm')
print('leverArm= ', leverArm*1e3, 'mm')
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) and (abs(ratio2)<.05):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # #########################################################
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayLoads()
# oh.displayFEMesh()
# oh.displayDispRot(itemToDisp='uX')
# oh.displayDispRot(itemToDisp='uY')
# oh.displayIntForc(itemToDisp='N1')
# oh.displayIntForc(itemToDisp='M1')
# oh.displayIntForc(itemToDisp='M2')
