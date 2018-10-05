# -*- coding: utf-8 -*-
# home made test
# Sum of the actions over the cantilever.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

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
F= 1.5e3 # Load magnitude (kN)
GF= 1.33 # Load factor.
M= 1.5e3 # Moment magnitude (kN m)
GM= 1.5 # Load factor.

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nodes.newNodeXYZ(0,0.0,0.0)
nodes.newNodeXYZ(L,0.0,0.0)

lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

    
# Materials definition
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= "lin"
elements.defaultMaterial= "scc"
elements.defaultTag= 1 #Tag for next element.
beam3d= elements.newElement("ElasticBeam3d",xc.ID([1,2]));

# Constraints
modelSpace.fixNode000_000(1)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
lp0= lPatterns.newLoadPattern("default","0")
lp0.gammaF= GF
lp1= lPatterns.newLoadPattern("default","1")
lp1.gammaF= GM
lp0.newNodalLoad(2,xc.Vector([F,0,0,0,0,0]))
lp1.newNodalLoad(2,xc.Vector([0,0,0,M,0,0]))
#We add the load case to domain.
lPatterns.addToDomain("0")
#We add the load case to domain.
lPatterns.addToDomain("1")

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)

delta= nodes.getNode(2).getDisp[0]
theta= nodes.getNode(2).getDisp[3]
elements.getElement(1).getResistingForce()
N1= elements.getElement(1).getN1
M1= elements.getElement(1).getT

deltateor= (GF*F*L/(E*A))
ratio1= (delta-deltateor)/deltateor
ratio2= (N1/F/GF)

thetateor= (GM*M*L/(G*J))
ratio3= (M1/M/GM)
ratio4= (theta-thetateor)/thetateor

# print "computed delta= ",delta
# print "theoretical delta= ",deltateor
# print "theta prog.= ", theta
# print "theta teor.= ", thetateor
# print "ratio1= ",ratio1
# print "ratio2= ",ratio2
# print "M1= ",M1
# print "ratio3= ",ratio3
# print "ratio4= ",ratio4

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5) & (abs(ratio4)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

