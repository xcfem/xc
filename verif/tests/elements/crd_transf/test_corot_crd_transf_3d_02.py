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
mdlr= prueba.getModelador   
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXYZ(0,0,0)
nod= nodos.newNodeXYZ(5,5,5)

# Materials
seccion= typical_materials.defElasticSection3d(mdlr,"seccion",1,1,1,1,1,1)

# Definimos transformaciones geométricas
trfs= mdlr.getTransfCooLoader
lin= trfs.newCorotCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])
    
# Elements definition
elementos= mdlr.getElementLoader

elementos.defaultTransformation= "lin" # Transformación de coordenadas para los nuevos elementos
elementos.defaultMaterial= "seccion"
elementos.defaultTag= 1 #Tag for the next element.
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,2]));

crdTransf= beam3d.getCoordTransf

# \print{"vector I:",getIVector
#print "vector J:",getJVector
#print "vector K:",getKVector}
vIGlobal= crdTransf.getVectorGlobalCoordFromLocal(xc.Vector([1,0,0]))
vJGlobal= crdTransf.getVectorGlobalCoordFromLocal(xc.Vector([0,1,0]))
vKGlobal= crdTransf.getVectorGlobalCoordFromLocal(xc.Vector([0,0,1]))
# \print{"vector I en globales:",vIGlobal
#print "vector J en globales:",vJGlobal
#print "vector K en globales:",vKGlobal}
ratio1= (vIGlobal-crdTransf.getIVector).Norm()
ratio2= (vJGlobal-crdTransf.getJVector).Norm()
ratio3= (vKGlobal-crdTransf.getKVector).Norm()




    


import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12) & (abs(ratio3)<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
