# -*- coding: utf-8 -*-

#Test from Ansys manual
#Reference:  Strength of Materials, Part I, Elementary Theory and Problems, pg. 26, problem 10

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

E= 30e6 #Young modulus (psi)
l= 10 #Bar length in inches
a= 0.3*l #Length of tranche a
b= 0.3*l #Length of tranche b
F1= 1000 #Force magnitude 1 (pounds)
F2= 1000/2 #Force magnitude 2 (pounds)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(0,l-a-b)
n3= nodes.newNodeXY(0,l-a)
n4= nodes.newNodeXY(0,l)

elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''

elements= preprocessor.getElementHandler
elements.dimElem= 2 #Bars defined ina a two dimensional space.
elements.defaultMaterial= "elast"
truss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
truss.sectionArea= 1
truss= elements.newElement("Truss",xc.ID([n2.tag,n3.tag]))
truss.sectionArea= 1
truss= elements.newElement("Truss",xc.ID([n3.tag,n4.tag]))
truss.sectionArea= 1

constraints= preprocessor.getBoundaryCondHandler
#Constrain the displacement of node 1.
spc1= constraints.newSPConstraint(n1.tag,0,0.0)
spc2= constraints.newSPConstraint(n1.tag,1,0.0)
#Constrain the displacement of node 4.
spc3= constraints.newSPConstraint(n4.tag,0,0.0)
spc4= constraints.newSPConstraint(n4.tag,1,0.0)
#Constrain the displacement of node 2 in X axis (gdl 0).
spc5= constraints.newSPConstraint(n2.tag,0,0.0)
#Constrain the displacement of node 3 in X axis (gdl 0).
spc6= constraints.newSPConstraint(n3.tag,0,0.0)

loadHandler= preprocessor.getLoadHandler
#Load pattern container:
lPatterns= loadHandler.getLoadPatterns
#time series for the load pattern:
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(n2.tag,xc.Vector([0,-F2]))
lp0.newNodalLoad(n3.tag,xc.Vector([0,-F1]))

#Add the load pattern to the domain.
lPatterns.addToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R1= n4.getReaction[1]
R2= n1.getReaction[1]

ratio1= (R1-900)/900
ratio2= (R2-600)/600

#print "R1= ",R1
#print "R2= ",R2
#print "ratio1= ",ratio1
#print "ratio2= ",ratio2

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


