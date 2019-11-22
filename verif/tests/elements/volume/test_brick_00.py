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
from postprocess import output_handler

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25,0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
nod9= nodes.newNodeXYZ(0,0,0)
nod10= nodes.newNodeXYZ(1,0,0)
nod11= nodes.newNodeXYZ(1,1,0)
nod12= nodes.newNodeXYZ(0,1,0)
nod13= nodes.newNodeXYZ(0,0,1)
nod14= nodes.newNodeXYZ(1,0,1)
nod15= nodes.newNodeXYZ(1,1,1)
nod16= nodes.newNodeXYZ(0,1,1)


elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast3d"
elements.defaultTag= 1 #Tag for the next element.
brick= elements.newElement("Brick",xc.ID([nod9.tag,nod10.tag,nod11.tag,nod12.tag,nod13.tag,nod14.tag,nod15.tag,nod16.tag]))

constraints= preprocessor.getBoundaryCondHandler
#Constrain the displacement of node 1.

nod9.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod10.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod11.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod12.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#lPatterns.currentLoadPattern= "0"
lp0.newNodalLoad(nod13.tag, xc.Vector([0,0,-1]))
lp0.newNodalLoad(nod14.tag, xc.Vector([0,0,-1]))
lp0.newNodalLoad(nod15.tag, xc.Vector([0,0,-1]))
lp0.newNodalLoad(nod16.tag, xc.Vector([0,0,-1]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# # Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)

# ## Uncomment to display the mesh
# oh.displayFEMesh()
# oh.displayLoads()

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)

nodes.calculateNodalReactions(True,1e-7)
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
