# -*- coding: utf-8 -*-
''' Test from Ansys manual
 Reference:  Strength of Material, Part I, Elementary Theory and Problems, pg. 26, problem 10.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

E= 30e6 # Young modulus (psi)
l= 10 # Cable length in inches
sigmaPret= 1000 # Prestressing force final (pounds)
sigmaPretA= sigmaPret/2 # Prestressing force parcial (pounds)
area= 2
fPret= sigmaPret*area # Force magnitude de tesado final
fPretA= fPret/2 # Force magnitude de tesado parcial

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0.0)

# Materials definition
cable= typical_materials.defCableMaterial(preprocessor, "cable",E,sigmaPretA,0.0)

    
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= cable.name
elements.dimElem= 2 # Dimension of element space
#  sintaxis: truss[<tag>] 
truss= elements.newElement("Truss",xc.ID([n1.tag, n2.tag]))
truss.sectionArea= area
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(n1.tag, 0,0.0) # Node 1
spc= constraints.newSPConstraint(n1.tag, 1,0.0)
spc= constraints.newSPConstraint(n2.tag, 0,0.0) # Node 2
spc= constraints.newSPConstraint(n2.tag, 1,0.0)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)
R1A= n2.getReaction[0] 
R2A= n1.getReaction[0] 


ratio1A= ((R1A-fPretA)/fPretA)
ratio2A= ((R2A+fPretA)/fPretA)

truss.getMaterial().prestress= sigmaPret

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)
R1B= n2.getReaction[0] 
R2B= n1.getReaction[0]

ratio1B= ((R1B-fPret)/fPret)
ratio2B= ((R2B+fPret)/fPret)

''' 
print("R1A= ",R1A)
print("R2A= ",R2A)
print("ratio1A= ",(ratio1A))
print("ratio2A= ",(ratio2A))
print("R1B= ",R1B)
print("R2B= ",R2B)
print("ratio1B= ",(ratio1B))
print("ratio2B= ",(ratio2B))
   '''
    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1A)<1e-5) & (abs(ratio2A)<1e-5) & (abs(ratio1B)<1e-5) & (abs(ratio2B)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
