# -*- coding: utf-8 -*-
# Home made test

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

Es= 2.1e11
n= 15.0
Ec= Es/n
areaFi16= 2.01e-4

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

acero= typical_materials.defElasticMaterial(preprocessor, "acero",Es)

geomPrueba= preprocessor.getMaterialLoader.newSectionGeometry("geomPrueba")
reinforcement= geomPrueba.getReinfLayers
armaduraA= reinforcement.newStraightReinfLayer("acero")
armaduraA.numReinfBars= 2
armaduraA.barDiam= 16e-3
armaduraA.barArea= areaFi16
armaduraA.p1= geom.Pos2d(0.05,0.95) # Armadura inferior.
armaduraA.p2= geom.Pos2d(0.05,0.05)
armaduraB= reinforcement.newStraightReinfLayer("acero")
armaduraB.numReinfBars= 2
armaduraB.barDiam= 16e-3
armaduraB.barArea= areaFi16
armaduraB.p1= geom.Pos2d(0.95,0.95) # Armadura inferior.
armaduraB.p2= geom.Pos2d(0.95,0.05)

nRebars= reinforcement.getNumReinfBars
area= geomPrueba.getAreaSeccHomogeneizada(Ec)
G= geomPrueba.getCdgSeccHomogeneizada(Ec)
Iy= geomPrueba.getIySeccHomogeneizada(Ec)
Iz= geomPrueba.getIzSeccHomogeneizada(Ec)
Pyz= geomPrueba.getPyzSeccHomogeneizada(Ec)

areaTeor= (n*4*areaFi16)
yGTeor= 0.5
zGTeor= 0.5
iTeor= (n*4*math.pi/4*(8e-3)**4+(0.45)**2*areaTeor)

ratio1= ((area-areaTeor)/areaTeor)
ratio2= ((G[0]-yGTeor)/yGTeor)
ratio3= ((G[1]-zGTeor)/zGTeor)
ratio4= ((Iy-iTeor)/iTeor)
ratio5= ((Iz-iTeor)/iTeor)
ratio6= ((nRebars-4)/4)

''' 
print "area= ",area
print "areaTeor= ",areaTeor
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
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-7) & (abs(ratio5)<1e-7) & (abs(ratio6)<1e-17) :
  print "test_geom_caract_mec_02: ok."
else:
  print "test_geom_caract_mec_02: ERROR."
