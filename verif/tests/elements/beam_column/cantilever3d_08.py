# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test
# Cantilever under uniform vertical load in local y axis.

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

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,-1,0]))
# Materials
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
section= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section",sectionProperties)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= "section"
elements.defaultTag= 1 #Tag for the next element.
beam3d= elements.newElement("ElasticBeam3d",xc.ID([1,2]))



# Constraints
modelSpace.fixNode000_000(1)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#lPatterns.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID([1]) 
eleLoad.transComponent= -f
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)


nodes= preprocessor.getNodeHandler 
nod2= nodes.getNode(2)
delta= nod2.getDisp[2]  # z displacement of node 2 global

elements= preprocessor.getElementHandler

elem1= elements.getElement(1)
elem1.getResistingForce()
Mz1= elem1.getMz1 # Moment at the back end of the beam
Mz2= elem1.getMz2 # Moment at the front end of the beam
Vy1= elem1.getVy1 # Shear force at the back end of the beam
Vy2= elem1.getVy2 # Shear force at the front end of the beam



deltateor= (-f*L**4/(8*E*Iz))
ratio1= (delta/deltateor)
Mz1teor= (-f*L*L/2)
ratio2= (Mz1/Mz1teor)
ratio3= (abs(Mz2)<1e-3)
Vy1teor= (-f*L)
ratio4= (Vy1/Vy1teor)
ratio5= (abs(Vy2)<1e-3)

'''
print("delta= ",delta)
print("deltateor= ",deltateor)
print("ratio1= ",ratio1)
print("Mz1= ",Mz1)
print("Mz1teor= ",Mz1teor)
print("ratio2= ",ratio2)
print("Mz2= ",Mz2)
print("Mz2teor= ",0)
print("ratio3= ",ratio3)
print("Vy1= ",Vy1)
print("Vy1teor= ",Vy1teor)
print("ratio4= ",ratio4)
print("Vy2= ",Vy2)
print("Vy2teor= ",0)
print("ratio5= ",ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5) & (abs(ratio4-1.0)<1e-5) & (abs(ratio5-1.0)<1e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
