# -*- coding: utf-8 -*-
''' Home made test.
    Obtaining the contour of an element set. '''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad

# Problem type
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader
predefined_spaces.gdls_elasticidad2D(nodos)

# This is the element set:
#
#      8+---+7
#   4   | c |
#   +---+---+6
#   | a | b |
#   +---+---+
#   1   2   5


n1= nodos.newNodeIDXY(1,0,0)
n2= nodos.newNodeIDXY(2,1,0)
n3= nodos.newNodeIDXY(3,1,1)
n4= nodos.newNodeIDXY(4,0,1)
n5= nodos.newNodeIDXY(5,2,0)
n6= nodos.newNodeIDXY(6,2,1)
n7= nodos.newNodeIDXY(7,2,2)
n8= nodos.newNodeIDXY(8,1,2)


elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)
# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "elast2d"
a= elementos.newElement("quad4n",xc.ID([1,2,3,4]))
b= elementos.newElement("quad4n",xc.ID([2,5,6,3]))
c= elementos.newElement("quad4n",xc.ID([3,6,7,8]))

totalSet= preprocessor.getSets.getSet('total')
polygons= totalSet.getElements.getContours(True)

length= polygons[0].getLongitud()

ratio1= (length-8.0)/8.0


''' 
print "lentgh= ",length
print "ratio1= ",ratio1
 '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-9):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
