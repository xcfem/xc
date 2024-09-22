# -*- coding: utf-8 -*-
'''Check values of stresses.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials import typical_materials
from misc_utils import log_messages as lmsg

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Material definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",E= 1e6, nu= 0.25, rho= 0.0)
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

## Nodes.
nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(1,0,0)
n3= nodes.newNodeXYZ(1,1,0)
n4= nodes.newNodeXYZ(0,1,0)
n5= nodes.newNodeXYZ(0,0,1)
n6= nodes.newNodeXYZ(1,0,1)
n7= nodes.newNodeXYZ(1,1,1)
n8= nodes.newNodeXYZ(0,1,1)

## Elements.
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name
brick= elements.newElement("Brick",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag,n5.tag,n6.tag,n7.tag,n8.tag]))

constraints= preprocessor.getBoundaryCondHandler
# Constrain the displacement of the nodes in the base.
n1.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
n2.fix(xc.ID([1,2]),xc.Vector([0,0]))
n5.fix(xc.ID([1]),xc.Vector([0]))
n6.fix(xc.ID([1]),xc.Vector([0]))
fixedNodes= [n1, n2, n5, n6]

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')

lp0.newNodalLoad(n3.tag, xc.Vector([0,-1,0]))
lp0.newNodalLoad(n4.tag, xc.Vector([0,-1,0]))
lp0.newNodalLoad(n7.tag, xc.Vector([0,-1,0]))
lp0.newNodalLoad(n8.tag, xc.Vector([0,-1,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")

xcTotalSet= modelSpace.getTotalSet()
modelSpace.computeValuesAtNodes(setToCompute= xcTotalSet, propToDefine= 'stress')
sigma_xx_ref= 0.0
sigma_yy_ref= -4.0
sigma_zz_ref= 0.0
err_xx= 0.0
err_yy= 0.0
err_zz= 0.0
for n in xcTotalSet.nodes:
    stress= n.getProp('stress')
    err_xx+= (sigma_xx_ref-stress[0])**2
    err_yy+= (sigma_yy_ref-stress[1])**2
    err_zz+= (sigma_zz_ref-stress[2])**2
    
err_xx= math.sqrt(err_xx)
err_yy= math.sqrt(err_yy)
err_zz= math.sqrt(err_zz)
testOK= (max(err_xx, err_yy, err_zz)<1e-12)

R= fixedNodes[0].getReaction
for n in fixedNodes[1:]:
    R+= n.getReaction

ratio1= (R-xc.Vector([0,4,0])).Norm()
testOK= testOK and (ratio1<1e-12)

''' 
print("R9= ",R9)
print("R10= ",R10)
print("R11= ",R11)
print("R12= ",R12)
print("R= ",R)
print("ratio1= ",ratio1)
   '''

import os
fname= os.path.basename(__file__)
if(ratio1<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # # Output stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# ## Uncomment to display the mesh
# oh.displayFEMesh()
# oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayStresses('sigma_11')
# oh.displayStresses('sigma_22')
# oh.displayStresses('sigma_33')

