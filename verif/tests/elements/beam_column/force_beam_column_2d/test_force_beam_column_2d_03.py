# -*- coding: utf-8 -*-
''' Home made test. Horizontal cantilever under tension bending moment at its end.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Geometry
width= .05
depth= .1
A= width*depth
E= 210e9
I= width*depth**3/12
nu= 0.3
G= E/(2*(1+nu))
L= 1.5 # Bar length (m)

# Load
M= 1.5e3 # Load magnitude en N m

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
n1= nodes.newNodeXY(0,0.0)
n2= nodes.newNodeXY(L,0.0)


# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

# Materials definition
section= typical_materials.defElasticShearSection2d(preprocessor, "section",A,E,G,I,1.0)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name # Coordinate transformation for the new elements
elements.defaultMaterial= section.name
beam2d= elements.newElement("ForceBeamColumn2d",xc.ID([n1.tag,n2.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000(n1.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([0,0,M]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)
# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
result= analysis.analyze(10)


nodes.calculateNodalReactions(True,1e-7) 
delta= n2.getDisp[1]  # z displacement of node 2
deltateor= (M*L**2/(2*E*I))
ratio1= (abs((delta-deltateor)/deltateor))
theta= n2.getDisp[2]  # z rotation of the node
thetateor= (M*L/(E*I))
ratio2= (abs((theta-thetateor)/thetateor))
RM= n1.getReaction[2] 

elements= preprocessor.getElementHandler

beam2d.getResistingForce()
scc= beam2d.getSections()[0]

Vscc= scc.getStressResultantComponent("Vy")
M1= scc.getStressResultantComponent("Mz")

ratio3= (abs((M+RM)/M))
ratio4= (abs((M-M1)/M))

# Check getM1 and getM2 (LP 28/04/2024).
M1= beam2d.getM1
MRef= M
M2= beam2d.getM2
ratio5= math.sqrt((MRef-M1)**2+(MRef-M2)**2)

# Check getV1 and getV2 (LP 28/04/2024).
V1= beam2d.getV1
V2= beam2d.getV2
ratio6= math.sqrt(V1**2+V2**2)

''' 
print("delta: ",delta)
print("deltaTeor: ",deltateor)
print("ratio1= ",ratio1)
print("theta: ",theta)
print("thetaTeor: ",thetateor)
print("ratio4= ",ratio4)
print("M= ",M)
print("RM= ",RM)
print("ratio2= ",ratio2)
print("M1= ",M1)
print("ratio3= ",ratio3)
print('MRef= ', MRef/1e3, 'M1= ', M1/1e3, ' M2= ', M2/1e3, ' ratio5= ', ratio5)
print('V1Ref= ', 0/1e3, 'V1= ', V1/1e3, ' V2= ', V2/1e3, ' ratio6= ', ratio6)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-10) & (abs(ratio4)<1e-10) & (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
  
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()#setsToDisplay= [columnSet, pileSet])
# # oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)
# oh.displayLocalAxes()
# oh.displayLoads()
# # oh.displayIntForcDiag('N')
# oh.displayIntForcDiag('M')
# oh.displayIntForcDiag('V')
# #oh.displayIntForcDiag('T')
# #oh.displayLocalAxes()
