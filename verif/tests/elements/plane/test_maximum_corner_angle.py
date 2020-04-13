# -*- coding: utf-8 -*-
''' Test made from the example 7.2 of the book: 
    Introducción Al Estudio Del Elemento Finito en Ingeniería. 
    Segunda Edición. Tirupathi R. Chandrupatla, Ashok D. Belegundu. 
    Pearson Educación, 1999
    isbn={9789701702604},
    url={https://books.google.ch/books?id=auQwAAAACAAJ},
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density

# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Don't pring warnings
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)


n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(1,0)
n3= nodes.newNodeXY(2,1)
n4= nodes.newNodeXY(1,1)


elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast2d"
quad4n= elements.newElement("FourNodeQuad",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))

maxAngle= quad4n.getMaximumCornerAngle(True)
testValue= 3.0*math.pi/4.0

ratio1= (maxAngle-testValue)/testValue

''' 
print("testValue= ",math.degrees(testValue))
print("maxAngle= ", math.degrees(maxAngle))
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-9):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
