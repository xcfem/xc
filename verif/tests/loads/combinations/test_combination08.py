# -*- coding: utf-8 -*-
from __future__ import print_function
'''Cantilever load combination. Home made test'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Material properties
E= 2.1e6*9.81/1e-4 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

# Cross section properties (IPE-80)
A= 7.64e-4 # Cross section area (m2)
Iy= 80.1e-8 # Cross section moment of inertia (m4)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)

# Geometry
L= 1.5 # Bar length (m)

# Load
f= 1.5e3 # Load magnitude (kN/m)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor  
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)

# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,-1,0]))
# Materials definition
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
#  sintaxis: ElasticBeam3d[<tag>] 
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))

# Constraints
modelSpace.fixNode000_000(n1.tag)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
lpA= lPatterns.newLoadPattern("default","A")
lpB= lPatterns.newLoadPattern("default","B")
#\set_current_load_pattern{"A"}
lpA.newNodalLoad(n2.tag,xc.Vector([1.0,0,0,0,0,0]))
lpB.newNodalLoad(n2.tag,xc.Vector([1.0,0,0,0,0,0]))
combs= loadHandler.getLoadCombinations
comb= combs.newLoadCombination("COMB","1.33*A+1.5*B")


# Add only the A component of the combination.
comb.addToDomain(['A'])
result= modelSpace.analyze(calculateNodalReactions= True)
reacxA= n1.getReaction[0]
reacxATeor= -1.33
ratio1= abs(reacxA-reacxATeor)/reacxATeor

# Add the rest of the combination.
comb.addToDomain()
result= modelSpace.analyze(calculateNodalReactions= True)
reacx= n1.getReaction[0]
reacxTeor= -(1.33+1.5)
ratio2= abs(reacx-reacxTeor)/reacxTeor


'''
print("reacxA= ",reacxA)
print("reacxATeor= ",reacxATeor)
print("ratio1= ",ratio1)
print("reacx= ",reacx)
print("reacxTeor= ",reacxTeor)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')



