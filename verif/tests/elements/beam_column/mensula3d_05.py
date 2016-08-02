# -*- coding: utf-8 -*-
# home made test
# Ménsula sometida a carga horizontal en su extremo.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
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
M= 1.5e3 # Magnitud del momento en kN

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodos= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXYZ(0,0.0,0.0)
nod= nodos.newNodeXYZ(L,0.0,0.0)


# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,-1,0])

# Materials
caracMecSeccion= xc.CrossSectionProperties3d()
caracMecSeccion.A= A; caracMecSeccion.E= E; caracMecSeccion.G= G;
caracMecSeccion.Iz= Iz; caracMecSeccion.Iy= Iy; caracMecSeccion.J= J
seccion= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "seccion",caracMecSeccion)

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "seccion"
elementos.defaultTag= 1 #Tag for the next element.
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,2]));


# Constraints
coacciones= preprocessor.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,M,0]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodos= preprocessor.getNodeLoader
nod2= nodos.getNode(2)
delta= nod2.getDisp[2]  # Desplazamiento del nodo 2 según z
nod2= nodos.getNode(2)
theta= nod2.getDisp[4]  # Giro del nodo según y

elementos= preprocessor.getElementLoader

elem1= elementos.getElement(1)
elem1.getResistingForce()
M1= elem1.getMz1
V= elem1.getVz



deltateor= (M*L**2/(2*E*Iz))
thetateor= (M*L/(E*Iz))
ratio1= (delta/-deltateor)
ratio2= (M1/-M)
ratio3= (theta/thetateor)

''' 
print "delta prog.= ", delta
print "delta teor.= ", deltateor
print "ratio1= ",ratio1
print "M1= ",M1
print "ratio2= ",ratio2
print "theta prog.= ", theta
print "theta teor.= ", thetateor
print "ratio3= ",ratio3
 '''

cumple= (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5)
import os
fname= os.path.basename(__file__)
if cumple:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
