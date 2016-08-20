# -*- coding: utf-8 -*-
# home made test
# Ménsula horizontal sometida a carga de tracción en su extremo.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model import fix_node_3dof

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

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodos= preprocessor.getNodeLoader
# Problem type
predefined_spaces.gdls_resist_materiales2D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(0,0.0)
nod= nodos.newNodeXY(L,0.0)


# Definimos transformaciones geométricas
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")


# Materials definition
seccion= typical_materials.defElasticShearSection2d(preprocessor, "seccion",A,E,G,I,1.0)

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin" # Transformación de coordenadas para los nuevos elementos
elementos.defaultMaterial= "seccion"
beam2d= elementos.newElement("force_beam_column_2d",xc.ID([1,2]));

# Constraints
coacciones= preprocessor.getConstraintLoader
fix_node_3dof.fixNode000(coacciones,1)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,-F,0]))
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
RMz= nod1.getReaction[2] 

elementos= preprocessor.getElementLoader

elem1= elementos.getElement(0)
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
fname= os.path.basename(__file__)
if (abs(ratio1)<0.005) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4)<1e-10) & (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
