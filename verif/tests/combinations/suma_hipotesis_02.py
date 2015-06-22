# -*- coding: utf-8 -*-
# home made test
# Suma de acciones sobre ménsula.

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
Gf1= 1.33 # Coeficiente de ponderación de la hipótesis.
Gf2= 1.5 # Coeficiente de ponderación de la hipótesis.

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador   
nodos= mdlr.getNodeLoader

# Problem type
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nodos.newNodeXYZ(0,0.0,0.0)
nodos.newNodeXYZ(L,0.0,0.0)

# Definimos transformaciones geométricas
trfs= mdlr.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,-1,0])
    
# Materials definition
scc= typical_materials.defElasticSection3d(mdlr,"scc",A,E,G,Iz,Iy,J)


# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "scc"
#  sintaxis: elastic_beam_3d[<tag>] 
elementos.defaultTag= 1 #Tag for next element.
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
lp0= casos.newLoadPattern("default","0")
lp0.gammaF= Gf1
lp1= casos.newLoadPattern("default","1")
lp1.gammaF= Gf2
#casos.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID([1])
eleLoad.axialComponent= f
eleLoad= lp1.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID([1])
eleLoad.transComponent= -f
#We add the load case to domain.
casos.addToDomain("0")
#We add the load case to domain.
casos.addToDomain("1")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

nod2= nodos.getNode(2)
deltax= nod2.getDisp[0] 
deltay= nod2.getDisp[2] 

elem1= elementos.getElement(1)
elem1.getResistingForce()
N1= elem1.getN1 # Axil en el extremo dorsal de la barra
N2= elem1.getN2 # Axil en el extremo frontal de la barra
Mz1= elem1.getMz1 # Momento en el extremo dorsal de la barra
Mz2= elem1.getMz2 # Momento en el extremo frontal de la barra
Vy1= elem1.getVy1 # Cortante en el extremo dorsal de la barra
Vy2= elem1.getVy2 # Cortante en el extremo frontal de la barra

deltaxteor= (Gf1*f*L**2/(2*E*A))
ratio1= (deltax/deltaxteor)
N1teor= (Gf1*f*L)
ratio2= (N1/N1teor)
ratio3= (abs(N2)<1e-3)
deltayteor= (-Gf2*f*L**4/(8*E*Iz))
ratio4= (deltay/deltayteor)
Mz1teor= (-Gf2*f*L*L/2)
ratio5= (Mz1/Mz1teor)
ratio6= (abs(Mz2)<1e-3)
Vy1teor= (-Gf2*f*L)
ratio7= (Vy1/Vy1teor)
ratio8= (abs(Vy2)<1e-3)

# print "deltax= ",deltax
# print "deltaxteor= ",deltaxteor
# print "ratio1= ",ratio1
# print "N1= ",N1
# print "N1teor= ",N1teor
# print "ratio2= ",ratio2
# print "N2= ",N2
# print "N2teor= ",0
# print "ratio3= ",ratio3
# print "deltax= ",deltax
# print "deltaxteor= ",deltaxteor
# print "ratio4= ",ratio4
# print "Mz1= ",Mz1
# print "Mz1teor= ",Mz1teor
# print "ratio5= ",ratio5
# print "Mz2= ",Mz2
# print "Mz2teor= ",0
# print "ratio6= ",ratio6
# print "Vy1= ",Vy1
# print "Vy1teor= ",Vy1teor
# print "ratio7= ",ratio7
# print "Vy2= ",Vy2
# print "Vy2teor= ",0
# print "ratio8= ",ratio8

import os
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5) & (abs(ratio4-1.0)<1e-5) & (abs(ratio5-1.0)<1e-5) & (abs(ratio6-1.0)<1e-5) & (abs(ratio7-1.0)<1e-5) & (abs(ratio8-1.0)<1e-5) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

