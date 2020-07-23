# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Horizontal cantilever under tension load at its end.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Geometry
width= .05
depth= .1
A= width*depth
E= 210e9
I= width*depth**3/12
nu= 0.3
G= E/(2*(1+nu))
L= 1.5 # Bar length (m)

# Load
M= 1.5e3 # Load magnitude en N m

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
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
elements.defaultMaterial= section.name
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
lp0.newNodalLoad(2,xc.Vector([0,0,M]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)
# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
result= analysis.analyze(10)


nodes.calculateNodalReactions(True,1e-7) 
nod2= nodes.getNode(2)
delta= nod2.getDisp[1]  # z displacement of node 2
theta= nod2.getDisp[2]  # z rotation of the node
nod1= nodes.getNode(1)
RM= nod1.getReaction[2] 

elements= preprocessor.getElementHandler

elem1= elements.getElement(0)
elem1.getResistingForce()
scc= elem1.getSections()[0]

V= scc.getStressResultantComponent("Vy")
M1= scc.getStressResultantComponent("Mz")

deltateor= (M*L**2/(2*E*I))
thetateor= (M*L/(E*I))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs((M+RM)/M))
ratio3= (abs((M-M1)/M))
ratio4= (abs((theta-thetateor)/thetateor))

''' 
print("delta: ",delta)
print("deltaTeor: ",deltateor)
print("theta: ",theta)
print("thetaTeor: ",thetateor)
print("ratio1= ",ratio1)
print("M= ",M)
print("RM= ",RM)
print("ratio2= ",ratio2)
print("M1= ",M1)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
   '''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.005) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4)<0.02):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
  
