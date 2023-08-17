# -*- coding: utf-8 -*-
''' Check that adding an element to an existing "hot" model: a model that has
    been already analyzed gives the correct results. Check 2D elastic beam element.
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
M= 5*F # Bending moment (pounds inches)
A= 1 # Section area in square inches.
I= 1/12.0 # Section inertia in inches^4.

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Mesh
### Nodes.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0.0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

### Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A= A, E= E, I= I)

### Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
beamA= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
beamA.sectionArea= A
beamB= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
beamB.sectionArea= A

### Constraints
modelSpace.fixNode("00F", n1.tag)
modelSpace.fixNode("F0F", n2.tag)

# Deactivate beamB element.
beamBSet= modelSpace.defSet(elements=[beamB])
modelSpace.deactivateElements(beamBSet, freezeDeadNodes= False)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([F,0,M]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

## Compute solution
solProc= predefined_solutions.PlainNewtonRaphson(feProblem, printFlag= 0)
result= solProc.solve(calculateNodalReactions= True)

# Check results.
R1a= n1.getReaction[0]
ratio0= abs(R1a+F)/F
N1Aa= beamA.getN()
M1Aa= beamA.getM2 # Bending moment at the frontal node.
N1Ba= beamB.getN()
M1Ba= beamB.getM2 # Bending moment at the frontal node.
ratio1= abs(N1Aa-F)/F+abs(M1Aa-M)/M+abs(N1Ba)/F+abs(M1Ba)/M
Ux1a= n2.getDisp[0]
Ux1aRef= F*l/E/A
ratio2= abs(Ux1a-Ux1aRef)/Ux1aRef

## Activate the second element (nothing must change).
modelSpace.activateElements(beamBSet)

# Solve again.
result= solProc.solve(calculateNodalReactions= True)

# Check results.
R1b= n1.getReaction[0] # Horizontal reaction.
ratio3= abs(R1b+F)/F
N1Ab= beamA.getN() # Axial force in the first element.
M1Ab= beamA.getM2 # Bending moment at the frontal node.
ratio4= abs(N1Ab-F)/F+abs(M1Ab-M)/M
N1Bb= beamB.getN() # Axial force in the awakened element.
M1Bb= beamB.getM2 # Bending moment at the frontal node.
ratio5= abs(N1Bb)/F+abs(M1Bb)/M # No axial force nor bending moment in the awakened element.
Ux1b= n2.getDisp[0] # Horizontal displacement.
Ux1bRef= Ux1aRef # No additional displacement.
ratio6= abs(Ux1b-Ux1bRef)/Ux1bRef

# Define a new load.
lp1= modelSpace.newLoadPattern(name= '1')
lp1.newNodalLoad(n2.tag,xc.Vector([F,0,M]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp1.name)

## Solve again.
result= solProc.solve(calculateNodalReactions= True)

# Check results.
R1c= n1.getReaction[0] # Horizontal reaction.
ratio7= abs(R1c+2*F)/F/2
N1Ac= beamA.getN() # Axial force in the first element.
M1Ac= beamA.getM2 # Bending moment at the frontal node.
ratio8= abs(N1Ac-1.5*F)/F/1.5+abs(M1Ac-1.5*M)/M/1.5
N1Bc= beamB.getN() # Axial force in the awakened element.
M1Bc= beamB.getM2 # Bending moment at the frontal node.
ratio9= abs(N1Bc-0.5*F)/F/0.5+abs(M1Bc-0.5*M)/M/0.5 # No axial force nor bending moment in the awakened element.
Ux1c= n2.getDisp[0] # Horizontal displacement.
Ux1cRef= 1.5*Ux1bRef # 1.5 times the previous displacement.
ratio10= abs(Ux1b-Ux1bRef)/Ux1bRef

'''
print('R1a= ', R1a, ' ratio0= ', ratio0)
print(beamA.tag, 'N1Aa= ', N1Aa, 'M1Aa= ', M1Aa, ' ratio1= ', ratio1)
print(beamB.tag, 'N1Ba= ', N1Bb, 'M1Bb= ', M1Ba, ' ratio1= ', ratio1)
print('Ux1a= ', Ux1a, ' ratio2= ', ratio2)
print('R1b= ', R1b, ' ratio3= ', ratio3)
print(beamA.tag, 'N1Ab= ', N1Ab, 'M1Ab= ', M1Ab, ' ratio4= ', ratio4)
print(beamB.tag, 'N1Bb= ', N1Bb, 'M1Bb= ', M1Bb, ' ratio5= ', ratio5)
print('Ux1b= ', Ux1b, ' ratio6= ', ratio6)
print('R1c= ', R1c, ' ratio7= ', ratio7)
print(beamA.tag, 'N1Ac= ', N1Ac, 'M1Ac= ', M1Ac, ' ratio8= ', ratio8)
print(beamB.tag, 'N1Bc= ', N1Bc, 'M1Bc= ', M1Bc, ' ratio9= ', ratio9)
print('Ux1c= ', Ux1c, ' ratio10= ', ratio10)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-8) &(abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-8) & (abs(ratio4)<1e-5) & (abs(ratio5)<1e-5) & (abs(ratio6)<1e-5) & (abs(ratio7)<1e-8) & (abs(ratio8)<1e-5) & (abs(ratio9)<1e-5) & (abs(ratio10)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
