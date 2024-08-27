# -*- coding: utf-8 -*-
''' Check that adding an element to an existing "hot" model: a model that has
    been already analyzed gives the correct results. Check Truss element.
'''

from __future__ import print_function

import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


E= 30e6 # Young modulus (psi)
l= 10 # Bar length in inches
F= 1000 # Force magnitude (pounds)
A= 1 # Section area in square inches.

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

## Mesh
### Nodes.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0.0)

### Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
    
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
### Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast.name
elements.dimElem= 2 # Dimension of element space
trussA= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
trussA.sectionArea= A
trussB= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
trussB.sectionArea= A

### Constraints
modelSpace.fixNode("00", n1.tag)
modelSpace.fixNode("F0", n2.tag)

# Deactivate trussB element.
trussBSet= modelSpace.defSet(elements=[trussB])
modelSpace.deactivateElements(trussBSet, freezeDeadNodes= False)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([F,0]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

## Compute solution
solProc= predefined_solutions.PlainNewtonRaphson(feProblem, printFlag= 0)
result= solProc.solve(calculateNodalReactions= True, reactionCheckTolerance= 1e-6)
## Check results.
R1a= n1.getReaction[0]
ratio0= abs(R1a+F)/F
N1a= trussA.getN()
ratio1= abs(N1a-F)/F
Ux1a= n2.getDisp[0]
Ux1aRef= F*l/E/A
ratio2= abs(Ux1a-Ux1aRef)/Ux1aRef

## Activate the second element (nothing must change).
modelSpace.activateElements(trussBSet)

# Solve again.
result= solProc.solve(calculateNodalReactions= True)

## Check results.
R1b= n1.getReaction[0] # Horizontal reaction.
ratio3= abs(R1b+F)/F
N1Ab= trussA.getN() # Axial force in the first element.
ratio4= abs(N1Ab-F)/F
N1Bb= trussB.getN() # Axial force in the awakened element.
ratio5= abs(N1Bb)/F # No axial force in the awakened element.
Ux1b= n2.getDisp[0] # Horizontal displacement.
Ux1bRef= Ux1aRef # No additional displacement.
ratio6= abs(Ux1b-Ux1bRef)/Ux1bRef

# Define a new load.
lp1= modelSpace.newLoadPattern(name= '1')
lp1.newNodalLoad(n2.tag,xc.Vector([F,0]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp1.name)


# Solve again.
result= solProc.solve(calculateNodalReactions= True)

## Check results.
R1c= n1.getReaction[0] # Horizontal reaction.
ratio7= abs(R1c+2*F)/2/F
N1Ac= trussA.getN() # Axial force in the first element.
ratio8= abs(N1Ac-F*1.5)/1.5/F # Three quarters of the total load.
N1Bc= trussB.getN() # Axial force in the awakened element.
ratio9= abs(N1Bc-0.5*F)/0.5/F # No axial force in the awakened element.
Ux1c= n2.getDisp[0] # Horizontal displacement.
Ux1cRef= Ux1bRef+F*l/E/A/2 # 1.5 times the previous displacement.
ratio10= abs(Ux1c-Ux1cRef)/Ux1cRef


'''
print('R1a= ', R1a, ' ratio0= ', ratio0)
print('N1a= ', N1a, ' ratio1= ', ratio1)
print('Ux1a= ', Ux1a, ' ratio2= ', ratio2)
print('R1b= ', R1b, ' ratio3= ', ratio3)
print('N1Ab= ', N1Ab, ' ratio4= ', ratio4)
print('N1Bb= ', N1Bb, ' ratio5= ', ratio5)
print('Ux1b= ', Ux1b, ' ratio6= ', ratio6)
print('R1c= ', R1c, ' ratio7= ', ratio7)
print('N1Ac= ', N1Ac, ' ratio8= ', ratio8)
print('N1Bc= ', N1Bc, ' ratio9= ', ratio9)
print('Ux1c= ', Ux1c, ' ratio10= ', ratio10)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-5) &(abs(ratio1)<1e-8) & (abs(ratio2)<1e-8) & (abs(ratio3)<1e-8) & (abs(ratio4)<1e-8) & (abs(ratio5)<1e-8) & (abs(ratio6)<1e-8) & (abs(ratio7)<1e-8)& (abs(ratio8)<1e-8) & (abs(ratio9)<1e-8) & (abs(ratio10)<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
