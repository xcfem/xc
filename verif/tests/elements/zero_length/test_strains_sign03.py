# -*- coding: utf-8 -*-
# home made test

# ZeroLengthSection element sign criteria.

#     Axial and shear forces have the same direction and sense that
#     the local axes.
#     Mx torque and My bending moment have the same direction and sense
#     that the local axes.
#     Bending moment Mz has the same direction and its sense is the OPPOSITE to local Z axis.
#     Section's y axis is element z axis.

from __future__ import print_function
from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from postprocess import prop_statistics

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   

# Positive My (section)
epsilon= 3.5e-3
epsilon1= -epsilon
epsilon2= -epsilon
epsilon3= epsilon
epsilon4= epsilon

# Read section definition from file.
import os
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
  pth= "."
exec(open(pth+"/../../aux/four_fiber_section.py").read())

sigma= E*epsilon
F= sigma*fiberArea
N= 0.0
My= 4*F*widthOverZ/2.0
Mz= 0.0


fourFibersSection.revertToStart()
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 # First node number.
nod= nodes.newNodeXYZ(0.0,0.0,0.0)
nod= nodes.newNodeXYZ(0.0,0.0,0.0)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= fourFibersSection.name
elements.dimElem= 1 # Dimension of element space
zl= elements.newElement("ZeroLengthSection",xc.ID([1,2]))

# Constraints
modelSpace.fixNode000_000(1)
spc= modelSpace.constraints.newSPConstraint(2,1,0.0)
spc= modelSpace.constraints.newSPConstraint(2,2,0.0)
spc= modelSpace.constraints.newSPConstraint(2,3,0.0)


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(2,xc.Vector([N,0,0,0,My,Mz])) # Section's y axis is element z axis.

# Add the load pattern to the domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

zl.getResistingForce()
section= zl.getSection()

N1= section.getFibers().getResultant()
Mz1= section.getFibers().getMz(0.0)
My1= section.getFibers().getMy(0.0)

section.setupFibers()
RR= section.getStressResultant()
R1= xc.Vector([RR[0],RR[2],RR[1]])

vTeor= xc.Vector([N,My,Mz])
v0= xc.Vector([N0,My0,Mz0])
v1= xc.Vector([N1,My1,Mz1])

import math
error= math.sqrt((vTeor-v0).Norm2()+(vTeor-v1).Norm2()+(vTeor-R0).Norm2()+(vTeor-R1).Norm2())

'''
print("I= ", zl.getIVector)
print("K= ", zl.getKVector)
print("EA= ",E*A)
print("EIz= ",E*Iz)
print("EIy= ",E*Iy)
print(fourFibersSection.getInitialTangentStiffness())

print("v0= ",v0)
print("v1= ",v1)
print("R0= ",R0)
print("R1= ",R1)
print("vTeor= ", vTeor)
print("error= ", error)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error < 1e-3):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
