# -*- coding: utf-8 -*-
'''Check strain load on brick elements.'''

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

# Mesh data.
## Node positions
r1= 1.0
r2= 2.0
ndiv= 72
angIncr= math.radians(360/ndiv)
rList=[1, 1.25, 1.5, 1.75, 2.0]
angList=[i*angIncr for i in range(ndiv)]
zList=[0.0, 0.25, 0.5, 0.75, 1.0]

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
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25, rho= 0.0)
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
brickTags= list()
elementHandler= preprocessor.getElementHandler
elementHandler.defaultMaterial= elast3d.name
for hexa in hexahedra:
    n0= nodes[hexa[0]]; n1= nodes[hexa[1]]; n2= nodes[hexa[2]]
    n3= nodes[hexa[3]]; n4= nodes[hexa[4]]; n5= nodes[hexa[5]]
    n6= nodes[hexa[6]]; n7= nodes[hexa[7]];
    brick= elementHandler.newElement("Brick",xc.ID([n0.tag, n1.tag, n2.tag, n3.tag, n4.tag, n5.tag, n6.tag, n7.tag]))
    bricks.append(brick)
    brickTags.append(brick.tag)

# Constraints.
for n in fixedNodes:
    modelSpace.fixNode('000', n.tag)

xcTotalSet= modelSpace.getTotalSet()

# Load case definition.
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
AT= 10.0 # Temperature increment (Celsius degrees)
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("brick_strain_load")
eleLoad.elementTags= xc.ID(brickTags)
for gaussPoint in range(0, 8):
    for gdl in range(0, 3):
        eleLoad.setStrainComp(gaussPoint,gdl,alpha*AT) #(id of Gauss point, id of component, value)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

# Check results.
## Check reactions (no external forces => no reactions).
R0= 0
R1= 0
R2= 0
for n in fixedNodes:
    R= n.getReaction
    R0+= R[0]
    R1+= R[1]
    R2+= R[2]

testOK= (abs(R0)<1e-10) and (abs(R1)<1e-10) and (abs(R1)<1e-10)

## Check displacements.
allNodes= xcTotalSet.nodes
node0Disp= allNodes[0].getDisp
uXMax= node0Disp[0]
uXMin= uXMax
uYMax= node0Disp[1]
uYMin= uYMax
uZMax= node0Disp[2]
uZMin= uZMax

for n in xcTotalSet.nodes:
    disp= n.getDisp
    uXMax= max(disp[0], uXMax)
    uXMin= min(disp[0], uXMin)
    uYMax= max(disp[1], uYMax)
    uYMin= min(disp[1], uYMin)
    uZMax= max(disp[2], uZMax)
    uZMin= min(disp[2], uZMin)

err= math.sqrt((uXMax-0.1479981545344665e-3)**2+(uXMin+0.18828260695015306e-3)**2+(uYMax-0.1784504383693609e-3)**2+(uYMin+0.17670398773247603e-3)**2+(uZMax-0.19960366222876175e-3)**2+(uZMin+0.015203306021049667e-3)**2)

testOK= testOK and (err<1e-6)

    
'''
print('R0= ', R0)
print('R1= ', R1)
print('R2= ', R2)
print('uXMax= '+str(uXMax*1e3)+' mm')
print('uXMin= '+str(uXMin*1e3)+' mm')
print('uYMax= '+str(uYMax*1e3)+' mm')
print('uYMin= '+str(uYMin*1e3)+' mm')
print('uZMax= '+str(uZMax*1e3)+' mm')
print('uZMin= '+str(uZMin*1e3)+' mm')
print('err= ', err)
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
# ## Uncomment to display the mesh
# oh.displayFEMesh()
# for strainLabel in ['epsilon_xx', 'epsilon_yy', 'epsilon_zz', 'epsilon_xy', 'epsilon_xz', 'epsilon_yz']:
#     oh.displayLoads(elLoadComp= strainLabel)
# oh.displayReactions()
# # oh.displayLoads()
# oh.displayDispRot(itemToDisp='uX', defFScale= 1000.0)
# oh.displayDispRot(itemToDisp='uY', defFScale= 1000.0)
# oh.displayDispRot(itemToDisp='uZ', defFScale= 1000.0)
