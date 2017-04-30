# -*- coding: utf-8 -*-
# home made test
# Seleccion de elementos a partir de nodos de un conjunto.
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0.0,0.0,0.0)
nod= nodes.newNodeXYZ(1.0,0.0,0.0)
nod= nodes.newNodeXYZ(1.0,1.0,0.0)
nod= nodes.newNodeXYZ(0.0,1.0,0.0)
nod= nodes.newNodeXYZ(0.0,0.0,1.0)
nod= nodes.newNodeXYZ(1.0,0.0,1.0)
nod= nodes.newNodeXYZ(1.0,1.0,1.0)
nod= nodes.newNodeXYZ(0.0,1.0,1.0)

# Geometric transformations
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,1])

# Materials
seccion= typical_materials.defElasticSection3d(preprocessor, "seccion",1,1,1,1,1,1)

elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin" # Coord. transformation.
elementos.defaultMaterial= "seccion"
elementos.defaultTag= 1 #Tag for next element.
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,2]));
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([2,3]))
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([3,4]))
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([4,1]))
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,3]))
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([2,4]))

beam3d= elementos.newElement("elastic_beam_3d",xc.ID([5,6]))
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([6,7]))
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([7,8]))
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([8,5]))
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([5,7]))
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([6,8]))

# Definimos el conjunto de prueba
prb1= preprocessor.getSets.defSet("prb1")

nodes= preprocessor.getSets.getSet("total").getNodes
for n in nodes:
  coord= n.getCoo
  if(abs(coord[1]-0.0)<1e-2):
    prb1.getNodes.append(n)



'''
nodes= prb1.getNodes
for n in nodes:
  coord= n.getCoord
  print "  nodo: ",n.tag," x= ",coord[0],", y= ",coord[1],", z= ",coord[2]
'''
elementos= preprocessor.getSets.getSet("total").getElements
for e in elementos:
  if((abs(e.getMaxCooNod(1)-0.0)<1e-2) & (abs(e.getMinCooNod(1)-0.0)<1e-2)):
    prb1.getElements.append(e)

''' 
elementos= prb1.getElements
for e in elements:
  print "tag= ",e.tag," nodo I:",e.nod[0].tag," nodo J:",e.nod[1].tag
'''
import os
fname= os.path.basename(__file__)
if(abs(prb1.getNumNodes-4)<1e-5) & (abs(prb1.getNumElements-2)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."




