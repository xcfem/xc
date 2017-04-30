# -*- coding: utf-8 -*-
# Home made test

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

lado= 1.0

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

elast= typical_materials.defElasticMaterial(preprocessor, "elast",1.0)
geomPrueba= preprocessor.getMaterialLoader.newSectionGeometry("geomPrueba")
regiones= geomPrueba.getRegions
elast= regiones.newQuadRegion("elast")
elast.pMin= geom.Pos2d(0.0,0.0)
elast.pMax= geom.Pos2d(lado,lado)
area= elast.getArea()
G= elast.getCdg()
Iy= elast.getIy()
Iz= elast.getIz()
Pyz= elast.getPyz()

areaTeor= lado*lado
yGTeor= lado/2.0
zGTeor= lado/2.0
iTeor= (lado)**4/12.0

ratio1= ((area-areaTeor)/areaTeor)
ratio2= ((G[0]-yGTeor)/yGTeor)
ratio3= ((G[1]-zGTeor)/zGTeor)
ratio4= ((Iy-iTeor)/iTeor)
ratio5= ((Iz-iTeor)/iTeor)

''' 
print "area= ",area
print "G= ",G
print "Iy= ",Iy
print "Iz= ",Iz
print "iTeor= ",iTeor
print "Pyz= ",Pyz
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5
 '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) & (abs(ratio5)<1e-15) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
