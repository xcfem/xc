# -*- coding: utf-8 -*-
# Home made test.

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

A= 1.0 # Cross-section area (in2)
E= 30e6 # Young modulus (psi)
l= 10 # Bar length in inches
F= 1000 # Force magnitude

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Mesh
modelSpace= predefined_spaces.SolidMechanics2D(nodes)
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0)

elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

elements= preprocessor.getElementHandler
elements.dimElem= 2 # Bars defined in a two dimensional space.
elements.defaultMaterial= elast.name
trussA= elements.newElement("CorotTruss",xc.ID([n1.tag,n2.tag]))
trussA.sectionArea= A
trussB= elements.newElement("CorotTruss",xc.ID([n1.tag,n2.tag]))
trussB.sectionArea= A

# Constraints
constraints= preprocessor.getBoundaryCondHandler
# Constrain the displacement of node 1.
spc1= constraints.newSPConstraint(n1.tag,0,0.0)
spc2= constraints.newSPConstraint(n1.tag,1,0.0)
# Constrain the displacement of node 2 in Y axis (gdl 1).
spc5= constraints.newSPConstraint(n2.tag,1,0.0)


## Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([F,0]))

## Add the load pattern to the domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
solProc= predefined_solutions.PenaltyNewtonLineSearch(feProblem, printFlag= 0, convergenceTestTol= 1e-7, maxNumIter= 6, convTestType= 'norm_unbalance_conv_test')
analOk= solProc.solve(calculateNodalReactions= True, reactionCheckTolerance= 1e-10)
if(analOk!=0):
    lmsg.error('Failed to solve for: '+lp0.name)
    quit()

# Results
ux_I= n2.getDisp[0]
ux_refI= F*l/(E*2*A) # Both elements are active
R1_I= n1.getReaction[0]
R2_I= n2.getReaction[0]
NA_I= trussA.getN1
NB_I= trussB.getN1
halfF= F/2.0

ratio0= (ux_I-ux_refI)/ux_refI
ratio1= (R1_I+F)/F
ratio2= (R2_I)
ratio3= (NA_I-halfF)/halfF
ratio4= (NB_I-halfF)/halfF

toKill= preprocessor.getSets.defSet("to_kill")
toKill.getElements.append(trussB)
modelSpace.deactivateElements(toKill)

# Solve with dead element.
analOk= solProc.solve(calculateNodalReactions= True, reactionCheckTolerance= 1e-10)
if(analOk!=0):
    lmsg.error('Failed to solve with dead element.')
    quit()

ux_II= n2.getDisp[0]
ux_refII=2.0*ux_refI # Only one active element
R1_II= n1.getReaction[0]
R2_II= n2.getReaction[0]
NA_II= trussA.getN1
NB_II= trussB.getN1

ratio5= (ux_II-ux_refII)/ux_refII
ratio6= (R1_II+F)/F
ratio7= (R2_II)/F
ratio8= (NA_II-F)/F
ratio9= (NB_II)/F

ratios= [ratio0,ratio1,ratio2,ratio3,ratio4,ratio5,ratio6,ratio7,ratio8,ratio9]

error= 0.0
for r in ratios:
    error+= r**2
error= math.sqrt(error)

'''
print("ux_I= ",ux_I)
print("ux_refI= ",ux_refI)
print("ratio0= ",ratio0)
print("R1_I= ",R1_I)
print("R2_I= ",R2_I)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("NA_I= ",NA_I)
print("NB_I= ",NB_I)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ux_II= ",ux_II)
print("ux_refII= ",ux_refII)
print("ratio5= ",ratio5)
print("R1_II= ",R1_II)
print("R2_II= ",R2_II)
print("ratio6= ",ratio6)
print("ratio7= ",ratio7)
print("NA_II= ",NA_II)
print("ratio8= ",ratio8)
print("NB_II= ",NB_II)
print("ratio9= ",ratio9)
print("error= ",error)
'''

import os
fname= os.path.basename(__file__)
if error<1e-5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


