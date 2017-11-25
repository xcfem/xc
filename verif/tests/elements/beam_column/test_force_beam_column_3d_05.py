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
nDivIJ= 10
nDivJK= 5
y0= 0
z0= 0
L= 1.5 # Bar length (m)
Iy= width*depth**3/12 # Cross section moment of inertia (m4)
Iz= depth*width**3/12 # Cross section moment of inertia (m4)
E= 210e9 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
J= .2e-1 # Cross section torsion constant (m4)

# Load
M= 1.5e3 # Load magnitude en N

prb= xc.ProblemaEF()
preprocessor=  prb.getPreprocessor   
nodes= preprocessor.getNodeLoader
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials definition
fy= 275e6 # Yield stress of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)

respT= typical_materials.defElasticMaterial(preprocessor, "respT",G*J) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e9) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e9) # Shear response in z direction.
# Secciones
# Secciones
import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/../../aux/testQuadRegion.py")
materiales= preprocessor.getMaterialLoader
quadFibers= materiales.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("testQuadRegion")
quadFibers.setupFibers()
A= quadFibers.getFibers().getSumaAreas

 # Torsion and shear responses.
agg= materiales.newMaterial("section_aggregator","agg")
agg.setSection("quadFibers")
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])



# Elements definition
elements= preprocessor.getElementLoader
elements.defaultTransformation= "lin"
elements.defaultMaterial= "agg"
elements.numSections= 2 # Number of sections along the element.
elements.defaultTag= 1
el= elements.newElement("ForceBeamColumn3d",xc.ID([1,2]))



# Constraints
modelSpace.fixNode000_000(1)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,M,0]))
#We add the load case to domain.
casos.addToDomain("0")
# Solution procedure
analisis= predefined_solutions.simple_static_modified_newton(prb)
result= analisis.analyze(10)


nodes.calculateNodalReactions(True) 
nod2= nodes.getNode(2)
delta= nod2.getDisp[2]  # z displacement of node 2
theta= nod2.getDisp[4]  # y rotation of the node
nod1= nodes.getNode(1)
RM= nod1.getReaction[4] 

elements= preprocessor.getElementLoader

elem1= elements.getElement(1)
elem1.getResistingForce()
scc= elem1.getSections()[0]

V= scc.getStressResultantComponent("Vz")
M1= scc.getStressResultantComponent("Mz")

deltateor= (-M*L**2/(2*E*Iz))
thetateor= (M*L/(E*Iz))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs((M+RM)/M))
ratio3= (abs((M-M1)/M))
ratio4= (abs((theta-thetateor)/thetateor))

''' 
print "delta: ",delta
print "deltaTeor: ",deltateor
print "theta: ",theta
print "thetaTeor: ",thetateor
print "ratio1= ",ratio1
print "M= ",M
print "RM= ",RM
print "ratio2= ",ratio2
print "M1= ",M1
print "ratio3= ",ratio3
print "ratio4= ",ratio4
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.02) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4)<0.02):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
