# -*- coding: utf-8 -*-
''' Trivial test for removing and existing load pattern.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O)"
__copyright__= "Copyright 2023, LCPT  AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.Ortega@ciccp.es"

import math
import xc
from model import predefined_spaces
from materials import typical_materials
from misc_utils import log_messages as lmsg

# Problem data
E= 2e6 # Elastic modulus
L= 5 # Bar length.
h= 0.30 # Beam cross-section depth.
b= 0.2 # Beam cross-section width.
A= b*h # Cross section area.
I= b*h**3/12 # Inertia of the beam section in inches to the fourth power.


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Define mesh.
## Define nodes.
n1= modelSpace.newNode(0,0)
n2= modelSpace.newNode(L, 0)
n3= modelSpace.newNode(2*L, 0)
## Define elements.    
### Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin") 
### Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)
### Elements definition
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(scc)
beamA= modelSpace.newElement("ElasticBeam2d", [n1.tag,n2.tag])
beamA.h= h
beamB= modelSpace.newElement("ElasticBeam2d", [n2.tag,n3.tag])
beamB.h= h

## Define constraints.
modelSpace.fixNode('00F', n1.tag)
modelSpace.fixNode('F0F', n3.tag)

# Define loads.
testLP= modelSpace.newLoadPattern(name= 'testLP')
modelSpace.setCurrentLoadPattern(testLP.name)
f= 1e3
loadVector= xc.Vector([0, -f])
eLA= beamA.vector2dUniformLoadGlobal(loadVector)
eLB= beamB.vector2dUniformLoadGlobal(loadVector)
## Apply load pattern.
modelSpace.addLoadCaseToDomain(testLP.name)

# Solve.
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)

R0= 0.0
for n in [n1, n2, n3]:
    R0+= n.getReaction[1]

eLBTag= eLB.tag # Get the identifier of interest.
eLB= None # Avoid calling an object that will not exist anymore.
testLP.removeElementalLoad(eLBTag)

# Solve again.
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)
    
R1= 0.0
for n in [n1, n2, n3]:
    R1+= n.getReaction[1]
    
err= math.sqrt((R0-2*f*L)**2+(R1-f*L)**2)

'''
print('R0= ', R0/1e3)
print('R1= ', R1/1e3)
print('err= ', err)
'''

import os
fname= os.path.basename(__file__)
if(err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


