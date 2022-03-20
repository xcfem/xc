# -*- coding: utf-8 -*-
from __future__ import print_function
''' Frame analysis taken from example «PDelta-3» of the Ansys verification manual.'''
__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#           P             P
#          |             |
#     2____v___4      6__v______8
#      |      |        |       |
#      |      |        |       |
#      |      |        |       |         
#     1|      |3      5|       |7

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

B= 100 # Frame width expressed in inches
H= 100 # Frame height expressed in inches
# Section mass properties (IPE-80)
Es= 29e6 # Young modulus (psi)
area= 1 # Section area expressed in in2
Iz= 1/12.0 # Cross-section moment of inertia expressed in in4
P= 1000 # Load (force-pounds).

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
n1= nodes.newNodeXY(0,0) # First frame
n2= nodes.newNodeXY(0,H)
n3= nodes.newNodeXY(B,0)
n4= nodes.newNodeXY(B,H)
n5= nodes.newNodeXY(2*B,0) # Second frame.
n6= nodes.newNodeXY(2*B,H)
n7= nodes.newNodeXY(2*B+B,0)
n8= nodes.newNodeXY(2*B+B,H)


lin= modelSpace.newLinearCrdTransf("lin")
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= area; sectionProperties.E= Es
sectionProperties.I= Iz
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
beam2d01= elements.newElement("ElasticBeam2d",xc.ID([n1.tag, n2.tag]))
beam2d02= elements.newElement("ElasticBeam2d",xc.ID([n2.tag, n4.tag]))
beam2d03= elements.newElement("ElasticBeam2d",xc.ID([n4.tag, n3.tag]))
beam2d04= elements.newElement("ElasticBeam2d",xc.ID([n5.tag, n6.tag]))
beam2d05= elements.newElement("ElasticBeam2d",xc.ID([n6.tag, n8.tag]))
beam2d06= elements.newElement("ElasticBeam2d",xc.ID([n8.tag, n7.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000(n1.tag)
modelSpace.fixNode000(n3.tag)
modelSpace.fixNode000(n5.tag)
modelSpace.fixNode000(n7.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("beam2d_point_load")
eleLoad.elementTags= xc.ID([beam2d02.tag])
eleLoad.transComponent= -P
eleLoad.x= 0.5
eleLoad= lp0.newElementalLoad("beam2d_point_load")
eleLoad.elementTags= xc.ID([beam2d05.tag])
eleLoad.transComponent= -P
eleLoad.x= 0.25
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

theta2= 0.0 # Rotation of the node 2
theta2Teor= -0.08620 # Theorethical value of the rotation of the node 2
ratioTheta2= 0.0
M12= 0.0 # Bending moment acting on element 1 front end.
M12Teor= 4166.7 # Theorethical value of the bending moment
                # acting on on element 1 front end.
ratioM12= 0.0
M21= 0.0 # Bending moment acting on element 2 back end.
M21Teor= -8333.3 # Theorethical value of the bending moment
                 # acting on element 2 back end.
ratioM21= 0.0 # Rotation of the node
Q= 0.0 # Shear on element 1
QTeor= 125 # Theorethical value of the shear on element 1
ratioQ= 0.0
delta6= 0.0 # Displacement of node 6
delta6Teor= 1.385 # Theoretical value of desplazamiento horizontal of the node 6
ratioDelta6= 0.0
theta6= 0.0 # Rotation of the node 6
theta6Teor= -0.0924 # Theorethical value of the rotation of the node 6
ratioTheta6= 0.0
theta8= 0.0 # Rotation of the node 6
theta8Teor= 0.0369 # Theorethical value of the rotation of the node 6
ratioTheta8= 0.0
M61= 0.0 # Bending moment acting on element 4 back end.
M61Teor= 2455.4 # Theoretical value of bending moment
                # acting on element 4 back end.
ratioM61= 0.0
Q4= 0.0 # Shear on element 1
Q4Teor= 93.75 # Theoretical value of shear on element 1
ratioQ4= 0.0
M51= 0.0 # bending moment acting on back end of the element 5
M51Teor= -6919.6 # Theoretical value of bending moment acting on back end of the element 5
ratioM51= 0.0
M71= 0.0 # Bending moment acting on element 6 back end.
M71Teor= 3794.6 # Theoretical value of bending moment
                # acting on element 6 back end.
ratioM71= 0.0
M52= 0.0 # bending moment acting on back end of the element 5
M52Teor= -5580.4 # Theoretical value of bending moment acting
                 # on back end of the element 5
ratioM52= 0.0


theta2= n2.getDisp[2]
delta6= n6.getDisp[0]
theta6= n6.getDisp[2]
theta8= n8.getDisp[2]

beam2d01.getResistingForce()
Q= beam2d01.getV1
M12= beam2d01.getM1

beam2d02.getResistingForce()
M21= beam2d02.getM1

beam2d04.getResistingForce()
Q4= beam2d04.getV1
M61= beam2d04.getM1

beam2d05.getResistingForce()
M51= beam2d05.getM1
M52= beam2d05.getM2

beam2d06.getResistingForce()
M71= beam2d06.getM2

ratioTheta2= abs((theta2-theta2Teor)/theta2Teor)
ratioDelta6= abs((delta6-delta6Teor)/delta6Teor)
ratioM12= abs((M12-M12Teor)/M12Teor)
ratioM21= abs((M21-M21Teor)/M21Teor)
ratioQ= abs((Q-QTeor)/QTeor)
ratioTheta6= abs((theta6-theta6Teor)/theta6Teor)
ratioTheta8= abs((theta8-theta8Teor)/theta8Teor)
ratioM61= abs((M61-M61Teor)/M61Teor)
ratioQ4= abs((Q4-Q4Teor)/Q4Teor)
ratioM51= abs((M51-M51Teor)/M51Teor)
ratioM71= abs((M71-M71Teor)/M71Teor)
ratioM52= abs((M52-M52Teor)/M52Teor)

''' 
print("theta2= ",theta2)
print("theta2Teor= ",theta2Teor)
print("ratioTheta2= ",ratioTheta2)
print("M12= ",M12)
print("M12Teor= ",M12Teor)
print("ratioM12= ",ratioM12)
print("M21= ",M21)
print("M21Teor= ",M21Teor)
print("ratioM21= ",ratioM21)
print("Q= ",Q)
print("QTeor= ",QTeor)
print("ratioQ= ",ratioQ)
print("delta6= ",delta6)
print("delta6Teor= ",delta6Teor)
print("ratioDelta6= ",ratioDelta6)
print("theta6= ",theta6)
print("theta6Teor= ",theta6Teor)
print("ratioTheta6= ",ratioTheta6)
print("theta8= ",theta8)
print("theta8Teor= ",theta8Teor)
print("ratioTheta8= ",ratioTheta8)
print("M61= ",M61)
print("M61Teor= ",M61Teor)
print("ratioM61= ",ratioM61)
print("Q4= ",Q4)
print("Q4Teor= ",Q4Teor)
print("ratioQ4= ",ratioQ4)
print("M51= ",M51)
print("M51Teor= ",M51Teor)
print("ratioM51= ",ratioM51)
print("M71= ",M71)
print("M71Teor= ",M71Teor)
print("ratioM71= ",ratioM71)
print("M52= ",M52)
print("M52Teor= ",M52Teor)
print("ratioM52= ",ratioM52)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratioTheta2<1e-3) & (ratioDelta6<1e-3) & (ratioM12<1e-4) & (ratioM21<1e-4) & (ratioQ<1e-4) & (ratioTheta6<1e-3) & (ratioTheta8<5e-3) & (ratioM61<1e-4) & (ratioQ4<1e-4) & (ratioM51<1e-4) & (ratioM71<5e-4) & (ratioM52<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
