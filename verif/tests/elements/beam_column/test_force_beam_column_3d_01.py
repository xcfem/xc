# -*- coding: utf-8 -*-
# home made test
# Ménsula horizontal sometida a carga de tracción en su extremo.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model import fix_node_6dof

# Geometry
ancho= .001
canto= .01
ndivIJ= 2
ndivJK= 2
y0= 0
z0= 0
L= 1.5 # Bar length (m)

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
fy= 275e6 # Tensión de cedencia del acero
E= 210e9 # Módulo de Young del acero.
acero= typical_materials.defSteel01(mdlr,"acero",E,fy,0.001)

import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/geomCuadFibrasTN.py")
cuadFibrasTN= mdlr.getMaterialLoader.newMaterial("fiber_section_3d","cuadFibrasTN")
fiberSectionRepr= cuadFibrasTN.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomCuadFibrasTN")
cuadFibrasTN.setupFibers()
fibras= cuadFibrasTN.getFibers()
A= fibras.getSumaAreas(1.0)



# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "cuadFibrasTN"
beam3d= elementos.newElement("force_beam_column_3d",xc.ID([1,2]));

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
lp0.newNodalLoad(2,xc.Vector([F,0,0,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")
# Procedimiento de solución
analisis= predefined_solutions.simple_static_modified_newton(prueba)
result= analisis.analyze(10)

nodos= mdlr.getNodeLoader 
nod2= nodos.getNode(2)
delta= nod2.getDisp[0]  # Desplazamiento del nodo 2 según x

elementos= mdlr.getElementLoader

elem1= elementos.getElement(0)
elem1.getResistingForce()
scc= elem1.getSections()[0]

N0= scc.getStressResultantComponent("N")

deltateor= (F*L/(E*A))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs((N0-F)/F))

''' 
print "delta: ",delta
print "deltaTeor: ",deltateor
print "ratio1= ",ratio1
print N0
print "ratio2= ",ratio2
   '''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

