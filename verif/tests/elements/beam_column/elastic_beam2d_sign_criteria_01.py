# -*- coding: utf-8 -*-
# Home made test
# Sign criteria for the internal forces o a elastic beam 2d element.

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
from model import fix_node_3dof
from materials import typical_materials

def getInternalForcesBeginNode(elemTag):
  ''':return: internal forces on the element's first node.''' 
  return (elementos.getElement(elemTag).getN1,elementos.getElement(elemTag).getV1,elementos.getElement(elemTag).getM1)

def getInternalForcesEndNode(elemTag):
  ''':return: internal forces on the element's last node.''' 
  return (elementos.getElement(elemTag).getN2,elementos.getElement(elemTag).getV2,elementos.getElement(elemTag).getM2)

def printResults(N1,V1,M1,N2,V2,M2,phaseRatios,phase):
  ratioMsg= 'ratio'+str(phase)
  print 'N1= ', N1, ' N2= ', N2 
  print ratioMsg+'0= ', phaseRatios[0]
  print 'V1= ',V1, 'V2= ',V2 
  print ratioMsg+'1= ', phaseRatios[1]
  print 'M1= ',M1, 'M2= ', M2
  print ratioMsg+'2= ', phaseRatios[2]

                     
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
predefined_spaces.gdls_resist_materiales2D(nodes)
nodes.defaultTag= 1 #First node number.
nodes.newNodeXYZ(0,0.0,0.0)
nodes.newNodeXYZ(L,0.0,0.0)

trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")
lin.xzVector= xc.Vector([0,0,1])

# Materials
caracMecSeccion= xc.CrossSectionProperties2d()
caracMecSeccion.A= A; caracMecSeccion.E= E; caracMecSeccion.G= G;
caracMecSeccion.I= Iz; 
seccion= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "seccion",caracMecSeccion)

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "seccion"
elementos.defaultTag= 1 #Tag for the next element.
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([1,2]));



coacciones= preprocessor.getConstraintLoader
fix_node_3dof.fixNode000(coacciones,1)

cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,0])) #Positive force along x axis
#We add the load case to domain.
casos.addToDomain("0")

# Solution 0 N
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

RF= elementos.getElement(1).getResistingForce()
(N1,V1,M1)= getInternalForcesBeginNode(1)
NTeor= F
(N2,V2,M2)= getInternalForcesEndNode(1)

ratios= list()
ratio0= abs((N1-NTeor)/NTeor)+abs((N2-NTeor)/NTeor)
ratio1= abs(V1)+abs(V2)
ratio2= abs(M1)+abs(M2)
phaseRatios= [ratio0,ratio1,ratio2]
ratios.extend(phaseRatios)

# print 'RF= ',RF
#printResults(N1,V1,M1,N2,V2,M2,phaseRatios,'')


lp0.removeFromDomain()
lp1= casos.newLoadPattern("default","1")
lp1.newNodalLoad(2,xc.Vector([0,F,0])) #Positive force along y axis
#We add the load case to domain.
casos.addToDomain("1")

# Solution 1 V
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

RF= elementos.getElement(1).getResistingForce()
(N1,V1,M1)= getInternalForcesBeginNode(1)
M1Teor= F*L
(N2,V2,M2)= getInternalForcesEndNode(1)

ratio10= abs(N1)+abs(N2)
ratio11= abs((V1-F)/F)+abs((V2-F)/F)
ratio12= abs((M1Teor-M1)/M1Teor)+abs(M2)

phaseRatios= [ratio10,ratio11,ratio12]
ratios.extend(phaseRatios)

# print "RF= ",RF
#printResults(N1,V1,M1,N2,V2,M2,phaseRatios,'1')

lp1.removeFromDomain()
lp2= casos.newLoadPattern("default","2")
lp2.newNodalLoad(2,xc.Vector([0,0,F])) #Positive moment about z axis
#We add the load case to domain.
casos.addToDomain("2")

# Solution 2 M
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

RF= elementos.getElement(1).getResistingForce()
(N1,V1,M1)= getInternalForcesBeginNode(1)
(N2,V2,M2)= getInternalForcesEndNode(1)

ratio20= abs(N1)+abs(N2)
ratio21= abs(V1)+abs(V2)
ratio22= abs((M1-F)/F)+abs((M2-F)/F)

phaseRatios= [ratio20,ratio21,ratio22]
ratios.extend(phaseRatios)

# print "RF= ",RF
#printResults(N1,V1,M1,N2,V2,M2,phaseRatios,'2')

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
