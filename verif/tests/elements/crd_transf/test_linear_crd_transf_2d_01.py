# -*- coding: utf-8 -*-
# home made test
from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(5,5)

# Geometric transformations
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")

# Materials
seccion= typical_materials.defElasticSection2d(preprocessor, "seccion",1,1,1)
    

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin" # Transformación de coordenadas para los nuevos elementos
elementos.defaultMaterial= "seccion"
elementos.defaultTag= 1 #Tag for next element.
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([1,2]))

setTotal= preprocessor.getSets.getSet("total")
elems= setTotal.getElements
for e in elems:
  crdTransf= e.getCoordTransf
  #print "vector I:",crdTransf.getIVector
  vILocal= crdTransf.getVectorLocalCoordFromGlobal(crdTransf.getIVector)
  #print "vector I en locales:",vILocal
  #print "vector J:",crdTransf.getJVector
  vJLocal= crdTransf.getVectorLocalCoordFromGlobal(crdTransf.getJVector)
  #print "vector J en locales:",vJLocal
  dif1= vILocal-xc.Vector([1,0])
  ratio1= dif1.Norm()
  ratio2= (vJLocal-xc.Vector([0,1])).Norm()

# print "vILocal= ", vILocal
# print "vJLocal= ", vJLocal
# print "ratio1= ", ratio1
# print "ratio2= ", ratio2

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
