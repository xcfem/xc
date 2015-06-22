# -*- coding: utf-8 -*-
# home made test
# Ménsula horizontal sometida a carga de tracción en su extremo.
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials

# Geometry
ancho= .001
canto= .01
A= ancho*canto
E= 210e9
I= ancho*canto**3/12.0
nu= 0.3
G= E/(2.0*(1+nu))
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Magnitud de la carga en N

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador   
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(0,0.0)
nod= nodos.newNodeXY(L,0.0)


# Definimos transformaciones geométricas
trfs= mdlr.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")


# Materials definition
seccion= typical_materials.defElasticShearSection2d(mdlr,"seccion",A,E,G,I,1.0)


# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultTransformation= "lin" # Transformación de coordenadas para los nuevos elementos
elementos.defaultMaterial= "seccion"
beam2d= elementos.newElement("force_beam_column_2d",xc.ID([1,2]));

# Constraints
coacciones= mdlr.getConstraintLoader
fix_node_3dof.fixNode000(coacciones,1)

# Loads definition
cargas= mdlr.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,0]))
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
print "F= ",(F)
print "delta: ",delta
print "deltaTeor: ",deltateor
print "ratio1= ",ratio1
print "N0= ",N0
print "ratio2= ",ratio2
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10 and abs(ratio2)<1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
