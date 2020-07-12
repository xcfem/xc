# -*- coding: utf-8 -*-
from __future__ import print_function
# Home made test
# Sign criteria for the internal forces in an elastic beam 3d element.
# 3D cantilever beam, start node  with all its 6DOF fixed, point loads on the 
# end node (x=L)

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
  return (elem.getN1,elem.getVy1,elem.getVz1,elem.getT1,elem.getMy1,elem.getMz1)

def getInternalForcesEndNode(elemTag):
  ''':return: internal forces on the element's last node.''' 
  elem= elements.getElement(elemTag)
  return (elem.getN2,elem.getVy2,elem.getVz2,elem.getT2,elem.getMy2,elem.getMz2)

def printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,phase):
  ratioMsg= 'ratio'+str(phase)
  print('N1= ', N1, ' N2= ', N2 )
  print(ratioMsg+'0= ', phaseRatios[0])
  print('Vy1= ',Vy1, 'Vy2= ',Vy2 )
  print(ratioMsg+'1= ', phaseRatios[1])
  print('Vz1= ',Vz1, 'Vz2= ',Vz2)
  print(ratioMsg+'2= ', phaseRatios[2])
  print('T1= ',T1, 'T2= ', T2)
  print(ratioMsg+'3= ', phaseRatios[3])
  print('My1= ',My1, 'My2= ', My2)
  print(ratioMsg+'4= ', phaseRatios[4])
  print('Mz1= ',Mz1, 'Mz2= ', Mz2)
  print(ratioMsg+'5= ', phaseRatios[5])
                     
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

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nodes.newNodeXYZ(0,0.0,0.0)
nodes.newNodeXYZ(L,0.0,0.0)

lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,0,1]))
# Materials
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
section= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section",sectionProperties)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
elements.defaultTag= 1 #Tag for the next element.
beam3d= elements.newElement("ElasticBeam3d",xc.ID([1,2]))



modelSpace.fixNode000_000(1)

loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,0,0,0,0])) #Positive force along x axis
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution 0 N
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= elements.getElement(1).getResistingForce()
(N1,Vy1,Vz1,T1,My1,Mz1)= getInternalForcesBeginNode(1)
NTeor= F
(N2,Vy2,Vz2,T2,My2,Mz2)= getInternalForcesEndNode(1)

ratios= list()
ratio0= abs((N1-NTeor)/NTeor)+abs((N2-NTeor)/NTeor)
ratio1= abs(Vy1)+abs(Vy2)
ratio2= abs(Vz1)+abs(Vz2)
ratio3= abs(T1)+abs(T2)
ratio4= abs(My1)+abs(My2)
ratio5= abs(Mz1)+abs(Mz2)
phaseRatios= [ratio0,ratio1,ratio2,ratio3,ratio4,ratio5]
ratios.extend(phaseRatios)

# print('RF= ',RF)
#printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,'')


lp0.removeFromDomain()
lp1= lPatterns.newLoadPattern("default","1")
lp1.newNodalLoad(2,xc.Vector([0,F,0,0,0,0])) #Positive force along y axis
#We add the load case to domain.
lPatterns.addToDomain("1")

# Solution 1 My Vz
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= elements.getElement(1).getResistingForce()
(N1,Vy1,Vz1,T1,My1,Mz1)= getInternalForcesBeginNode(1)
Mz1Teor= F*L
(N2,Vy2,Vz2,T2,My2,Mz2)= getInternalForcesEndNode(1)

ratio10= abs(N1)+abs(N2)
ratio11= abs((Vy1-F)/F)+abs((Vy2-F)/F)
ratio12= abs(Vz1)+abs(Vz2)
ratio13= abs(T1)+abs(T2)
ratio14= abs(My1)+abs(My2)
ratio15= abs((Mz1Teor-Mz1)/Mz1Teor)+abs(Mz2)

phaseRatios= [ratio10,ratio11,ratio12,ratio13,ratio14,ratio15]
ratios.extend(phaseRatios)

# print("RF= ",RF)
#printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,'1')

lp1.removeFromDomain()
lp2= lPatterns.newLoadPattern("default","2")
lp2.newNodalLoad(2,xc.Vector([0,0,F,0,0,0])) #Positive force along z axis
#We add the load case to domain.
lPatterns.addToDomain("2")

