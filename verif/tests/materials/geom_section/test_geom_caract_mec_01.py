# -*- coding: utf-8 -*-
# Home made test

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

Es= 2.1e11
n= 15
Ec= Es/n
lado= 1.0

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador

hormigon= typical_materials.defElasticMaterial(mdlr,"hormigon",Ec)

geomPrueba= mdlr.getMaterialLoader.newSectionGeometry("geomPrueba")
regiones= geomPrueba.getRegions
hormigon= regiones.newQuadRegion("hormigon")
hormigon.pMin= geom.Pos2d(0.0,0.0)
hormigon.pMax= geom.Pos2d(1.0,1.0)

area= geomPrueba.getAreaSeccHomogeneizada(Ec)
G= geomPrueba.getCdgSeccHomogeneizada(Ec)
Iy= geomPrueba.getIySeccHomogeneizada(Ec)
Iz= geomPrueba.getIzSeccHomogeneizada(Ec)
Pyz= geomPrueba.getPyzSeccHomogeneizada(Ec)

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
  print "test_geom_caract_mec_01: ok."
else:
  print "test_geom_caract_mec_01: ERROR."
