# -*- coding: utf-8 -*-
from __future__ import print_function
''' Test from Ansys manual
    Reference:  Strength of Material, Part I, Elementary Theory & Problems, pg. 26, problem 10

    Test the UMFPACK solver.
'''

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
l= 10 # Bar length in inches
a= 0.3*l # Length of tranche a
b= 0.3*l # Length of tranche b
F1= 1000 # Force magnitude 1 (pounds)
F2= 1000/2 # Force magnitude 2 (pounds)

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(0.0,l-a-b)
n3= nodes.newNodeXY(0.0,l-a)
n4= nodes.newNodeXY(0.0,l)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
    
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast.name
elements.dimElem= 2 # Dimension of element space
#  sintaxis: truss[<tag>] 
truss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
truss.sectionArea= 1
truss= elements.newElement("Truss",xc.ID([n2.tag,n3.tag]))
truss.sectionArea= 1
truss= elements.newElement("Truss",xc.ID([n3.tag,n4.tag]))
truss.sectionArea= 1
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(n1.tag,0,0.0) # Node 1
spc= constraints.newSPConstraint(n1.tag,1,0.0)
spc= constraints.newSPConstraint(n4.tag,0,0.0) # Node 4
spc= constraints.newSPConstraint(n4.tag,1,0.0)
spc= constraints.newSPConstraint(n2.tag,0,0.0) # Node 2
spc= constraints.newSPConstraint(n3.tag,0,0.0) # Node 3


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([0,-F2]))
lp0.newNodalLoad(n3.tag,xc.Vector([0,-F1]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
import os
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
  pth= "."

solProc= predefined_solutions.PenaltyModifiedNewtonUMF(feProblem)
result= solProc.solve()

nodes.calculateNodalReactions(True,1e-7)
R1= n4.getReaction[1] 
R2= n1.getReaction[1] 


ratio1= R1/900
ratio2= R2/600
    
''' 
print("R1= ",R1)
print("R2= ",R2)
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
'''
    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
