# -*- coding: utf-8 -*-
''' Home made test. Check newGlueNodeToElement function.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

E= 30e6 # Young modulus (psi)
l= 10 # Bar length in inches
F= 1000.0
A= .1 # section area (in**2)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Define nodes.
n1= modelSpace.newNode(0,5)
n2= modelSpace.newNode(l,5)
nA= modelSpace.newNode(l/2.0,5) # node to be glued.

n11= modelSpace.newNode(0,0)
n1A= modelSpace.newNode(l/2.0,0) 
n12= modelSpace.newNode(l,0)

n2Nodes= list()
numParts= 81
for i in range(0, numParts+1):
    x= float(i/numParts)*l
    newNode= modelSpace.newNode(x,-5)
    n2Nodes.append(newNode)
n2A= modelSpace.newNode(l/2.0,-5) # node to be glued.

# Define materials.
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

# Define elements.
modelSpace.setElementDimension(2)
modelSpace.setDefaultMaterial(elast)

# First bar.
truss1= modelSpace.newElement("Truss", [n1.tag,n2.tag])
truss1.sectionArea= A
# Second bar.
truss11= modelSpace.newElement("Truss", [n11.tag,n1A.tag])
truss11.sectionArea= A
truss12= modelSpace.newElement("Truss", [n1A.tag,n12.tag])
truss12.sectionArea= A
# Third bar.
n2Trusses= list()
nn0= n2Nodes[0]
for nn1 in n2Nodes[1:]:
    newTruss= modelSpace.newElement("Truss", [nn0.tag, nn1.tag])
    newTruss.sectionArea= A
    n2Trusses.append(newTruss)
    nn0= nn1
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
for n in [n1, n2, n11, n12, n2Nodes[0], n2Nodes[-1]]:
    modelSpace.fixNode('00', n.tag)
for n in [n1A]+n2Nodes[1:-1]:
    modelSpace.fixNode('F0', n.tag) # Fix vertical displacement.

## Glued nodes.
gluedDOFs= xc.ID([0])
loadOnDOFs= [0,0]
for n in [nA, n2A]:
    for i in range(0,2):
        if i not in gluedDOFs:
            modelSpace.constraints.newSPConstraint(n.tag,i,0.0)
        else:
            loadOnDOFs[i]= -F

glueA= modelSpace.constraints.newGlueNodeToElement(nA,truss1, gluedDOFs)
middleIndex= int((len(n2Trusses)-1)/2)
middleTruss= n2Trusses[middleIndex]
gluesA= modelSpace.constraints.newGlueNodeToElement(n2A, middleTruss, gluedDOFs)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
loadVector= xc.Vector(loadOnDOFs)
lp0.newNodalLoad(nA.tag, loadVector)
lp0.newNodalLoad(n1A.tag, loadVector)
lp0.newNodalLoad(n2A.tag, loadVector)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
if(result!= 0):
    lmsg.error("Can't solve.")
    exit(1)

nodes.calculateNodalReactions(False,1e-7)

refVector= xc.Vector([F/2.0, 0])
reactionNodeA= nA.getReaction
ratio1= reactionNodeA.Norm()
reactionNode1= n1.getReaction
ratio2= (reactionNode1-refVector).Norm()
reactionNode2= n2.getReaction
ratio3= (reactionNode2-refVector).Norm()
nADisp= nA.getDisp[0]
n1ADisp= n1A.getDisp[0]
n2ADisp= n2A.getDisp[0]
refDisp= -F/2.*(l/2.)/E/A
ratio4= abs(nADisp-refDisp)/-refDisp # Ignored. The displacement approx. is very poor.
ratio5= abs(n1ADisp-refDisp)/-refDisp
ratio6= abs(n2ADisp-refDisp)/-refDisp

'''
print("ratio1= ", ratio1)
print("reactionNode1= ", reactionNode1)
print("ratio2= ", ratio2)
print("reactionNode2= ", reactionNode2)
print("ratio3= ", ratio3)
print('A glued node displacement: ', nADisp)
print('2A glued node displacement: ', n2ADisp)
print('reference node displacement: ', n1ADisp)
print('reference displacement: ', refDisp)
print("ratio4= ", ratio4, 'Ignored. The displacement approx. is very poor.')
print("ratio5= ", ratio5)
print("ratio6= ", ratio6)
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio5)<1e-8) & (abs(ratio6)<5e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
