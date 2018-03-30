# -*- coding: utf-8 -*-
# Test de elaboración propia, comprueba el funcionamiento del
#    comando vector3d_edge_load_global.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

E= 2.1e6 # Steel Young modulus.
nu= 0.3 # Poisson's coefficient.
h= 0.1 # Espesor.
dens= 1.33 # Densidad kg/m2.
L= 2
p= 10

# Tipo de problema
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeHandler
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1; nodos.newNodeXY(0,0,0})
nodos.defaultTag= 2; nodos.newNodeXY(L,0,0})
nodos.defaultTag= 3; nodos.newNodeXY(L,L,0})
nodos.defaultTag= 4; nodos.newNodeXY(0,L,0})


# Definimos materiales

        memb1= typical_materials.defElasticMembranePlateSection(mdlr,"memb1",E,nu,dens,h)


elementos= mdlr.getElementHandler
elementos.defaultMaterial= "memb1"
elem= elementos.newElement("shell_mitc4",xc.ID([1,2,3,4]))


# Condiciones de contorno
coacciones= mdlr.getBoundaryCondHandler

fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.fixNode6DOF(coacciones,2)


# Definimos cargas
cargas= mdlr.getLoadHandler

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Define un caso de carga
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"


elementos= mdlr.getElementHandler
        \for_each

            \vector3d_edge_load_global[3,4]{[0,0,p]


cargas= mdlr.getLoadHandler

casos= cargas.getLoadPatterns{#We add the load case to domain.
casos.addToDomain("0")


#Solución
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


        nodos.calculateNodalReactions(True)
nodos= mdlr.getNodeHandler

 RNX1= .getReaction[0] RNY1= nodos.getNode(1).getReaction[1] RNZ1= .getReaction[2] \expr{MX1= .getReaction[3] 
 RNX2= .getReaction[0] RNY2= nodos.getNode(2).getReaction[1] RNZ2= .getReaction[2] \expr{MX2= .getReaction[3] 




RTeor= -p*L/2
MTeor= RTeor*L

ratio1= (abs((RNX1+RNX2)/RTeor))
ratio2= (abs((RNY1+RNY2)/RTeor))
ratio3= (abs((RNZ1-RTeor)/RTeor))
ratio4= (abs((RNZ2-RTeor)/RTeor))
ratio5= (abs((MX1-MTeor)/MTeor))
ratio6= (abs((MX2-MTeor)/MTeor))



''' 
print "RNX1= ",RNX1
print "RNY1= ",RNY1
print "RNZ1= ",RNZ1
print "MX1= ",MX1
print "ratio1= ",ratio1
print "RNX2= ",RNX2
print "RNY2= ",RNY2
print "RNZ2= ",RNZ2
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5
print "ratio6= ",ratio6
 '''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12) & (ratio5 < 1e-12) & (ratio6 < 1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

