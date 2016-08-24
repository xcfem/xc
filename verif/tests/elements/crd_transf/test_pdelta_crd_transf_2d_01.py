# -*- coding: utf-8 -*-
# home made test
from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
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
lin= preprocessor.getTransfCooLoader.newPDeltaCrdTransf2d("lin")

# Materials
seccion= typical_materials.defElasticSection2d(preprocessor, "seccion",1,1,1)
    
# Elements definition
elementos= preprocessor.getElementLoader

elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "seccion"
elementos.defaultTag= 1 #Tag for next element.
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([1,2]))

crdTransf= beam2d.getCoordTransf

# print "vector I:",getIVector
# print "vector J:",getJVector
vILocal= crdTransf.getVectorLocalCoordFromGlobal(crdTransf.getIVector)
vJLocal= crdTransf.getVectorLocalCoordFromGlobal(crdTransf.getJVector)
# print "vector I en locales:",vILocal
# print "vector J en locales:",vJLocal
ratio1= (vILocal-xc.Vector([1,0])).Norm()
ratio2= (vJLocal-xc.Vector([0,1])).Norm()




    


import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
