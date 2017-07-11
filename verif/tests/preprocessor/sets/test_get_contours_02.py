# -*- coding: utf-8 -*-
''' Home made test.
    Obtaining the contour of an element set. '''

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

E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad

# Problem type
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# This is the element set:
#
#      7+---+6
#   4   | b |
#   +---+---+5
#   | a |3 
#   +---+
#   1   2   5


n1= nodes.newNodeIDXY(1,0,0)
n2= nodes.newNodeIDXY(2,1,0)
n3= nodes.newNodeIDXY(3,1,1)
n4= nodes.newNodeIDXY(4,0,1)
n6= nodes.newNodeIDXY(5,2,1)
n7= nodes.newNodeIDXY(6,2,2)
n8= nodes.newNodeIDXY(7,1,2)


elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)
# Elements definition
elements= preprocessor.getElementLoader
elements.defaultMaterial= "elast2d"
a= elements.newElement("quad4n",xc.ID([1,2,3,4]))
b= elements.newElement("quad4n",xc.ID([3,5,6,7]))

totalSet= preprocessor.getSets.getSet('total')
polygons= totalSet.getElements.getContours(True)

length= polygons[0].getLongitud()+polygons[1].getLongitud()

ratio1= (length-8.0)/8.0


''' 
print "lentgh= ",length
print "ratio1= ",ratio1
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-9):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
