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
nDivIJ= 2
nDivJK= 2
y0= 0
z0= 0
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude en N

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials definition
fy= 275e6 # Yield stress of the steel
E= 210e9 # Young modulus of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)

import os
pth= os.path.dirname(__file__)
#print("pth= ", pth)
if(not pth):
  pth= "."
exec(open(pth+"/../../aux/testQuadRegion.py").read()) #Definition of section geometry (regions and rebars)
# Definition of a new empty fiber section named 'quadFibers' and stored in a
# Python variable of the same name (surprisingly enough).
quadFibers= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr() #Fiber section representation
                                                     # of 'quadFibers'
fiberSectionRepr.setGeomNamed("testQuadRegion") #We assign the geometry (regions and rebars)
                                                  #to the fiber section representation
                                                  #of 'quadFibers'
quadFibers.setupFibers() #Create the fibers from the information contained in th
                           #geometry.
fibras= quadFibers.getFibers() #Get the fiber container from the object.
A= fibras.getArea(1.0) #Get the sum of the fiber areas.



# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= "quadFibers" #Material name for the element (the fiber section).
beam3d= elements.newElement("ForceBeamColumn3d",xc.ID([1,2]))

# Constraints
modelSpace.fixNode000_000(1)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,0,0,0,0]))
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
print("delta: ",delta)
print("deltaTeor: ",deltateor)
print("ratio1= ",ratio1)
print(N0)
print("ratio2= ",ratio2)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

