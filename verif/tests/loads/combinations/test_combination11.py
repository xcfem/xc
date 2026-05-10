# -*- coding: utf-8 -*-
'''Check behaviour when the combination expression contains a minus sign. Home 
   made test.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
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
lpAm= lPatterns.newLoadPattern("default","Am")
lpB= lPatterns.newLoadPattern("default","B")
lpBm= lPatterns.newLoadPattern("default","Bm")
#\set_current_load_pattern{"A"}
lpA.newNodalLoad(n2.tag,xc.Vector([1.0,0,0,0,0,0]))
lpAm.newNodalLoad(n2.tag,xc.Vector([-1.0,0,0,0,0,0]))
lpB.newNodalLoad(n2.tag,xc.Vector([2.0,0,0,0,0,0]))
lpBm.newNodalLoad(n2.tag,xc.Vector([-2.0,0,0,0,0,0]))
combs= loadHandler.getLoadCombinations
comb1= combs.newLoadCombination("COMB1","1.33*A+1.5*B")
comb2= combs.newLoadCombination("COMB2","-1.33*Am+1.5*B")
comb3= combs.newLoadCombination("COMB3","1.33*A-1.5*Bm")
comb4= combs.newLoadCombination("COMB4","-1.33*Am-1.5*Bm")

reactions= list()
ratios= list()
for comb in [comb1, comb2, comb3, comb4]:
    comb.addToDomain()
    result= modelSpace.analyze(calculateNodalReactions= True)
    reacx= n1.getReaction[0]
    reacxTeor= -(1.33+1.5*2)
    reactions.append(reacx)
    ratios.append(abs(reacx-reacxTeor)/reacxTeor)
    comb.removeFromDomain()

testOK= True
for ratio in ratios:
    if(abs(ratio)>1e-6):
        testOK= False
        break


'''
print("reactions= ", reactions)
print("reacxTeor= ", reacxTeor)
print("ratios= ", ratios)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')



