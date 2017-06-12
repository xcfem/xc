# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Se comprueba el funcionamiento de remove_fix.

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
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
prueba.logFileName= "/tmp/borrar.log" # Ignore warning messages
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

# Materials definition
fy= 275e6 # Yield stress of the steel.
E= 210e9 # Young modulus of the steel.
acero= typical_materials.defSteel01(preprocessor, "acero",E,fy,0.001)

respT= typical_materials.defElasticMaterial(preprocessor, "respT",1e10) # Respuesta de la sección a torsión.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e9) # Respuesta de la sección a cortante según y.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e9) # Respuesta de la sección a cortante según z.
# Secciones
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
A= quadFibers.getFibers().getSumaAreas

agg= materiales.newMaterial("section_aggregator","agg")
agg.setSection("quadFibers")
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])
# Shear and torsion responses.

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "agg"
elementos.numSections= 3 # Número de secciones a lo largo del elemento.
elementos.defaultTag= 1
el= elementos.newElement("force_beam_column_3d",xc.ID([1,2]))



# Constraints
modelSpace.fixNode000_000(1)
spc= modelSpace.constraints.newSPConstraint(2,1,0.0)
spcTag= spc.tag

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,-F,0,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")
# Procedimiento de solución
analisis= predefined_solutions.simple_static_modified_newton(prueba)
result= analisis.analyze(10)


nodes= preprocessor.getNodeLoader 
nod2= nodes.getNode(2)
delta0= nod2.getDisp[1]  # Node 2 yAxis displacement



modelSpace.constraints.removeSPConstraint(spcTag)

# Procedimiento de solución
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodes.calculateNodalReactions(True) 
nod2= nodes.getNode(2)
delta= nod2.getDisp[1]  # Node 2 xAxis displacement
nod1= nodes.getNode(1)
Ry= nod1.getReaction[1] 
RMz= nod1.getReaction[5] 

elem1= elementos.getElement(1)
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
print "delta0= ",delta0
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
if (abs(delta0)<1e-11) & (abs(ratio1)<0.02) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4)<1e-10) & (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