# Solution 2 Mz Vy
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= elements.getElement(1).getResistingForce()
(N1,Vy1,Vz1,T1,My1,Mz1)= getInternalForcesBeginNode(1)
My1Teor= -F*L
(N2,Vy2,Vz2,T2,My2,Mz2)= getInternalForcesEndNode(1)

ratio20= abs(N1)+abs(N2)
ratio21= abs(Vy1)+abs(Vy2)
ratio22= abs((Vz1-F)/F)+abs((Vz2-F)/F)
ratio23= abs(T1)+abs(T2)
ratio24= abs((My1Teor-My1)/My1Teor)+abs(My2)
ratio25= abs(Mz1)+abs(Mz2)

phaseRatios= [ratio20,ratio21,ratio22,ratio23,ratio24,ratio25]
ratios.extend(phaseRatios)

# print("RF= ",RF)
#printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,'2')


lp2.removeFromDomain()
lp3= lPatterns.newLoadPattern("default","3")
lp3.newNodalLoad(2,xc.Vector([0,0,0,F,0,0])) #Positive moment about x axis
#We add the load case to domain.
lPatterns.addToDomain("3")

# Solution 3 T
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= elements.getElement(1).getResistingForce()
(N1,Vy1,Vz1,T1,My1,Mz1)= getInternalForcesBeginNode(1)
(N2,Vy2,Vz2,T2,My2,Mz2)= getInternalForcesEndNode(1)

ratio30= abs(N1)+abs(N2)
ratio31= abs(Vy1)+abs(Vy2)
ratio32= abs(Vz1)+abs(Vz2)
ratio33= abs((T1-F)/F)+abs((T2-F)/F)
ratio34= abs(My1)+abs(My2)
ratio35= abs(Mz1)+abs(Mz1)

phaseRatios= [ratio30,ratio31,ratio32,ratio33,ratio34,ratio35]
ratios.extend(phaseRatios)

# print("RF= ",RF)
#printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,'3')

lp3.removeFromDomain()
lp4= lPatterns.newLoadPattern("default","4")
lp4.newNodalLoad(2,xc.Vector([0,0,0,0,F,0])) #Positive moment about y axis
#We add the load case to domain.
lPatterns.addToDomain("4")

# Solution 4 T
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= elements.getElement(1).getResistingForce()
(N1,Vy1,Vz1,T1,My1,Mz1)= getInternalForcesBeginNode(1)
(N2,Vy2,Vz2,T2,My2,Mz2)= getInternalForcesEndNode(1)

ratio40= abs(N1)+abs(N2)
ratio41= abs(Vy1)+abs(Vy2)
ratio42= abs(Vz1)+abs(Vz2)
ratio43= abs(T1)+abs(T2)
ratio44= abs((My1-F)/F)+abs((My2-F)/F)
ratio45= abs(Mz1)+abs(Mz2)

phaseRatios= [ratio40,ratio41,ratio42,ratio43,ratio44,ratio45]
ratios.extend(phaseRatios)

# print("RF= ",RF)
#printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,'4')

lp4.removeFromDomain()
lp5= lPatterns.newLoadPattern("default","5")
lp5.newNodalLoad(2,xc.Vector([0,0,0,0,0,F])) #Positive moment about z axis
#We add the load case to domain.
lPatterns.addToDomain("5")

# Solution 5 T
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= elements.getElement(1).getResistingForce()
(N1,Vy1,Vz1,T1,My1,Mz1)= getInternalForcesBeginNode(1)
(N2,Vy2,Vz2,T2,My2,Mz2)= getInternalForcesEndNode(1)

ratio50= abs(N1)+abs(N2)
ratio51= abs(Vy1)+abs(Vy2)
ratio52= abs(Vz1)+abs(Vz2)
ratio53= abs(T1)+abs(T2)
ratio54= abs(My1)+abs(My2)
ratio55= abs((Mz1-F)/F)+abs((Mz2-F)/F)

phaseRatios= [ratio50,ratio51,ratio52,ratio53,ratio54,ratio55]
ratios.extend(phaseRatios)

# print("RF= ",RF)
#printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,'5')

result= 0.0
for r in ratios:
  result+= r*r
result= math.sqrt(result)
# print('ratios= ',ratios)
# print('result= ',result)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (result<1e-10):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
