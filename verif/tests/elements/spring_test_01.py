# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

K= 1000 # Spring constant
l= 100 # Distance between nodes
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
# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(l,0.0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",K)
    
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast"
elements.dimElem= 2 # Dimension of element space
elements.defaultTag= 1
#  sintaxis: Spring[<tag>] 
spring= elements.newElement("Spring",xc.ID([1,2]));
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(1,0,0.0) # Node 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(2,1,0.0) # Node 2


# Loads definition
cargas= preprocessor.getLoadHandler
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
lPattern= "0"
lp0= casos.newLoadPattern("default",lPattern)
#casos.currentLoadPattern= lPattern
# de paso comprobamos que las cargas se acumulan
lp0.newNodalLoad(2,xc.Vector([F/2.0,0]))
lp0.newNodalLoad(2,xc.Vector([F/2.0,0]))
casos.addToDomain(lPattern) # Append load pattern to domain.
# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)



nodes.calculateNodalReactions(True,1e-7)

nod2= nodes.getNode(2)
deltax= nod2.getDisp[0] 
deltay= nod2.getDisp[1] 
nod1= nodes.getNode(1)
R= nod1.getReaction[0] 

elements= preprocessor.getElementHandler
elem1= elements.getElement(1)
elem1.getResistingForce()
Ax= elem1.getMaterial().getStrain() # Spring elongation


ratio1= (F+R/F)
ratio2= ((K*deltax-F)/F)
ratio3= ((deltax-Ax)/Ax)
''' 
print "R= ",R
print "dx= ",deltax
print "dy= ",deltay
print "Ax= ",Ax
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
 '''
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
