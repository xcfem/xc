# -*- coding: utf-8 -*-
''' Home made test.'''
from __future__ import print_function
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

def getInternalForcesBeginNode(elem):
    ''':return: internal forces on the element's first node.''' 
    return (elem.getN1,elem.getV1,elem.getM1)

def getInternalForcesEndNode(elem):
    ''':return: internal forces on the element's last node.''' 
    return (elem.getN2,elem.getV2,elem.getM2)

def printResults(N1,V1,M1,N2,V2,M2,phaseRatios,phase):
    ratioMsg= 'ratio'+str(phase)
    print('N1= ', N1, ' N2= ', N2 )
    print(ratioMsg+'0= ', phaseRatios[0])
    print('V1= ',V1, 'V2= ',V2 )
    print(ratioMsg+'1= ', phaseRatios[1])
    print('M1= ',M1, 'M2= ', M2)
    print(ratioMsg+'2= ', phaseRatios[2])

                     
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
n1= nodes.newNodeXY(0,0.0)
n2= nodes.newNodeXY(L,0.0)

lin= modelSpace.newLinearCrdTransf("lin")
# Materials
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz; sectionProperties.Alpha= 5/6
section= typical_materials.defElasticShearSectionFromMechProp2d(preprocessor, "section",sectionProperties)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
beam2d= elements.newElement("ElasticTimoshenkoBeam2d",xc.ID([n1.tag,n2.tag]))

modelSpace.fixNode000(n1.tag)

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([F,0,0])) # Positive force along x axis
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution 0 N
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= beam2d.getResistingForce()
(N1,V1,M1)= getInternalForcesBeginNode(beam2d)
NTeor= F
(N2,V2,M2)= getInternalForcesEndNode(beam2d)

ratios= list()
ratio0= abs((N1-NTeor)/NTeor)+abs((N2-NTeor)/NTeor)
ratio1= abs(V1)+abs(V2)
ratio2= abs(M1)+abs(M2)
phaseRatios= [ratio0,ratio1,ratio2]
ratios.extend(phaseRatios)

'''
print('N1= ', N1)
print('ratio0= ', ratio0)
print('V1= ', V1)
print('ratio1= ', ratio1)
print('M1= ', M1)
print('ratio2= ', ratio2)
print('RF= ',RF)
printResults(N1,V1,M1,N2,V2,M2,phaseRatios,'')
'''

lp0.removeFromDomain()
lp1= modelSpace.newLoadPattern(name= '1')
lp1.newNodalLoad(n2.tag,xc.Vector([0,F,0])) # Positive force along y axis
# We add the load case to domain.
modelSpace.addLoadCaseToDomain("1")

# Solution 1 V
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= beam2d.getResistingForce()
(N1,V1,M1)= getInternalForcesBeginNode(beam2d)
M1Teor= F*L
(N2,V2,M2)= getInternalForcesEndNode(beam2d)

ratio10= abs(N1)+abs(N2)
ratio11= abs((V1-F)/F)+abs((V2-F)/F)
ratio12= abs((M1Teor-M1)/M1Teor)+abs(M2)

'''
print('N1= ', N1)
print('ratio10= ', ratio10)
print('V1= ', V1)
print('ratio11= ', ratio11)
print('M1= ', M1)
print('ratio12= ', ratio12)
'''

phaseRatios= [ratio10,ratio11,ratio12]
ratios.extend(phaseRatios)

# print("RF= ",RF)
# printResults(N1,V1,M1,N2,V2,M2,phaseRatios,'1')

lp1.removeFromDomain()
lp2= modelSpace.newLoadPattern(name= '2')
lp2.newNodalLoad(n2.tag,xc.Vector([0,0,F])) # Positive moment about z axis
# We add the load case to domain.
modelSpace.addLoadCaseToDomain("2")

# Solution 2 M
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

RF= beam2d.getResistingForce()
(N1,V1,M1)= getInternalForcesBeginNode(beam2d)
(N2,V2,M2)= getInternalForcesEndNode(beam2d)

ratio20= abs(N1)+abs(N2)
ratio21= abs(V1)+abs(V2)
ratio22= abs((M1-F)/F)+abs((M2-F)/F)

phaseRatios= [ratio20,ratio21,ratio22]
ratios.extend(phaseRatios)

# print("RF= ",RF)
# printResults(N1,V1,M1,N2,V2,M2,phaseRatios,'2')

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
