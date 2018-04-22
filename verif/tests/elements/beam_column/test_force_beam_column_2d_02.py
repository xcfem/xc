# -*- coding: utf-8 -*-
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
width= .05
depth= .1
A= width*depth
E= 210e9
I= width*depth**3/12
nu= 0.3
G= E/(2*(1+nu))
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude en N

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
seccion= typical_materials.defElasticShearSection2d(preprocessor, "seccion",A,E,G,I,1.0)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= "lin" # Coordinate transformation for the new elements
elements.defaultMaterial= "seccion"
beam2d= elements.newElement("ForceBeamColumn2d",xc.ID([1,2]));

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000(1)

# Loads definition
cargas= preprocessor.getLoadHandler
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,-F,0]))
#We add the load case to domain.
casos.addToDomain("0")
# Solution procedure
analisis= predefined_solutions.simple_static_modified_newton(feProblem)
result= analisis.analyze(10)


nodes.calculateNodalReactions(True,1e-7) 
nod2= nodes.getNode(2)
delta= nod2.getDisp[1]  # Node 2 xAxis displacement
nod1= nodes.getNode(1)
Ry= nod1.getReaction[1] 
RMz= nod1.getReaction[2] 

elements= preprocessor.getElementHandler

elem1= elements.getElement(0)
elem1.getResistingForce()
scc= elem1.getSections()[0]

V= scc.getStressResultantComponent("Vy")
N0= scc.getStressResultantComponent("N")
M= scc.getStressResultantComponent("Mz")

deltateor= (-F*L**3/(3*E*I))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs(N0))
MTeor= (-F*L)
ratio3= (abs((M-MTeor)/MTeor))
ratio4= (abs((V-F)/F))
ratio5= (abs((Ry-F)/F))
ratio6= (abs((RMz+MTeor)/MTeor))

''' 
print "delta: ",delta
print "deltaTeor: ",deltateor
print "ratio1= ",ratio1
print "N0= ",N0
print "ratio2= ",ratio2
print "M= ",M
print "MTeor= ",MTeor
print "ratio3= ",ratio3
print "V= ",V
print "ratio4= ",ratio4
print "Ry= ",Ry
print "ratio5= ",ratio5
print "RMz= ",RMz
print "ratio6= ",ratio6
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.005) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4)<1e-10) & (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
