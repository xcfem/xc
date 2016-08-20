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
M= 1.5e3 # Load magnitude en N m

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
lp0.newNodalLoad(2,xc.Vector([0,0,M]))
#We add the load case to domain.
casos.addToDomain("0")
# Procedimiento de solución
analisis= predefined_solutions.simple_static_modified_newton(prueba)
result= analisis.analyze(10)


nodos.calculateNodalReactions(True) 
nod2= nodos.getNode(2)
delta= nod2.getDisp[1]  # Desplazamiento del nodo 2 según z
theta= nod2.getDisp[2]  # Giro del nodo según z
nod1= nodos.getNode(1)
RM= nod1.getReaction[2] 

elementos= preprocessor.getElementLoader

elem1= elementos.getElement(0)
elem1.getResistingForce()
scc= elem1.getSections()[0]

V= scc.getStressResultantComponent("Vy")
M1= scc.getStressResultantComponent("Mz")

deltateor= (M*L**2/(2*E*I))
thetateor= (M*L/(E*I))
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
fname= os.path.basename(__file__)
if (abs(ratio1)<0.005) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(ratio4)<0.02):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
  
