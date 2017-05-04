# -*- coding: utf-8 -*-
# home made test
# Ménsula sometida a carga uniforme horizontal según el eje z local.

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
f= 1.5e3 # Load magnitude (kN/m)

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)

# Materials
caracMecSeccion= xc.CrossSectionProperties3d()
caracMecSeccion.A= A; caracMecSeccion.E= E; caracMecSeccion.G= G;
caracMecSeccion.Iz= Iz; caracMecSeccion.Iy= Iy; caracMecSeccion.J= J
seccion= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "seccion",caracMecSeccion)

# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,-1,0])

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
eleLoad= lp0.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID([1]) 
eleLoad.transZComponent= -f
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodes= preprocessor.getNodeLoader 
nod2= nodes.getNode(2)
delta= nod2.getDisp[1]  # Node 2 yAxis displacement global

elementos= preprocessor.getElementLoader

elem1= elementos.getElement(1)
elem1.getResistingForce()
My1= elem1.getMy1 # Momento en el extremo dorsal de la barra
My2= elem1.getMy2 # Momento en el extremo frontal de la barra
Vz1= elem1.getVz1 # Cortante en el extremo dorsal de la barra
Vz2= elem1.getVz2 # Cortante en el extremo frontal de la barra



deltateor= (-f*L**4/(8*E*Iy))
ratio1= (-delta/deltateor)
My1teor= f*L*L/2
ratio2= abs((My1-My1teor)/My1teor)
ratio3= (abs(My2)<1e-3)
Vz1teor= (-f*L)
ratio4= abs((Vz1-Vz1teor)/Vz1teor)
ratio5= (abs(Vz2)<1e-3)

'''
print "delta= ",delta
print "deltateor= ",deltateor
print "ratio1= ",ratio1
print "My1= ",My1
print "My1teor= ",My1teor
print "ratio2= ",ratio2
print "My2= ",My2
print "My2teor= ",0
print "ratio3= ",ratio3
print "Vz1= ",Vz1
print "Vz1teor= ",Vz1teor
print "ratio4= ",ratio4
print "Vz2= ",Vz2
print "Vz2teor= ",0
print "ratio5= ",ratio5}
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (ratio2<1e-5) & ratio3 & (ratio4<1e-5) & (abs(ratio5-1.0)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
