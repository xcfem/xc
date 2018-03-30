# -*- coding: utf-8 -*-
# Home made test
# Sign criteria for the internal forces o a elastic beam 2d element.
# 2D cantilever beam, start node  with all its 3DOF fixed, point loads on the 
# end node (L)

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

def getInternalForcesBeginNode(elemTag):
  ''':return: internal forces on the element's first node.''' 
  elem= elements.getElement(elemTag)
  return (elem.getN1,elem.getV1,elem.getM1)

def getInternalForcesEndNode(elemTag):
  ''':return: internal forces on the element's last node.''' 
  elem= elements.getElement(elemTag)
  return (elem.getN2,elem.getV2,elem.getM2)

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

# Geometry
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude (kN)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
nodes.defaultTag= 1 #First node number.
nodes.newNodeXY(0,0.0)
nodes.newNodeXY(L,0.0)

lin= modelSpace.newLinearCrdTransf("lin")
# Materials
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G;
sectionProperties.I= Iz; 
seccion= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "seccion",sectionProperties)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= "lin"
elements.defaultMaterial= "seccion"
elements.defaultTag= 1 #Tag for the next element.
beam2d= elements.newElement("ElasticBeam2d",xc.ID([1,2]));

modelSpace.fixNode000(1)

cargas= preprocessor.getLoadHandler
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,0])) #Positive force along x axis
#We add the load case to domain.
casos.addToDomain("0")

# Solution 0 N
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)

RF= elements.getElement(1).getResistingForce()
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
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)

RF= elements.getElement(1).getResistingForce()
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
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)

RF= elements.getElement(1).getResistingForce()
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
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (result<1e-10):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
