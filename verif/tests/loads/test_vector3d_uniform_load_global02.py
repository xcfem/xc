# -*- coding: utf-8 -*-
# home made test
# Horizontal cantilever under tension load at its end.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Geometry
width= .05
depth= .1
nDivIJ= 5
nDivJK= 10
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
f= 1.5e3 # Load magnitude en N/m.

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L*math.sqrt(2)/2,L*math.sqrt(2)/2,0.0)


# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newCorotCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

# Materials definition
fy= 275e6 # Yield stress of the steel.
acero= typical_materials.defSteel01(preprocessor, "acero",E,fy,0.001)

respT= typical_materials.defElasticMaterial(preprocessor, "respT",G*J) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e9) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e9) # Shear response in z direction.
# Secciones
import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/../aux/testQuadRegion.py")
materiales= preprocessor.getMaterialLoader
quadFibers= materiales.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("testQuadRegion")
quadFibers.setupFibers()
A= quadFibers.getFibers().getSumaAreas(1.0)

agg= materiales.newMaterial("section_aggregator","agg")
agg.setSection("quadFibers")
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])
 # Torsion and shear responses.



# Elements definition
elements= preprocessor.getElementLoader
elements.defaultTransformation= "lin"
elements.defaultMaterial= "agg"
elements.numSections= 2 # Number of sections along the element.
elements.defaultTag= 1
el= elements.newElement("force_beam_column_3d",xc.ID([1,2]))



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
casos.currentLoadPattern= "0"
mesh= prueba.getDomain.getMesh
eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
  elem.vector3dUniformLoadGlobal(xc.Vector([f*math.sqrt(2)/2,f*math.sqrt(2)/2,0]))
  elem= eIter.next()

cargas= preprocessor.getLoadLoader

#We add the load case to domain.
casos.addToDomain("0")
# Solution procedure
analisis= predefined_solutions.simple_static_modified_newton(prueba)
prueba.setVerbosityLevel(0) #Dont print warning messages
                            #about loads over
                            #elements with corotational formulation.
result= analisis.analyze(10)

execfile(pth+"/test_vector3d_uniform_load_global.py")
prueba.setVerbosityLevel(1) #Print warnings again 


''' 
print "delta: ",delta
print "deltaTeor: ",deltateor
print "ratio1= ",ratio1
print "N0= ",N0
print "ratio2= ",ratio2
print "RN= ",RN
print "ratio3= ",ratio3
   '''

from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) & (abs(ratio2)<1e-10) &  (abs(ratio3)<1e-10):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
