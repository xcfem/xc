# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

K= 1000.0 # Spring constant
l= 100.0 # Distance between nodes
F= 1.0 # Force magnitude
D= F/K # Displacement magnitude

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials



# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

nodA= nodes.newNodeXY(0,0)
nodB= nodes.newNodeXY(l,0.0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",K)
    
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast"
elements.dimElem= 2 # Dimension of element space
elements.defaultTag= 1 #First node number.
spring= elements.newElement("Spring",xc.ID([nodA.tag,nodB.tag]))
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(nodA.tag,0,0.0) # Node 1
spc= constraints.newSPConstraint(nodA.tag,1,0.0)
spc= constraints.newSPConstraint(nodB.tag,1,0.0) # Node 2

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#lPatterns.currentLoadPattern= "0"
lp0.newSPConstraint(nodB.tag,0,D)

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)
# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

deltax= nodB.getDisp[0] 
deltay= nodB.getDisp[1] 
R= nodA.getReaction[0] 

spring.getResistingForce()
Ax= spring.getMaterial().getStrain() # Spring elongation

ratio1= (F+R)/F
ratio2= (F-(K*deltax))/F
ratio3= (Ax-deltax)/deltax

''' 
print "R= ",R
print "dx= ",deltax
print "dy= ",deltay
print "Ax= ",Ax
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
   '''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
