# -*- coding: utf-8 -*-
# home made test
# Ménsula horizontal sometida a carga vertical en su extremo.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model import fix_node_6dof

# Geometry
ancho= .05
canto= .1
ndivIJ= 5
ndivJK= 10
y0= 0
z0= 0
L= 1.5 # Bar length (m)
Iy= ancho*canto**3/12 # Momento de inercia de la sección expresada en m4

# Load
F= 1.5e3 # Magnitud de la carga en N

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador   
nodos= mdlr.getNodeLoader
# Problem type
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXYZ(0,0.0,0.0)
nod= nodos.newNodeXYZ(L,0.0,0.0)


trfs= mdlr.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

# Materials definition
fy= 275e6 # Tensión de cedencia del acero.
E= 210e9 # Módulo de Young del acero.
acero= typical_materials.defSteel01(mdlr,"acero",E,fy,0.001)

respT= typical_materials.defElasticMaterial(mdlr,"respT",1e10) # Respuesta de la sección a torsión.
respVy= typical_materials.defElasticMaterial(mdlr,"respVy",1e9) # Respuesta de la sección a cortante según y.
respVz= typical_materials.defElasticMaterial(mdlr,"respVz",1e9) # Respuesta de la sección a cortante según z.
# Secciones
import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/geomCuadFibrasTN.py")

materiales= mdlr.getMaterialLoader
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
elementos= mdlr.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "cuadFibras"
elementos.numSections= 3 # Número de secciones a lo largo del elemento.
elementos.defaultTag= 1
el= elementos.newElement("force_beam_column_3d",xc.ID([1,2]))



# Constraints
coacciones= mdlr.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)

# Loads definition
cargas= mdlr.getLoadLoader
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


nodos.calculateNodalReactions(True) 
nod2= nodos.getNode(2)
delta= nod2.getDisp[1]  # Desplazamiento del nodo 2 según x
nod1= nodos.getNode(1)
Ry= nod1.getReaction[1] 
nod1= nodos.getNode(1)
RMz= nod1.getReaction[5] 

elementos= mdlr.getElementLoader

elem1= elementos.getElement(1)
elem1.getResistingForce()
scc= elem1.getSections()[0]

V= scc.getStressResultantComponent("Vz")
N0= scc.getStressResultantComponent("N")
M= scc.getStressResultantComponent("My")

deltateor= (-F*L**3/(3*E*Iy))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs(N0))
MTeor= (F*L)
ratio3= (abs((M-MTeor)/MTeor))
ratio4= (abs((V+F)/F))
ratio5= (abs((Ry-F)/F))
ratio6= (abs((RMz-MTeor)/MTeor))

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
fname= os.path.basename(__file__)
if (abs(ratio1)<0.02) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4)<1e-10) & (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
