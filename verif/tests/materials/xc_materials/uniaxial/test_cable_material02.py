# -*- coding: utf-8 -*-
from __future__ import print_function
# Test from Ansys manual
# Reference:  Strength of Material, Part I, Elementary Theory and Problems, pg. 26, problem 10
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


E= 30e6 # Young modulus (psi)
l= 10 # Cable length in inches
sigmaPret= 1000.0 # Prestressing force (pounds)
area= 2.0
fPret= sigmaPret*area # Prestressing force (pounds)

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)


nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(l,0.0)

# Materials definition
cable= typical_materials.defCableMaterial(preprocessor, "cable",E,sigmaPret,0.0)
    
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= cable.name
elements.dimElem= 2 # Dimension of element space
truss= elements.newElement("CorotTruss",xc.ID([1,2]))
truss.sectionArea= area
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(1,0,0.0) # Node 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(2,0,0.0) # Node 2
spc= constraints.newSPConstraint(2,1,0.0)



# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R1= nodes.getNode(2).getReaction[0] 
R2= nodes.getNode(1).getReaction[0] 


ratio1= (abs(R1-fPret)/fPret)
ratio2= (abs(R2+fPret)/fPret)
    
''' 
print("R1= ",R1)
print("fPret= ", fPret)
print("ratio1= ",ratio1)
print("R2= ",R2)
print("ratio2= ",(ratio2))
   '''
    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
