# -*- coding: utf-8 -*-
'''Test taken from example 5-001 of SAP2000 verification manual.'''

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

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25,0.0)

nodes= preprocessor.getNodeLoader 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
nod9= nodes.newNodeIDXYZ(9,0,0,0)
nod10= nodes.newNodeIDXYZ(10,1,0,0)
nod11= nodes.newNodeIDXYZ(11,1,1,0)
nod12= nodes.newNodeIDXYZ(12,0,1,0)
nod13= nodes.newNodeIDXYZ(13,0,0,1)
nod14= nodes.newNodeIDXYZ(14,1,0,1)
nod15= nodes.newNodeIDXYZ(15,1,1,1)
nod16= nodes.newNodeIDXYZ(16,0,1,1)


elements= preprocessor.getElementLoader
elements.defaultMaterial= "elast3d"
elements.defaultTag= 1 #Tag for the next element.
brick= elements.newElement("Brick",xc.ID([9,10,11,12,13,14,15,16]));

constraints= preprocessor.getConstraintLoader
#Constrain the displacement of node 1.

nod9.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod10.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod11.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod12.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
lp0.newNodalLoad(13, xc.Vector([0,0,-1]))
lp0.newNodalLoad(14, xc.Vector([0,0,-1]))
lp0.newNodalLoad(15, xc.Vector([0,0,-1]))
lp0.newNodalLoad(16, xc.Vector([0,0,-1]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)

nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader 
R9= nod9.getReaction
R10= nod10.getReaction
R11= nod11.getReaction
R12= nod12.getReaction


R= R9+R10+R11+R12
ratio1= (R-xc.Vector([0,0,4])).Norm()

''' 
print "R9= ",R9
print "R10= ",R10
print "R11= ",R11
print "R12= ",R12
print "R= ",R
print "ratio1= ",ratio1
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
