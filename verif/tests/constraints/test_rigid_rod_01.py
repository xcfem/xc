# -*- coding: utf-8 -*-
'''Rigid rod between two nodes. Home made test.'''

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
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


# Constraints
constraints= preprocessor.getBoundaryCondHandler

modelSpace.fixNode000(1)
rr= preprocessor.getBoundaryCondHandler.newRigidRod(1,2)


# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([-F,F,-F]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)


# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)


nodes= preprocessor.getNodeHandler
 
nod2= nodes.getNode(2)
deltaX= nod2.getDisp[0]
deltaY= nod2.getDisp[0]
deltaZ= nod2.getDisp[0]  # Node 2 displacement


ratio1= math.sqrt(deltaX**2+deltaY**2+deltaZ**2)

''' 
    print deltaX
    print deltaY
    print deltaZ
    print ratio1
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if ratio1<1e-11:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
