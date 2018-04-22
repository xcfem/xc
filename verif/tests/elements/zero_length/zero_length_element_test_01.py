# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

K= 1000 # Spring constant
l= 1e-7 # Distance between nodes
F= 1 # Force magnitude

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
nodes.dimSpace= 1 # One coordinate for each node.
nodes.numDOFs= 1 # One degree of freedom for each node.

nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(1,0)
nod= nodes.newNodeXY(1.0+l,0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",K)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast"
elements.dimElem= 1 #Element dimension.
elements.defaultTag= 1
zl= elements.newElement("ZeroLength",xc.ID([1,2]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(1,0,0.0) # Node 1


# Loads definition
cargas= preprocessor.getLoadHandler
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
nl= lp0.newNodalLoad(2,xc.Vector([F]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)



nodes.calculateNodalReactions(True,1e-7)

nod2= nodes.getNode(2)
deltax= nod2.getDisp[0] 
nod1= nodes.getNode(1)
R= nod1.getReaction[0] 

elements= preprocessor.getElementHandler

elem1= elements.getElement(1)
elem1.getResistingForce()



ratio1= -R/F
ratio2= abs(((K*deltax)-F)/F)

''' 
print "R= ",R
print "dx= ",deltax
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
   '''
  
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (ratio2<1e-11) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
