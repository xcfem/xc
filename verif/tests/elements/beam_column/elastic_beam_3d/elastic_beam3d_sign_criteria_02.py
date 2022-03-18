# -*- coding: utf-8 -*-
''' Home made test.'''
from __future__ import print_function
# Sign criteria for the internal forces in an elastic beam 3d element.
# 3D cantilever beam, start node  with all its 6DOF fixed, point loads on the 
# mid-span (x=L/2)

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

def getInternalForcesBeginNode(elem):
    ''':return: internal forces on the element's first node.'''
    return (elem.getN1,elem.getVy1,elem.getVz1,elem.getT1,elem.getMy1,elem.getMz1)

def getInternalForcesEndNode(elem):
    ''':return: internal forces on the element's last node.''' 
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
F= 1.5e3          # Load magnitude (kN)
xRelPtoAplic= 0.5 # x relative (compared to the total length) of the
                  # point on which the load is applied

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)

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
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n1.tag, n2.tag]))


modelSpace.fixNode000_000(n1.tag)

elem=preprocessor.getElementHandler.getElement(1) # element 1

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
beam3d.vector3dPointByRelDistLoadLocal(xRelPtoAplic,xc.Vector([F,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution 0 N 
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= beam3d.getResistingForce()
(N1,Vy1,Vz1,T1,My1,Mz1)= getInternalForcesBeginNode(beam3d)
NTeor= F
(N2,Vy2,Vz2,T2,My2,Mz2)= getInternalForcesEndNode(beam3d)

ratios= list()
ratio0= abs((N1-NTeor)/NTeor)+abs(N2)
ratio1= abs(Vy1)+abs(Vy2)
ratio2= abs(Vz1)+abs(Vz2)
ratio3= abs(T1)+abs(T2)
ratio4= abs(My1)+abs(My2)
ratio5= abs(Mz1)+abs(Mz2)
phaseRatios= [ratio0,ratio1,ratio2,ratio3,ratio4,ratio5]
ratios.extend(phaseRatios)

# print('RF= ',RF)
# printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,'')

lp0.removeFromDomain()
lp1= modelSpace.newLoadPattern(name= '1')
modelSpace.setCurrentLoadPattern("1")
beam3d.vector3dPointByRelDistLoadLocal(xRelPtoAplic,xc.Vector([0,F,0]))
modelSpace.addLoadCaseToDomain("1")

# Solution 1 Mz Vy
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= beam3d.getResistingForce()
(N1,Vy1,Vz1,T1,My1,Mz1)= getInternalForcesBeginNode(beam3d)
Vy1Teor=F
Mz1Teor= F*L/2.0
(N2,Vy2,Vz2,T2,My2,Mz2)= getInternalForcesEndNode(beam3d)

ratio10= abs(N1)+abs(N2)
ratio11= abs((Vy1-Vy1Teor)/Vy1)+abs(Vy2)
ratio12= abs(Vz1)+abs(Vz2)
ratio13= abs(T1)+abs(T2)
ratio14= abs(My1)+abs(My2)
ratio15= abs((Mz1Teor-Mz1)/Mz1)+abs(Mz2)

phaseRatios= [ratio10,ratio11,ratio12,ratio13,ratio14,ratio15]
ratios.extend(phaseRatios)

# print("RF= ",RF)
# printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,'1')


lp1.removeFromDomain()
lp2= modelSpace.newLoadPattern(name= '2')
modelSpace.setCurrentLoadPattern("2")
beam3d.vector3dPointByRelDistLoadLocal(xRelPtoAplic,xc.Vector([0,0,F]))
modelSpace.addLoadCaseToDomain("2")


# Solution 2 My Vz
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= beam3d.getResistingForce()
(N1,Vy1,Vz1,T1,My1,Mz1)= getInternalForcesBeginNode(beam3d)
Vz1Teor=F
My1Teor= -F*L/2.0
(N2,Vy2,Vz2,T2,My2,Mz2)= getInternalForcesEndNode(beam3d)

ratio20= abs(N1)+abs(N2)
ratio21= abs(Vy1)+abs(Vy2)
ratio22= abs((Vz1-Vz1Teor)/Vz1)+abs(Vz2)
ratio23= abs(T1)+abs(T2)
ratio24= abs((My1Teor-My1)/My1)+abs(My2)
ratio25= abs(Mz1)+abs(Mz2)

phaseRatios= [ratio20,ratio21,ratio22,ratio23,ratio24,ratio25]
ratios.extend(phaseRatios)

# print("RF= ",RF)
# printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,'2')


lp2.removeFromDomain()
lp3= modelSpace.newLoadPattern(name= '3')
modelSpace.setCurrentLoadPattern("3")
beam3d.vector3dUniformLoadLocal(xc.Vector([0,0,F]))
modelSpace.addLoadCaseToDomain("3")

# Solution 3 T
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= beam3d.getResistingForce()
(N1,Vy1,Vz1,T1,My1,Mz1)= getInternalForcesBeginNode(beam3d)
Vz1Teor=F*L
My1Teor=-F*L*L/2.0
(N2,Vy2,Vz2,T2,My2,Mz2)= getInternalForcesEndNode(beam3d)

ratio30= abs(N1)+abs(N2)
ratio31= abs(Vy1)+abs(Vy2)
ratio32= abs((Vz1-Vz1Teor)/Vz1)+abs(Vz2)
ratio33= abs(T1)+abs(T2)
ratio34= abs((My1-My1Teor)/My1)+abs(My2)
ratio35= abs(Mz1)+abs(Mz1)

phaseRatios= [ratio30,ratio31,ratio32,ratio33,ratio34,ratio35]
ratios.extend(phaseRatios)

# print("RF= ",RF)
# printResults(N1,Vy1,Vz1,T1,My1,Mz1,N2,Vy2,Vz2,T2,My2,Mz2,phaseRatios,'3')

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
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
