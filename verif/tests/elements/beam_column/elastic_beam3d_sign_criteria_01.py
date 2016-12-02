# -*- coding: utf-8 -*-
# Home made test
# Sign criteria for the internal forces o a elastic beam 3d element.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
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
F= 1.5e3 # Load magnitude (kN)

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
# Problem type
predefined_spaces.gdls_resist_materiales3D(nodes)
nodes.defaultTag= 1 #First node number.
nodes.newNodeXYZ(0,0.0,0.0)
nodes.newNodeXYZ(L,0.0,0.0)

trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

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



coacciones= preprocessor.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)

cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,0,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution 0 N
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

RF= elementos.getElement(1).getResistingForce()
N1= elementos.getElement(1).getN1
N1Teor= F
Vy1= elementos.getElement(1).getVy
Vz1= elementos.getElement(1).getVz
T1= elementos.getElement(1).getT1
My1= elementos.getElement(1).getMy1
Mz1= elementos.getElement(1).getMz1

ratios= list()
ratio0= abs((N1-N1Teor)/N1Teor)
ratio1= abs(Vy1)
ratio2= abs(Vz1)
ratio3= abs(T1)
ratio4= abs(My1)
ratio5= abs(Mz1)

ratios.extend([ratio0,ratio1,ratio2,ratio3,ratio4,ratio5])

# print "RF= ",RF
# print "N1= ",N1
# print  'ratio0= ',ratio0
# print "My1= ",My1
# print  'ratio1= ',ratio1
# print "Vz1= ",Vz1
# print 'ratio2= ', ratio2
# print "Vy1= ",Vy1
# print  'ratio3= ',ratio3
# print "T1= ",T1
# print  'ratio4= ',ratio4

lp0.removeFromDomain()
lp1= casos.newLoadPattern("default","1")
lp1.newNodalLoad(2,xc.Vector([0,-F,0,0,0,0]))
#We add the load case to domain.
casos.addToDomain("1")

# Solution 1 My Vz
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

RF= elementos.getElement(1).getResistingForce()
N1= elementos.getElement(1).getN1
Vy1= elementos.getElement(1).getVy
Vz1= elementos.getElement(1).getVz
T1= elementos.getElement(1).getT1
My1= elementos.getElement(1).getMy1
My1Teor= -F*L
Mz1= elementos.getElement(1).getMz1

ratio10= abs(N1)
ratio11= abs(Vy1)
ratio12= abs((Vz1-F)/F)
ratio13= abs(T1)
ratio14= abs((My1Teor-My1)/My1Teor)
ratio15= abs(Mz1)

ratios.extend([ratio10,ratio11,ratio12,ratio13,ratio14,ratio15])

# print "RF= ",RF
# print "N1= ",N1
# print  'ratio10= ',ratio10
# print "My1= ",My1
# print  'ratio11= ',ratio11
# print "Vz1= ",Vz1
# print 'ratio12= ', ratio12
# print "Vy1= ",Vy1
# print  'ratio13= ',ratio13
# print "T1= ",T1
# print  'ratio14= ',ratio14

lp1.removeFromDomain()
lp2= casos.newLoadPattern("default","2")
lp2.newNodalLoad(2,xc.Vector([0,0,F,0,0,0]))
#We add the load case to domain.
casos.addToDomain("2")

# Solution 2 Mz Vy
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

RF= elementos.getElement(1).getResistingForce()
N1= elementos.getElement(1).getN1
Vy1= elementos.getElement(1).getVy
Vz1= elementos.getElement(1).getVz
T1= elementos.getElement(1).getT1
My1= elementos.getElement(1).getMy1
Mz1= elementos.getElement(1).getMz1
Mz1Teor= -F*L

ratio20= abs(N1)
ratio21= abs((Vy1-F)/F)
ratio22= abs(Vz1)
ratio23= abs(T1)
ratio24= abs(My1)
ratio25= abs((Mz1Teor-Mz1)/Mz1Teor)

ratios.extend([ratio20,ratio21,ratio22,ratio23,ratio24,ratio25])

# print "RF= ",RF
# print "N1= ",N1
# print  'ratio20= ',ratio20
# print "My1= ",My1
# print  'ratio21= ',ratio21
# print "Vz1= ",Vz1
# print 'ratio22= ', ratio22
# print "Vy1= ",Vy1
# print  'ratio23= ',ratio23
# print "T1= ",T1
# print  'ratio24= ',ratio24

lp2.removeFromDomain()
lp3= casos.newLoadPattern("default","3")
lp3.newNodalLoad(2,xc.Vector([0,0,0,F,0,0]))
#We add the load case to domain.
casos.addToDomain("3")

# Solution 3 T
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

RF= elementos.getElement(1).getResistingForce()
N1= elementos.getElement(1).getN1
Vy1= elementos.getElement(1).getVy
Vz1= elementos.getElement(1).getVz
T1= elementos.getElement(1).getT1
My1= elementos.getElement(1).getMy1
Mz1= elementos.getElement(1).getMz1

ratio30= abs(N1)
ratio31= abs(Vy1)
ratio32= abs(Vz1)
ratio33= abs((T1+F)/F)
ratio34= abs(My1)
ratio35= abs(Mz1)

ratios.extend([ratio30,ratio31,ratio32,ratio33,ratio34,ratio35])

# print "RF= ",RF
# print "N1= ",N1
# print  'ratio30= ',ratio30
# print "My1= ",My1
# print  'ratio31= ',ratio31
# print "Vz1= ",Vz1
# print 'ratio32= ', ratio32
# print "Vy1= ",Vy1
# print  'ratio33= ',ratio33
# print "T1= ",T1
# print  'ratio34= ',ratio34

result= 0.0
for r in ratios:
  result+= r*r
result= math.sqrt(result)
# print 'ratios= ',ratios
# print 'result= ',result

import os
fname= os.path.basename(__file__)
if (result<1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
