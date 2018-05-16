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

# Problem type
prueba= xc.ProblemaEF()
prep= prueba.getPreprocessor
nodos= prep.getNodeHandler
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.newNodeIDXYZ(1,0,0,0)
nodos.newNodeIDXYZ(2,L,0,0)
nodos.newNodeIDXYZ(3,L,L,0)
nodos.newNodeIDXYZ(4,0,L,0)


# Definimos materiales

        memb1= typical_materials.defElasticMembranePlateSection(prep,"memb1",E,nu,dens,h)


elementos= prep.getElementHandler
elementos.defaultMaterial= "memb1"
elem= elementos.newElement("shell_mitc4",xc.ID([1,2,3,4]))


# Condiciones de contorno
coacciones= prep.getBoundaryCondHandler

fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.fixNode6DOF(coacciones,2)


# Definimos cargas
cargas= prep.getLoadHandler

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Define un caso de carga
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"


elementos= prep.getElementHandler
        \for_each

            .vector3dEdgeLoadGlobal(xc.ID(3,4),xc.Vector(0,p,0))


cargas= prep.getLoadHandler

casos= cargas.getLoadPatterns{#We add the load case to domain.
casos.addToDomain("0")


#Solución
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


        nodos.calculateNodalReactions(True)
nodos= prep.getNodeHandler

 RN1= nodos.getNode(1).getReaction[1] 
 RN2= nodos.getNode(2).getReaction[1] 
 RN3= nodos.getNode(3).getReaction[1] 
 RN4= nodos.getNode(4).getReaction[1] 




RTeor= -p*L/2

ratio1= (abs((RN1-RTeor)/RTeor))
ratio2= (abs((RN2-RTeor)/RTeor))
ratio3= (abs((RN3)))
ratio4= (abs((RN4)))



''' 
print "RN1= ",RN1
print "ratio1= ",ratio1
print "RN2= ",RN2
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
 '''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

