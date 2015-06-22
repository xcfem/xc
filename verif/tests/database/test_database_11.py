# -*- coding: utf-8 -*-
''' home made test
   Prueba del funcionamiento de la base de datos
   con elementos Shell. '''

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
from model import fix_node_6dof
import math

E= 2.1e6 # Módulo de Young del acero en kg/cm2.
nu= 0.3 # Coeficiente de Poisson.
h= 0.1 # Espesor.
dens= 1.33 # Densidad kg/m2.

v1= xc.Vector([0,math.sqrt(2)/2,math.sqrt(2)/2])
v2= xc.Vector([0,-math.sqrt(2)/2,math.sqrt(2)/2])

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.newNodeIDXYZ(1,0,0,0)
nodos.newNodeIDXYZ(2,2,0,0)
nodos.newNodeIDXYZ(3,2,1,1)
nodos.newNodeIDXYZ(4,0,1,1)


# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(mdlr,"memb1",E,nu,dens,h)

elementos= mdlr.getElementLoader
elementos.defaultMaterial= "memb1"
elem= elementos.newElement("shell_mitc4",xc.ID([1,2,3,4]))


import os
os.system("rm -r -f /tmp/test11.db")
db= prueba.newDatabase("BerkeleyDB","/tmp/test11.db")
db.save(100)
prueba.clearAll()
db.restore(100)

elem= elementos.getElement(0)
ratio1= (elem.getCoordTransf.getG2Vector-v1).Norm()
ratio2= (elem.getCoordTransf.getG3Vector-v2).Norm()
#ratio1= abs((abs(getCoordTransf.getG2Vector-v1)))
#ratio2= abs((abs(getCoordTransf.getG3Vector-v2)))




''' 
print "v1= ",v1
print "v2= ",v2
print "ratio1= ",ratio1
print "ratio2= ",ratio2
 '''

fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

os.system("rm -rf /tmp/test11.db") # Your garbage you clean it
