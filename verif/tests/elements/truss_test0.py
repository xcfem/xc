# -*- coding: utf-8 -*-

#Test from Ansys manual
#Reference:  Strength of Materials, Part I, Elementary Theory and Problems, pg. 26, problem 10

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

E= 30e6 #Young modulus
l= 10 #Bar length

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeLoader

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(0,l,0)

elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''

elements= preprocessor.getElementLoader
elements.dimElem= 2 #Bars defined in a two dimensional space.
elements.defaultMaterial= "elast"
elements.defaultTag= 1 #Tag for the next element.
truss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]));
truss.area= 1

vI= truss.getIVector3d(True)
vJ= truss.getJVector3d(True)
vK= truss.getKVector3d(True)

ratio= (vI-geom.Vector3d(0,1,0)).getModulo()
ratio+= (vJ-geom.Vector3d(-1,0,0)).getModulo()
ratio+= (vK-geom.Vector3d(0,0,1)).getModulo()

#print 'vI= ', vI, ' vJ= ',vJ, ' vK= ',vK
#print "ratio= ",ratio

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-15:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


