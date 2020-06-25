# -*- coding: utf-8 -*-
from __future__ import print_function
''' Verification of removeSPConstraint method.'''

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
nDivIJ= 5
nDivJK= 10
y0= 0
z0= 0
L= 1.5 # Bar length (m)
Iy= width*depth**3/12 # Cross section moment of inertia (m4)

# Load
F= 1.5e3 # Load magnitude en N

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messages
nod1= nodes.newNodeXYZ(0,0.0,0.0)
nod2= nodes.newNodeXYZ(L,0.0,0.0)


lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials definition
fy= 275e6 # Yield stress of the steel.
E= 210e9 # Young modulus of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)

respT= typical_materials.defElasticMaterial(preprocessor, "respT",1e10) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e9) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e9) # Shear response in z direction.

# Sections
import os
pth= os.path.dirname(__file__)
#print("pth= ", pth)
if(not pth):
  pth= "."
execfile(pth+"/../aux/testQuadRegion.py")

materialHandler= preprocessor.getMaterialHandler
quadFibers= materialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("testQuadRegion")
quadFibers.setupFibers()
A= quadFibers.getFibers().getArea

agg= materialHandler.newMaterial("section_aggregator","agg")
agg.setSection("quadFibers")
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])
# Shear and torsion responses.

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= "lin"
elements.defaultMaterial= "agg"
elements.numSections= 3 # Number of sections along the element.
elem1= elements.newElement("ForceBeamColumn3d",xc.ID([nod1.tag,nod2.tag]))



# Constraints
modelSpace.fixNode000_000(nod1.tag)
spc= modelSpace.constraints.newSPConstraint(nod2.tag,1,0.0)
spcTag= spc.tag

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(nod2.tag,xc.Vector([0,-F,0,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)
# Solution procedure
analysis= predefined_solutions.simple_static_modified_newton(feProblem)
result= analysis.analyze(10)


nodes= preprocessor.getNodeHandler 
delta0= nod2.getDisp[1]  # Node 2 yAxis displacement



modelSpace.constraints.removeSPConstraint(spcTag)

# Solution procedure
result= modelSpace.analyze(calculateNodalReactions= True)
 
delta= nod2.getDisp[1]  # Node 2 xAxis displacement
Ry= nod1.getReaction[1] 
RMz= nod1.getReaction[5] 

elem1.getResistingForce()
scc0= elem1.getSections()[0]

V= scc0.getStressResultantComponent("Vz")
N0= scc0.getStressResultantComponent("N")
M= scc0.getStressResultantComponent("My")

deltateor= (-F*L**3/(3*E*Iy))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs(N0))
MTeor= (F*L)
ratio3= (abs((M-MTeor)/MTeor))
ratio4= (abs((V+F)/F))
ratio5= (abs((Ry-F)/F))
ratio6= (abs((RMz-MTeor)/MTeor))

''' 
print("delta0= ",delta0)
print("delta: ",delta)
print("deltaTeor: ",deltateor)
print("ratio1= ",ratio1)
print("N0= ",N0)
print("ratio2= ",ratio2)
print("M= ",M)
print("MTeor= ",MTeor)
print("ratio3= ",ratio3)
print("V= ",V)
print("ratio4= ",ratio4)
print("Ry= ",Ry)
print("ratio5= ",ratio5)
print("RMz= ",RMz)
print("ratio6= ",ratio6)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(delta0)<1e-11) & (abs(ratio1)<0.02) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4)<1e-10) & (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
