# -*- coding: utf-8 -*-
from __future__ import print_function
''' Home made test.
    Obtaining the contour of an element set. '''

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density

# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Para no imprimir mensajes de advertencia
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# This is the element set:
#
#      8+---+7
#   4   | c |
#   +---+---+6
#   | a | b |
#   +---+---+
#   1   2   5


n1= nodes.newNodeIDXY(1,0,0)
n2= nodes.newNodeIDXY(2,1,0)
n3= nodes.newNodeIDXY(3,1,1)
n4= nodes.newNodeIDXY(4,0,1)
n5= nodes.newNodeIDXY(5,2,0)
n6= nodes.newNodeIDXY(6,2,1)
n7= nodes.newNodeIDXY(7,2,2)
n8= nodes.newNodeIDXY(8,1,2)
totalSet= preprocessor.getSets.getSet('total')
bnd0= totalSet.getBnd(0.0)
pmin0= bnd0.pMin
pmax0= bnd0.pMax

elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast2d"
a= elements.newElement("FourNodeQuad",xc.ID([1,2,3,4]))
b= elements.newElement("FourNodeQuad",xc.ID([2,5,6,3]))
c= elements.newElement("FourNodeQuad",xc.ID([3,6,7,8]))

bnd1= totalSet.getBnd(0.0)
pmin1= bnd1.pMin
pmax1= bnd1.pMax


ratio= math.sqrt(pmin0.x**2+pmin0.y**2+pmin0.z**2)
ratio+= math.sqrt((pmax0.x-2)**2+(pmax0.y-2)**2+pmax0.z**2)
ratio+= math.sqrt(pmin1.x**2+pmin1.y**2+pmin1.z**2)
ratio+= math.sqrt((pmax1.x-2)**2+(pmax1.y-2)**2+pmax1.z**2)

'''
print('bnd0:', bnd0)
print('bnd1:', bnd1)
print('ratio= ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-12):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
