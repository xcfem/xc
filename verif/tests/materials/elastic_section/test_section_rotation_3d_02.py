# -*- coding: utf-8 -*-
# home made test
# «rotate» method verification for a 3D elastic section.
#    Ménsula sometida a carga vertical en su extremo con la sección y la carga inclinadas
#    45 grados.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math

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
L= 1 # Bar length (m)

# Load
F= 1e3 # Load magnitude (kN)
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)

trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

# Materials definition
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)
scc.sectionProperties.rotate(math.radians(90))

# Elements definition
elements= preprocessor.getElementLoader
elements.defaultTransformation= "lin"
elements.defaultMaterial= "scc"
elements.defaultTag= 1 #Tag for next element.
beam3d= elements.newElement("elastic_beam_3d",xc.ID([1,2]));

# Constraints
modelSpace.fixNode000_000(1)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
lp0= casos.newLoadPattern("default","0") 
lp0.newNodalLoad(2,xc.Vector([0,0,-F,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodes= preprocessor.getNodeLoader 
nod2= nodes.getNode(2)
delta= nod2.getDisp[2]  # z displacement of node 2
elements= preprocessor.getElementLoader

elem1= elements.getElement(1)
elem1.getResistingForce()
M= elem1.getMz1
V= elem1.getVy

deltateor= (-F*L**3/(3*E*Iy))
ratio1= abs(delta-deltateor)/deltateor
ratio2= abs((F*L)-M)/(F*L)
ratio3= abs(V+F)/F

''' 
print "delta= ",delta
print "deltaTeor= ",deltateor
print "ratio1= ",ratio1
print "M= ",M
print "MTeor= ",(F*L)
print "ratio2= ",ratio2
print "V1= ",V
print "ratio3= ",ratio3
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-9) & (ratio2<1e-15) & (ratio3<1e-15)): 
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
