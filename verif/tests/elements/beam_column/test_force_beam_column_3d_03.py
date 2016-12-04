# -*- coding: utf-8 -*-
# home made test
# Ménsula horizontal sometida a carga de tracción en su extremo.

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
from model import fix_node_6dof

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

# Load
F= 1.5e3 # Load magnitude en N

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader
# Problem type
predefined_spaces.gdls_resist_materiales3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

# Materials definition
fy= 275e6 # Tensión de cedencia del acero.
E= 210e9 # Módulo de Young del acero.
acero= typical_materials.defSteel01(preprocessor, "acero",E,fy,0.001)

respT= typical_materials.defElasticMaterial(preprocessor, "respT",1e10) # Respuesta de la sección a torsión.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e9) # Respuesta de la sección a cortante según y.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e9) # Respuesta de la sección a cortante según z.
# Secciones
import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/geomCuadFibrasTN.py")

materiales= preprocessor.getMaterialLoader
cuadFibrasTN= materiales.newMaterial("fiber_section_3d","cuadFibrasTN")
fiberSectionRepr= cuadFibrasTN.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomCuadFibrasTN")
cuadFibrasTN.setupFibers()
A= cuadFibrasTN.getFibers().getSumaAreas

agg= materiales.newMaterial("section_aggregator","cuadFibras")
agg.setSection("cuadFibrasTN")
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])
 # Respuestas a torsión y cortantes.



# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "cuadFibras"
elementos.numSections= 3 # Número de secciones a lo largo del elemento.
elementos.defaultTag= 1
el= elementos.newElement("force_beam_column_3d",xc.ID([1,2]))



# Constraints
coacciones= preprocessor.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,0,-F,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")
# Procedimiento de solución
analisis= predefined_solutions.simple_static_modified_newton(prueba)
result= analisis.analyze(10)


nodes.calculateNodalReactions(True) 
nod2= nodes.getNode(2)
delta= nod2.getDisp[2]  # Node 2 displacement según z
nod1= nodes.getNode(1)
Rz= nod1.getReaction[2] 
nod1= nodes.getNode(1)
RMy= nod1.getReaction[4] 

elementos= preprocessor.getElementLoader

elem1= elementos.getElement(1)
elem1.getResistingForce()
scc= elem1.getSections()[0]

V= scc.getStressResultantComponent("Vy")
N0= scc.getStressResultantComponent("N")
M= scc.getStressResultantComponent("Mz")


deltateor= (-F*L**3/(3*E*Iz))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs(N0))
MTeor= (F*L)
ratio3= (abs((M-MTeor)/MTeor))
ratio4= (abs((V+F)/F))
ratio5= (abs((Rz-F)/F))
ratio6= (abs((RMy+MTeor)/MTeor))

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
print "Rz= ",Rz
print "ratio5= ",ratio5
print "RMy= ",RMy
print "ratio6= ",ratio6
   '''
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<0.02) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4)<1e-10) &  (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
