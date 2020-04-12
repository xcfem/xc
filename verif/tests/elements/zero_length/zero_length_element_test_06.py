# -*- coding: utf-8 -*-
''' home made test. Verifies that the "ZeroLength" element gives right
 results when not aligned with global axes.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

K= 1000 # Spring constant
F= 1 # Force magnitude

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Model definition
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(1,1)
nod= nodes.newNodeXY(1,1)

# Materials definition
k= typical_materials.defElasticMaterial(preprocessor, "k",K)


''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "k"
elements.dimElem= 3 # Dimension of element space
elements.deraultTag= 1
zl1= elements.newElement("ZeroLength",xc.ID([1,2]))
zl1.setupVectors(xc.Vector([1,1,0]),xc.Vector([-1,1,0]))
zl1.clearMaterials()
zl1.setMaterial(0,"k")
zl2= elements.newElement("ZeroLength",xc.ID([1,2]))
zl2.setupVectors(xc.Vector([-1,1,0]),xc.Vector([-1,-1,0]))
zl2.setMaterial(0,"k")

# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(1,0,0.0) # Node 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(1,2,0.0)
spc= constraints.newSPConstraint(2,2,0.0) # Node 2


# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F*math.sin(math.pi/4),F*math.cos(math.pi/4),0]))

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)



nodes.calculateNodalReactions(True,1e-7)

nod2= nodes.getNode(2)
deltax= nod2.getDisp[0]
deltay= nod2.getDisp[1]
deltat= nod2.getDisp[2] 
nod1= nodes.getNode(1)
RX= nod1.getReaction[0]
RY= nod1.getReaction[1]
RT= nod1.getReaction[2] 


R= RX**2+RY**2
d= math.sqrt(deltax**2+deltay**2)
ratio1= R/F-1
ratio2= (K*d)/F-1

''' 
print "R= ",R
print "d= ",d
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
   '''
  
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
