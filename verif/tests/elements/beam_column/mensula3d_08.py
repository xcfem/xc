# -*- coding: utf-8 -*-
# home made test
# Ménsula sometida a carga uniforme vertical según el eje y local.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

# Propiedades del material
E= 2.1e6*9.81/1e-4 # Módulo elástico en Pa
nu= 0.3 # Coeficiente de Poisson
G= E/(2*(1+nu)) # Módulo de elasticidad a cortante

# Propiedades de la sección (IPE-80)
A= 7.64e-4 # Área de la sección expresada en m2
Iy= 80.1e-8 # Momento de inercia de la sección expresada en m4
Iz= 8.49e-8 # Momento de inercia de la sección expresada en m4
J= 0.721e-8 # Momento de inercia a torsión expresado en m4

# Geometry
L= 1.5 # Bar length (m)

# Load
f= 1.5e3 # Magnitud de la carga en kN/m

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador   
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXYZ(0,0.0,0.0)
nod= nodos.newNodeXYZ(L,0.0,0.0)


# Definimos transformaciones geométricas
trfs= mdlr.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,-1,0])

# Materials
caracMecSeccion= xc.ConstantesSecc3d()
caracMecSeccion.A= A; caracMecSeccion.E= E; caracMecSeccion.G= G;
caracMecSeccion.Iz= Iz; caracMecSeccion.Iy= Iy; caracMecSeccion.J= J
seccion= typical_materials.defElasticSectionFromMechProp3d(mdlr,"seccion",caracMecSeccion)

# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "seccion"
elementos.defaultTag= 1 #Tag for the next element.
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,2]));



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
#casos.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID([1]) 
eleLoad.transComponent= -f
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodos= mdlr.getNodeLoader 
nod2= nodos.getNode(2)
delta= nod2.getDisp[2]  # Desplazamiento del nodo 2 según z global

elementos= mdlr.getElementLoader

elem1= elementos.getElement(1)
elem1.getResistingForce()
Mz1= elem1.getMz1 # Momento en el extremo dorsal de la barra
Mz2= elem1.getMz2 # Momento en el extremo frontal de la barra
Vy1= elem1.getVy1 # Cortante en el extremo dorsal de la barra
Vy2= elem1.getVy2 # Cortante en el extremo frontal de la barra



deltateor= (-f*L**4/(8*E*Iz))
ratio1= (delta/deltateor)
Mz1teor= (-f*L*L/2)
ratio2= (Mz1/Mz1teor)
ratio3= (abs(Mz2)<1e-3)
Vy1teor= (-f*L)
ratio4= (Vy1/Vy1teor)
ratio5= (abs(Vy2)<1e-3)

'''
print "delta= ",delta
print "deltateor= ",deltateor
print "ratio1= ",ratio1
print "Mz1= ",Mz1
print "Mz1teor= ",Mz1teor
print "ratio2= ",ratio2
print "Mz2= ",Mz2
print "Mz2teor= ",0
print "ratio3= ",ratio3
print "Vy1= ",Vy1
print "Vy1teor= ",Vy1teor
print "ratio4= ",ratio4
print "Vy2= ",Vy2
print "Vy2teor= ",0
print "ratio5= ",ratio5
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5) & (abs(ratio4-1.0)<1e-5) & (abs(ratio5-1.0)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
