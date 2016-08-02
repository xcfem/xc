# -*- coding: utf-8 -*-
# home made test
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

E= 2.1e6 # Módulo de Young del acero.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Espesor.
dens= 1.33 # Densidad kg/m2.

q= -2

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.newNodeIDXYZ(1,0,0,0)
nodos.newNodeIDXYZ(2,1,0,0)
nodos.newNodeIDXYZ(3,1,1,0)
nodos.newNodeIDXYZ(4,0,1,0)


# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)


elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "memb1"
elem= elementos.newElement("shell_mitc4",xc.ID([1,2,3,4]))


# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.Nodo6DOFGirosLibres(coacciones, 1)
fix_node_6dof.Nodo6DOFGirosLibres(coacciones, 2)
fix_node_6dof.Nodo6DOFGirosLibres(coacciones, 3)
fix_node_6dof.Nodo6DOFGirosLibres(coacciones, 4)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("shell_uniform_load")
eleLoad.elementTags= xc.ID([0]) 
eleLoad.transComponent= q
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodos.calculateNodalReactions(True)

RN1= nodos.getNode(1).getReaction[2] 
RN2= nodos.getNode(2).getReaction[2] 
RN3= nodos.getNode(3).getReaction[2] 
RN4= nodos.getNode(4).getReaction[2] 
RTeor= q/4.0

ratio1= abs((RN1+RTeor)/RTeor)
ratio2= abs((RN2+RTeor)/RTeor)
ratio3= abs((RN3+RTeor)/RTeor)
ratio4= abs((RN4+RTeor)/RTeor)


''' 
elementos= preprocessor.getElementLoader
        \for_each

            print "G3= ",vectorG3



nodos= preprocessor.getNodeLoader

             \nodo[1]{print "reac= ",reac} 
             \nodo[2]{print "reac= ",reac} 
             \nodo[3]{print "reac= ",reac} 
             \nodo[4]{print "reac= ",reac} 
'''

'''
print "RTeor= ", RTeor
print "RN1= ", RN1
print "ratio1= ",ratio1
print "RN2= ", RN2
print "ratio2= ",ratio2
print "RN3= ", RN3
print "ratio3= ",ratio3
print "RN4= ", RN4
print "ratio4= ",ratio4
'''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

