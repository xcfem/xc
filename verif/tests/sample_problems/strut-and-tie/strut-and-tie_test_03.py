# -*- coding: utf-8 -*-
''' Generation of a simple linear elastic pile cap model that transfer the
    loads from the piles to the piles. This example tries to show how the
    PileCap2Piles object can be used inside a bigger model.

    When analyzing the pile cap itself we create the strut-and-tie model by
    means of the createStrutAndTieModel method. When analyzing other parts
    of the model we use the createDummyElasticModel as a set of rigid bars
    to simulate the pile cap without the complexities of the 
    strut-and-tie model.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from misc_utils import log_messages as lmsg
from solution import predefined_solutions
from model.mesh import strut_and_tie_utils

# Problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Problem geometry.
#    
#                n0
#                 |  pier
#                 |
#                 |
#                 |n1
#                 +
#                / \
#               /   \    d
#              /     \
#             +-------+
#          n6 |       | n9
#             |       | 
#             |       | piles
#             |       |
#          n10         n11
#    
s= 1.6
d= 1.0
pierHeight= 2
pileLength= 5.0
pierSide= 0.25
pileDiameter= 150e-3
v= s/2.0
a= pierSide/2.0

## Define mesh.
### Top of the pier.
n0= modelSpace.newNode(0.0, d+pierHeight) # pier top.
n3= modelSpace.newNode(0.0, d) # pier bottom.
### Bottom of the pile cap.
n6= modelSpace.newNode(-v, 0.0) # left pile top.
n9= modelSpace.newNode(v, 0.0) # right pile top.
### Bottom of the piles.
n10= modelSpace.newNode(-v, -pileLength)
n11= modelSpace.newNode(v, -pileLength)

pileCap= strut_and_tie_utils.PileCap2Piles(modelSpace= modelSpace, pierBottomNode= n3, leftPileTopNode= n6, rightPileTopNode= n9, pierEffectiveWidth= pierSide)

# Define materials. 
concrete= EC2_materials.C30
reinfSteel= EC2_materials.S500B
## Define pier material.
pierRCSection= def_simple_RC_section.RCRectangularSection(name= 'pierRCSection', sectionDescr= 'pier section', concrType= concrete, reinfSteelType= reinfSteel, width= pierSide, depth= pierSide)
xcPierSectionMaterial= pierRCSection.defElasticShearSection2d(preprocessor)

# Define dummy model of the pile cap.
pileCap.createDummyElasticModel(concrete)

### Define pier.
#### Define pier elements.
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(xcPierSectionMaterial)
modelSpace.newElement('ElasticBeam2d', [n0.tag, n3.tag])

### Define piles.
#### Define pile material.
pilesRCSection= def_column_RC_section.RCCircularSection(name= 'pilesRCSection', sectionDescr= 'piles section', concrType= concrete, reinfSteelType= reinfSteel, Rext= pileDiameter/2.0)
pilesMaterial= pilesRCSection.defElasticShearSection2d(preprocessor, reductionFactor= 1.0)
modelSpace.setDefaultMaterial(pilesMaterial)
modelSpace.newElement('ElasticBeam2d', [n6.tag, n10.tag])
modelSpace.newElement('ElasticBeam2d', [n9.tag, n11.tag])

## Define constraints
modelSpace.fixNode('000', n10.tag)
modelSpace.fixNode('000', n11.tag)

# Load definition.
F= 1.5*183e3
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n0.tag,xc.Vector([0,-F,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= False)
if(result!=0):
    lmsg.error("Can't solve.")
    info= None
    solver= modelSpace.analysis.linearSOE.solver
    if(solver.hasProp("info")):
        info= solver.getProp("info")
    unconstrainedNode= modelSpace.locateEquationNumber(eqNumber= info-1)
    print('unconstrained node id: ', unconstrainedNode.tag)
    print('unconstrained node position: ', unconstrainedNode.getInitialPos2d)
    exit(1)
stressesOK= pileCap.checkStressesSign()
if(not stressesOK):
    exit(1)

# Check results.
modelSpace.calculateNodalReactions()
R0a= n10.getReaction
R0b= n11.getReaction
ratio1= abs(R0a[1]+R0b[1]-F)/F
ratio2= abs(R0a[0]+R0b[0])
R0= R0a+R0b

'''
print('\nR0a= ', R0a*1/1e3, 'kN')
print('R0b= ', R0b*1/1e3, 'kN')
print('R0= ', R0*1/1e3, 'kN')
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
'''
testOK= stressesOK and (abs(ratio1)<1e-8) and (abs(ratio2)<1e-8)

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
# # oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayIntForcDiag('N')
# # oh.displayDispRot(itemToDisp='uY')

