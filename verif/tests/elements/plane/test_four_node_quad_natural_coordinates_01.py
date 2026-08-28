# -*- coding: utf-8 -*-
''' Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 2.1e6 # Young modulus of the steel en kg/cm2.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.

import geom
import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)


delta= 10.1
n1= modelSpace.newNode(0+delta,0+delta)
n2= modelSpace.newNode(1+delta,0+delta)
n3= modelSpace.newNode(1+delta,1+delta)
n4= modelSpace.newNode(0+delta,1+delta)

# Define material.
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu, dens)
# Define elements.
modelSpace.setDefaultMaterial(elast2d)
elem= modelSpace.newElement("FourNodeQuad",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))

c= elem.getPosCentroid(True)
On= elem.getNaturalCoordinates(geom.Pos2d(c.x, c.y), True)
p1n= elem.getNaturalCoordinates(n1.getInitialPos2d, True)
p2n= elem.getNaturalCoordinates(n2.getInitialPos2d, True)
p3n= elem.getNaturalCoordinates(n3.getInitialPos2d, True)
p4n= elem.getNaturalCoordinates(n4.getInitialPos2d, True)

ratio= 0.0
ratio1= abs(On.r)
ratio= max(ratio, ratio1)
ratio2= abs(On.s)
ratio= max(ratio, ratio2)
ratio3= abs(p1n.r+1.0)
ratio= max(ratio,ratio3)
ratio4= abs(p1n.s+1.0)
ratio= max(ratio,ratio4)
ratio5= abs(p2n.r-1.0)
ratio= max(ratio,ratio5)
ratio6= abs(p2n.s+1.0)
ratio= max(ratio,ratio6)
ratio7= abs(p3n.r-1.0)
ratio= max(ratio,ratio7)
ratio8= abs(p3n.s-1.0)
ratio= max(ratio,ratio8)
ratio9= abs(p4n.r+1.0)
ratio= max(ratio,ratio9)
ratio10= abs(p4n.s-1.0)
ratio= max(ratio,ratio10)

'''
print("Centroid natural coordinates: ", On)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print(n1.getInitialPos3d)
print(p1n)
print("ratio3= ", ratio3)
print("ratio4= ", ratio4)
print(n2.getInitialPos3d)
print(p2n)
print("ratio5= ", ratio5)
print("ratio6= ", ratio6)
print(n3.getInitialPos3d)
print(p3n)
print("ratio7= ",ratio7)
print("ratio8= ",ratio8)
print(n4.getInitialPos3d)
print(p4n)
print("ratio9= ",ratio9)
print("ratio10= ",ratio10)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio < 1e-14):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLocalAxes()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
