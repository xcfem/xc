# -*- coding: utf-8 -*-
''' Home made test.'''
from __future__ import print_function

import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Quand region:
#
#    L                           K
#     +-------------------------+
#     |                         |
#     |                         |
#     |                         |
#     |                         |
#     +-------------------------+
#    I                           J
#
# Cross-section geometry.
b=  10
h=  20
nDivIJ= 1 # number of divisions I->J direction.
nDivJK= 32 # number of divisions J->K direction.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

elast= typical_materials.defElasticMaterial(preprocessor, "elast",1.0)
sectionGeometryTest= preprocessor.getMaterialHandler.newSectionGeometry("sectionGeometryTest")
regions= sectionGeometryTest.getRegions
elast= regions.newQuadRegion(elast.name)
elast.nDivIJ= nDivIJ
elast.nDivJK= nDivJK
elast.pMin= geom.Pos2d(-h/2.0,-b/2.0)
elast.pMax= geom.Pos2d(h/2.0,b/2.0)
area= elast.getArea()
G= elast.getCenterOfMass()
Iy= elast.getIy()
Iz= elast.getIz()
Pyz= elast.getPyz()

areaTeor= b*h
yGTeor= 0.0
zGTeor= 0.0
izTeor= b*h**3/12.0

ratio1= abs(area-areaTeor)/areaTeor
ratio2= abs(G[0]-yGTeor)
ratio3= abs(G[1]-zGTeor)
ratio4= (Iz-izTeor)/izTeor

'''
print("area= ",area)
print("G= ",G)
print("Iy= ",Iy)
print("Iz= ",Iz)
print("izTeor= ",izTeor)
print("Pyz= ",Pyz)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
''' 

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
