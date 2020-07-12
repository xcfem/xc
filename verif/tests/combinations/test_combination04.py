# -*- coding: utf-8 -*-
from __future__ import print_function

'''Cantilever load combination. Home made test.'''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from misc_utils import log_messages as lmsg

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

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
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,-1,0]))    
# Materials definition
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
#  sintaxis: ElasticBeam3d[<tag>] 
elements.defaultTag= 1 #Tag for next element.
beam3d= elements.newElement("ElasticBeam3d",xc.ID([1,2]))

# Constraints
modelSpace.fixNode000_000(1)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
lpA= lPatterns.newLoadPattern("default","A") 
lpB= lPatterns.newLoadPattern("default","B") 
#lPatterns.currentLoadPattern= "A"
eleLoad= lpA.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID([1]) 
eleLoad.axialComponent= f

#lPatterns.currentLoadPattern= "B"
eleLoad= lpB.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID([1]) 
eleLoad.transComponent= -f

combs= loadHandler.getLoadCombinations
comb= combs.newLoadCombination("COMB","1.33*A+1.5*B")

#We add the load case to domain.
loadHandler.addToDomain("COMB")

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
os.system("rm -f /tmp/combinacion04.db")
lmsg.error(fname+' test in QUARANTINE; SQLite interface debugging needed.')
exit(-1)
db= feProblem.newDatabase("SQLite","/tmp/combinacion04.db")
db.save(100)
feProblem.clearAll()
db.restore(100)


nodes= preprocessor.getNodeHandler
 
nod2= nodes.getNode(2)

deltax= nod2.getDisp[0] 
deltay= nod2.getDisp[2] 

elements= preprocessor.getElementHandler

elem1= elements.getElement(1)

elem1.getResistingForce()
N1= elem1.getN1 # Axial force at the back end of the beam
N2= elem1.getN2 # Axial force at the front end of the beam
Mz1= elem1.getMz1 # Moment at the back end of the beam
Mz2= elem1.getMz2 # Moment at the front end of the beam
Vy1= elem1.getVy1 # Shear force at the back end of the beam
Vy2= elem1.getVy2 # Shear force at the front end of the beam

deltaxteor= (1.33*f*L**2/(2*E*A))
ratio1= (deltax/deltaxteor)
N1teor= (1.33*f*L)
ratio2= (N1/N1teor)
ratio3= (abs(N2)<1e-3)
deltayteor= (-1.5*f*L**4/(8*E*Iz))
ratio4= (deltay/deltayteor)
Mz1teor= (-1.5*f*L*L/2)
ratio5= (Mz1/Mz1teor)
ratio6= (abs(Mz2)<1e-3)
Vy1teor= (-1.5*f*L)
ratio7= (Vy1/Vy1teor)
ratio8= (abs(Vy2)<1e-3)

''' 
print("deltax= ",deltax)
print("deltaxteor= ",deltaxteor)
print("ratio1= ",ratio1)
print("N1= ",N1)
print("N1teor= ",N1teor)
print("ratio2= ",ratio2)
print("N2= ",N2)
print("N2teor= ",0)
print("ratio3= ",ratio3)
print("deltax= ",deltax)
print("deltaxteor= ",deltaxteor)
print("ratio4= ",ratio4)
print("Mz1= ",Mz1)
print("Mz1teor= ",Mz1teor)
print("ratio5= ",ratio5)
print("Mz2= ",Mz2)
print("Mz2teor= ",0)
print("ratio6= ",ratio6)
print("Vy1= ",Vy1)
print("Vy1teor= ",Vy1teor)
print("ratio7= ",ratio7)
print("Vy2= ",Vy2)
print("Vy2teor= ",0)
print("ratio8= ",ratio8 ''')

if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5) & (abs(ratio4-1.0)<1e-5) & (abs(ratio5-1.0)<1e-5) & (abs(ratio6-1.0)<1e-5) & (abs(ratio7-1.0)<1e-5) & (abs(ratio8-1.0)<1e-5) :
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
os.system("rm -f /tmp/combinacion04.db") # Your garbage you clean it
