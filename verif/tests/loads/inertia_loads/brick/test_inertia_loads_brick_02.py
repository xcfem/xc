# -*- coding: utf-8 -*-
'''Check strain load on brick elements.

'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from scipy.constants import g

# Mesh data.
## Node positions
r1= 1.0
r2= 2.0
ndiv= 72
angIncr= math.radians(360/ndiv)
rList=[1, 1.25, 1.5, 1.75, 2.0]
angList=[i*angIncr for i in range(ndiv)]
zList=[0.0, 0.25, 0.5, 0.75, 1.0]

area= math.pi*(r2**2-r1**2)*(1-1/ndiv)
volume= (zList[-1]-zList[0])*area
### Compute node positions.
positions= list()
count= 0
for a in angList:
    ca= math.cos(a)
    sa= math.sin(a)
    radiusPoints= list()
    for r in rList:
        zPoints= list()
        for z in zList:
            x= r*ca; y= r*sa;
            zPoints.append((count, (x,y,z)))
            count+= 1
        radiusPoints.append(zPoints)
    positions.append(radiusPoints)

## Hexahedra
numLayers= len(zList)
numRanges= len(rList)
numAngles= len(angList)
hexahedra= list()
for ia in range(0,numAngles-1):
    for ir in range(0,numRanges-1):
        for iz in range(0, numLayers-1):
            idx= list()
            index0= (ia, ir, iz)
            index1= (ia, ir+1, iz)
            index2= (ia+1, ir+1, iz)
            index3= (ia+1, ir, iz)
            
            index4= (ia, ir, iz+1)
            index5= (ia, ir+1, iz+1)
            index6= (ia+1, ir+1, iz+1)
            index7= (ia+1, ir, iz+1)
            idx= (index0, index1, index2, index3,
                  index4, index5, index6, index7)
            p= list()
            for i in idx:
                item= positions[i[0]][i[1]][i[2]]
                count= item[0]
                p.append(count)
            hexahedra.append(p) 
# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Material definition
rho= 2500
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25, rho= rho)
# Mesh
## Nodes.
nodeHandler= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodeHandler)
nodes= dict()
fixedNodes= list()
for layer in positions:
    for row in layer:
        for item in row:
            count= item[0]
            pos= item[1]
            newNode= nodeHandler.newNodeXYZ(pos[0], pos[1], pos[2])
            nodes[count]= newNode
            if(abs(pos[2])<1e-3):
                fixedNodes.append(newNode)

## Elements.
bricks= list()
elementHandler= preprocessor.getElementHandler
elementHandler.defaultMaterial= elast3d.name
for hexa in hexahedra:
    n0= nodes[hexa[0]]; n1= nodes[hexa[1]]; n2= nodes[hexa[2]]
    n3= nodes[hexa[3]]; n4= nodes[hexa[4]]; n5= nodes[hexa[5]]
    n6= nodes[hexa[6]]; n7= nodes[hexa[7]];
    brick= elementHandler.newElement("Brick",xc.ID([n0.tag, n1.tag, n2.tag, n3.tag, n4.tag, n5.tag, n6.tag, n7.tag]))
    bricks.append(brick)

# Constraints.
for n in fixedNodes:
    modelSpace.fixNode('000', n.tag)

## Whole model mass data.
xcTotalSet= modelSpace.getTotalSet()
massZ= xcTotalSet.getTotalMassComponent(2)
massRefZ= rho*volume
ratio1= abs(massZ-massRefZ)/massRefZ
testOK= abs(ratio1)<1e-2


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
accel= xc.Vector([0,0,g])
xcTotalSet= modelSpace.getTotalSet()
modelSpace.createSelfWeightLoad(xcTotalSet, accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

# Check "horizontal" and "vertical" reactions.
R0= 0
R1= 0
vReac= list()
for n in fixedNodes:
    R= n.getReaction
    R0+= R[0]
    R1+= R[1]
    vReac.append(R[2])
R2= sum(vReac)
weightRefZ= massRefZ*g
ratio2= abs(R2-weightRefZ)/weightRefZ
testOK= testOK and (abs(R0)<1e-6)
testOK= testOK and (abs(R1)<1e-6)
testOK= testOK and (abs(ratio2)<1e-2)

'''
print('ratio1= ', ratio1)
print('R0= ', R0)
print('R1= ', R1)
print('R2= ', R2/1e3)
print('weightRefZ= ', weightRefZ/1e3)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# #oh.displayLocalAxes()
# #oh.displayReactions()
