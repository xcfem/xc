# -*- coding: utf-8 -*-
''' Verification of newSPConstraint method of LoadPattern class.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

# Material properties
E= 30e6 # Young modulus (psi)
l= 10 # Bar length in inches
F= 10000 # Force magnitude 1 (pounds)
barArea= .01

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Create nodes.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l/2.0,0)
n3= nodes.newNodeXY(l,0)

# Create material.
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
# Create elements.
elements= preprocessor.getElementHandler
elements.dimElem= 2 # Bars defined ina a two dimensional space.
elements.defaultMaterial= elast.name
trussA= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
trussA.sectionArea= barArea
trussB= elements.newElement("Truss",xc.ID([n2.tag,n3.tag]))
trussB.sectionArea= barArea

# Create constraints.
modelSpace.fixNode('00', n1.tag) # No displacement.
modelSpace.fixNode('F0', n2.tag) # Displacement along x axis.
modelSpace.fixNode('F0', n3.tag) # Displacement along x axis.

# Create load pattern.
lp0= modelSpace.newLoadPattern(name= 'lp0')
lp0.newNodalLoad(n3.tag,xc.Vector([F,0]))
modelSpace.addLoadCaseToDomain(lp0.name) # Add the load pattern to the domain.

# Solution
modelSpace.setSolutionProcedureType(predefined_solutions.PlainNewtonRaphson)
result= modelSpace.analyze(calculateNodalReactions= True)

R1x= n1.getReaction[0]
R2x= n3.getReaction[0]
D2x= n3.getDisp[0]

# Load pattern with single point constraint.
lp1= modelSpace.newLoadPattern(name= 'lp1')
lp1.newSPConstraint(n3.tag,0,0.0) # No more displacement along x in node 3
modelSpace.addLoadCaseToDomain(lp1.name) # Add the load pattern to the domain.

# Solve again.
result= modelSpace.analysis.analyze(1)
modelSpace.calculateNodalReactions()

RR1x= n1.getReaction[0]
RR2x= n3.getReaction[0]
DD2x= n3.getDisp[0]

# Remove the force at node 3.
modelSpace.removeLoadCaseFromDomain(lp0.name)

# Solve again and compute reactions.
result= modelSpace.analysis.analyze(1)
modelSpace.calculateNodalReactions()

RRR1x= n1.getReaction[0]
RRR2x= n3.getReaction[0]
DDD2x= n3.getDisp[0]

# Check values.
theoreticalDisp= F*l/E/barArea
## Initial state.
ratio1= abs(R1x+F)/F
ratio2= abs(R2x)
ratio3= abs(theoreticalDisp-D2x)/theoreticalDisp
## Fix node 3.
ratio4= abs(RR1x+F)/F
ratio5= abs(RR2x)
ratio6= abs(theoreticalDisp-DD2x)/theoreticalDisp
## Remove load on node 3
ratio7= abs(RRR1x+F)/F
ratio8= abs(RRR2x-F)/F # Now the constraint receives the tension of the bar.
ratio9= abs(theoreticalDisp-DD2x)/theoreticalDisp

'''
print('R1x= ', R1x)
print('R2x= ', R2x)
print('D2x= ', D2x)
print('RR1x= ', RR1x)
print('RR2x= ', RR2x)
print('DD2x= ', DD2x)
print('RRR1x= ', RRR1x)
print('RRR2x= ', RRR2x)
print('DDD2x= ', DDD2x)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-9) & (abs(ratio2)<1e-9) & (abs(ratio3)<1e-9) & (abs(ratio4)<1e-9) & (abs(ratio5)<1e-9) & (abs(ratio6)<1e-9)& (abs(ratio7)<1e-9) & (abs(ratio8)<1e-9) & (abs(ratio9)<1e-9):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
