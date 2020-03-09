# -*- coding: utf-8 -*-
'''Stiff beam between two nodes. Home made test'''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math


# Geometry
L= 15 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude (kN)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod1= nodes.newNodeXYZ(0,0.0,0.0)
nod2= nodes.newNodeXYZ(L,0.0,0.0)


# Constraints
modelSpace.fixNode('000_000',nod1.tag)
modelSpace.fixNode('F00_000',nod2.tag)
rr= modelSpace.setHugeTrussBetweenNodes(nod1.tag,nod2.tag)

# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(nod2.tag,xc.Vector([F,0,0,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)


deltaX= nod2.getDisp[0]
rr.getResistingForce()
N=rr.getN()

''' 
print deltaX
print N
  '''

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if deltaX<1e-10:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
