# -*- coding: utf-8 -*-
''' Symmetry constraint trivial test.

See section 4.2 of the book: Finite Element Procedures Klaus-Jurgen Bathe, 2006
 ISBN 097900490X, 9780979004902.  Page 190 et seq.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
# Define mesh.
# Mesh
#
#     5 +---------+ 8     0: Bottom face; vertices 1,4,3,2. (0,3,2,1)
#      /|        /|       1: Left-side face; vertices 1,2,6,5. (0,1,5,4)
#     / |       / |       2: Front face; vertices 2,3,7,6. (1,2,6,5)
#  6 +---------+7 |       3: Right-side face; vertices 3,4,8,7. (2,3,7,6)
#    |  |      |  |       4: Back face; vertices 1,5,8,4. (0,4,7,3)
#    |1 +------|--+ 4     5: Top face; vertices 5,6,7,8. (4,5,6,7)
#    | /       | /
#    |/        |/
#  2 +---------+ 3
#

## Define nodes.
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(0.1,0,0)
n3= nodes.newNodeXYZ(0.1,0.1,0)
n4= nodes.newNodeXYZ(0,0.1,0)
n5= nodes.newNodeXYZ(0,0,0.1)
n6= nodes.newNodeXYZ(0.1,0,0.1)
n7= nodes.newNodeXYZ(0.1,0.1,0.1)
n8= nodes.newNodeXYZ(0,0.1,0.1)

## Define material.
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",E= 1e4, nu= 0.25, rho= 0.0)
## Define elements.
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name
brick= elements.newElement("Brick",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag,n5.tag,n6.tag,n7.tag,n8.tag]))

# Constraints
modelSpace.fixNode('000', n1.tag)
modelSpace.fixNode('00F', n5.tag)
vertPlaneXZ= geom.Plane3d(n1.getInitialPos3d, n2.getInitialPos3d, n6.getInitialPos3d) # Vertical plane XZ.
# Normally, the symmetry constraints will not be aligned with the axes, but
# this case is an exception to make the checking of the results easier.
symmetry2= modelSpace.constraints.newSymmetryConstraint(n2.tag, vertPlaneXZ)
symmetry6= modelSpace.constraints.newSymmetryConstraint(n6.tag, vertPlaneXZ)
vertPlaneYZ= geom.Plane3d(n1.getInitialPos3d, n4.getInitialPos3d, n8.getInitialPos3d) # Vertical plane YZ.
symmetry4= modelSpace.constraints.newSymmetryConstraint(n4.tag, vertPlaneYZ)
symmetry8= modelSpace.constraints.newSymmetryConstraint(n8.tag, vertPlaneYZ)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
fX= 1e5
fY= 1e5
loadVector= xc.Vector([fX,fY,0])
lp0.newNodalLoad(n3.tag, loadVector)
lp0.newNodalLoad(n7.tag, loadVector)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solve.
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
if(result!=0):
  errMsg= 'Solution algorithm failed.'
  lmsg.error(errMsg)
  exit(1)

# Check results.
## Check displacement results (verify symmetry).
uX_n2= n2.getDisp[0]
uY_n2= n2.getDisp[1]
uX_n4= n4.getDisp[0]
uY_n4= n4.getDisp[1]

uX_n6= n6.getDisp[0]
uY_n6= n6.getDisp[1]
uX_n8= n8.getDisp[0]
uY_n8= n8.getDisp[1]


error= math.sqrt(uX_n4**2+uY_n2**2+(uY_n4-uX_n2)**2+uX_n8**2+uY_n6**2+(uY_n8-uX_n6)**2)

## Check reaction results (verify symmetry).
modelSpace.calculateNodalReactions()

r_n1= n1.getReaction
rX_n1= r_n1[0]
rY_n1= r_n1[1]
r_n2= n2.getReaction
rX_n2= r_n2[0]
rY_n2= r_n2[1]
r_n4= n4.getReaction
rX_n4= r_n4[0]
rY_n4= r_n4[1]

r_n5= n5.getReaction
rX_n5= r_n5[0]
rY_n5= r_n5[1]
r_n6= n6.getReaction
rX_n6= r_n6[0]
rY_n6= r_n6[1]
r_n8= n8.getReaction
rX_n8= r_n8[0]
rY_n8= r_n8[1]

eqX= abs(2*fX+rX_n4+rX_n1+rX_n2+rX_n8+rX_n5+rX_n6)
eqY= abs(2*fY+rY_n4+rY_n1+rY_n2+rY_n8+rY_n5+rY_n6)


'''
print('displacements n2: ', uX_n2, uY_n2)
print('displacements n4: ', uX_n4, uY_n4)
print('displacements n6: ', uX_n6, uY_n6)
print('displacements n8: ', uX_n8, uY_n8)
print(error)
print('reactions n2: ', rX_n2, rY_n2)
print('reactions n4: ', rX_n4, rY_n4)
print('reactions n6: ', rX_n6, rY_n6)
print('reactions n8: ', rX_n8, rY_n8)
print('equlibrium X axis: ', eqX)
print('equlibrium Y axis: ', eqY)
'''

import os
fname= os.path.basename(__file__)
if (abs(error)<1e-10) and (eqX<1e-10) and (eqY<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX')
# oh.displayDispRot(itemToDisp='uY')
