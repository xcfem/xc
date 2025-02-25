# -*- coding: utf-8 -*-
''' Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

K= 1000 # Spring constant
l= 1e-7 # Distance between nodes
F= 1 # Force magnitude

import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics1D(nodeHandler)
# nodeHandler.dimSpace= 1 # One coordinate for each node.
# nodeHandler.numDOFs= 1 # One degree of freedom for each node.

n1= modelSpace.newNodeX(1)
n2= modelSpace.newNodeX(1.0+l)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor= preprocessor, name= "elast", E= K)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast.name
elements.dimElem= 1 #Element dimension.
zl= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(n1.tag,0,0.0) # Node 1


# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= ts.name
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#lPatterns.currentLoadPattern= "0"
nl= lp0.newNodalLoad(n2.tag,xc.Vector([F]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
nodeHandler.calculateNodalReactions(True,1e-7)

deltax= n2.getDisp[0] 
R= n1.getReaction[0]

ratio1= -R/F
ratio2= abs(((K*deltax)-F)/F)

''' 
print("R= ",R)
print("dx= ",deltax)
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
'''
  
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (ratio2<1e-11) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayLocalAxes()
