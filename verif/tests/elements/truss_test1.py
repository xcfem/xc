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
from solution import predefined_solutions
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
nodes= preprocessor.getNodeLoader

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nodes.defaultTag= 1 #Number for next node will be 1.
nodes.newNodeXYZ(0,0,0)
nodes.newNodeXYZ(0,l-a-b,0)
nodes.newNodeXYZ(0,l-a,0)
nodes.newNodeXYZ(0,l,0)

elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''

elements= preprocessor.getElementLoader
elements.dimElem= 2 #Bars defined ina a two dimensional space.
elements.defaultMaterial= "elast"
elements.defaultTag= 1 #Tag for the next element.
truss= elements.newElement("Truss",xc.ID([1,2]));
truss.area= 1
truss= elements.newElement("Truss",xc.ID([2,3]));
truss.area= 1
truss= elements.newElement("Truss",xc.ID([3,4]));
truss.area= 1

constraints= preprocessor.getConstraintLoader
#Constrain the displacement of node 1.
spc= constraints.newSPConstraint(1,0,0.0)
spc= constraints.newSPConstraint(1,1,0.0)
#Constrain the displacement of node 4.
spc= constraints.newSPConstraint(4,0,0.0)
spc= constraints.newSPConstraint(4,1,0.0)
#Constrain the displacement of node 2 in X axis (gdl 0).
spc= constraints.newSPConstraint(2,0,0.0)
#Constrain the displacement of node 3 in X axis (gdl 0).
spc= constraints.newSPConstraint(3,0,0.0)

cargas= preprocessor.getLoadLoader
#Load pattern container:
casos= cargas.getLoadPatterns
#time series for the load pattern:
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,-F2]))
lp0.newNodalLoad(3,xc.Vector([0,-F1]))

#Add the load pattern to the domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)

nodes.calculateNodalReactions(True)
R1= nodes.getNode(4).getReaction[1]
R2= nodes.getNode(1).getReaction[1]

ratio1= (R1-900)/900
ratio2= (R2-600)/600

#print "R1= ",R1
#print "R2= ",R2
#print "ratio1= ",ratio1
#print "ratio2= ",ratio2

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


