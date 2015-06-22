# -*- coding: utf-8 -*-
# Home made test

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

Es= 2.1e11
nu= 0.3
Gs= 0.01*Es/(2*(1+nu))
radio= 1.2/2.0
espesor= 10.3e-3
RExt= radio+espesor

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
acero= typical_materials.defElasticMaterial(mdlr,"acero",Es)
geomTuberia= mdlr.getMaterialLoader.newSectionGeometry("geomTuberia")
regiones= geomTuberia.getRegions
acero= regiones.newCircularRegion("acero")

acero.nDivCirc= 1000
acero.centro= geom.Pos2d(0.0,0.0)
acero.extRad= radio+espesor
acero.intRad= radio
acero.initAngle= 0.0
acero.finalAngle= 2*math.pi

area= acero.getArea()
Iy= acero.getIy()
Iz= acero.getIz()
Pyz= acero.getPyz()


G= geomTuberia.getCdgSeccHomogeneizada(Es)




areaTeor= math.pi*(RExt**2-radio**2)
ratio1= ((area-areaTeor)/areaTeor)
ITeor= math.pi/4*(RExt**4-radio**4)
ratio2= ((Iy-ITeor)/ITeor)
ratio3= ((Iz-ITeor)/ITeor)
ratio4= G.Norm()

''' 
print "area= ",area
print "areaTeor= ",areaTeor
print "ratio1= ",ratio1
print "Iy= ",Iy
print "ITeor= ",ITeor
print "ratio2= ",ratio2
print "Iz= ",Iz
print "ITeor= ",ITeor
print "ratio3= ",ratio3
print "G= ",G
print "ratio4= ",ratio4
print "Pyz= ",Pyz
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-4) & (abs(ratio4)<1e-13) & (abs(Pyz)<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
