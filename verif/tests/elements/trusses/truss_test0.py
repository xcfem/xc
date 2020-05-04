# -*- coding: utf-8 -*-

from __future__ import print_function

# Home made test
# Checks the orientation of the element's unary vectors.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

E= 30e6 #Young modulus
l= 10 #Bar length

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodeHandler)
n1= nodeHandler.newNodeXY(0,0)
n2= nodeHandler.newNodeXY(0,l)

elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''

elements= preprocessor.getElementHandler
elements.dimElem= 2 #Bars defined in a two dimensional space.
elements.defaultMaterial= "elast"
truss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
truss.sectionArea= 1

vI= truss.getIVector3d(True)
vJ= truss.getJVector3d(True)
vK= truss.getKVector3d(True)

# Orientation of the element's unary vectors.
ratio= (vI-geom.Vector3d(0,1,0)).getModulus()
ratio+= (vJ-geom.Vector3d(-1,0,0)).getModulus()
ratio+= (vK-geom.Vector3d(0,0,1)).getModulus()

#print 'vI= ', vI, ' vJ= ',vJ, ' vK= ',vK
#print "ratio= ",ratio

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-15:
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')


