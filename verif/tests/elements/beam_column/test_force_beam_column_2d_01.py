# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test
# Horizontal cantilever under tension load at its end.

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

# Geometry
width= .001
depth= .01
A= width*depth
E= 210e9
I= width*depth**3/12.0
nu= 0.3
G= E/(2.0*(1+nu))
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude en N

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0.0)
nod= nodes.newNodeXY(L,0.0)


# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

# Materials definition
section= typical_materials.defElasticShearSection2d(preprocessor, "section",A,E,G,I,1.0)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name # Coordinate transformation for the new elements
elements.defaultMaterial= "section"
beam2d= elements.newElement("ForceBeamColumn2d",xc.ID([1,2]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000(1)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,0]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution procedure
analysis= predefined_solutions.simple_static_modified_newton(feProblem)
result= analysis.analyze(10)


nodes= preprocessor.getNodeHandler 
nod2= nodes.getNode(2)
delta= nod2.getDisp[0]  # Node 2 xAxis displacement

elements= preprocessor.getElementHandler

elem1= elements.getElement(0)
elem1.getResistingForce()
scc= elem1.getSections()[0]
N0= scc.getStressResultantComponent("N")
deltateor= (F*L/(E*A))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs((N0-F)/F))

''' 
print("F= ",(F))
print("delta: ",delta)
print("deltaTeor: ",deltateor)
print("ratio1= ",ratio1)
print("N0= ",N0)
print("ratio2= ",ratio2)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10 and abs(ratio2)<1e-10):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
