# -*- coding: utf-8 -*-
'''Home made test
steel02 material presstressing.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 1.0 # Bar length (m)
E= 190e9 # Elastic modulus expressed in MPa
A= 140e-6 # bar area expressed in square meters
fMax= 1860e6 # Maximum unit load of the material expressed in MPa.
fy= 1171e6 # Yield stress of the material expressed in Pa.
tInic= 0.75**2*fMax # Effective prestress (0.75*P0 y 25% prestress losses).

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
#from postprocess import output_handler

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
n1= nodes.newNodeXY(0.0,0.0)
n2= nodes.newNodeXY(L,0.0)


# Materials definition
prestressingSteel= typical_materials.defSteel02(preprocessor, "prestressingSteel",E,fy,0.001,tInic)
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= prestressingSteel.name
elements.dimElem= 2 # Dimension of element space
truss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
truss.sectionArea= A

    
# Constraints
constraints= preprocessor.getBoundaryCondHandler

#
spc= constraints.newSPConstraint(n1.tag,0,0.0)
spc= constraints.newSPConstraint(n1.tag,1,0.0)
spc= constraints.newSPConstraint(n2.tag,0,0.0)
spc= constraints.newSPConstraint(n2.tag,1,0.0)

solProc= predefined_solutions.PenaltyNewtonRaphson(feProblem)
result= solProc.solve()

elements= preprocessor.getElementHandler

truss.getResistingForce()
ratio= (tInic*A-truss.getN())/(tInic*A)

'''
# print{"force= ",getN()
print("prestressing force= ",(tInic*A))
print("strain= ",getStrain)
print("ratio= ",(ratio)})
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<0.02:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Graphic stuff.
#oh= output_handler.OutputHandler(modelSpace)
#oh.displayFEMesh(setsToDisplay= xcSets)
#oh.displayIntForcDiag(itemToDisp= 'N', setToDisplay= modelSpace.getSet('total'))

