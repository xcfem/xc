# -*- coding: utf-8 -*-
# Ejemplo tomado de la página 258 del Jiménez Montoya (14a. edición)

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

Es= 2.1e11
n= 7.365
Ec= Es/n
nuc= 0.3
Gc= Ec/(2*(1+nuc))
b= 1.0 # Ancho eficaz de ala.
bw= 0.25 # Ancho del nervio.
hf= 0.08 # Canto del ala.
d= 0.57 # Canto útil.
diamBar= 22e-3 # Diámetro de los redondos.
areaBar= math.pi*(diamBar/2)**2 # Área de los redondos.

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
hormigon= typical_materials.defElasticMaterial(mdlr,"hormigon",Ec)
acero= typical_materials.defElasticMaterial(mdlr,"acero",Es)
geomPrueba= mdlr.getMaterialLoader.newSectionGeometry("geomPrueba")
regiones= geomPrueba.getRegions

ala= regiones.newQuadRegion("hormigon")# Ala
ala.pMin= geom.Pos2d(d-hf,0.0)
ala.pMax= geom.Pos2d(d,b)
nervio= regiones.newQuadRegion("hormigon")# Nervio
nervio.pMin= geom.Pos2d(0.0,b/2-bw/2)
nervio.pMax= geom.Pos2d(d-hf,b/2+bw/2)

armaduras= geomPrueba.getReinfLayers
armaduraA= armaduras.newStraightReinfLayer("acero")
armaduraA.numReinfBars= 5
armaduraA.barArea= areaBar
armaduraA.p1= geom.Pos2d(0.0,b/2-bw/2+0.05)
armaduraA.p2= geom.Pos2d(0.0,b/2+bw/2-0.05)

prb= typical_materials.defElasticSection3d(mdlr,"prb",0.0,Ec,Gc,0.0,0.0,1.0)
prb.sectionGeometry("geomPrueba")
paramSeccion= prb.sectionProperties
area= paramSeccion.A
Iy= paramSeccion.Iy
Iz= paramSeccion.Iz

areaTeor= b*hf+(d-hf)*bw+n*5*areaBar
izTeor= 0.0081
iyTeor= 0.0073

ratio1= ((area-areaTeor)/areaTeor)
ratio2= ((Iz-izTeor)/izTeor)
ratio3= ((Iy-iyTeor)/iyTeor)

''' 
print "area= ",area
print "areaTeor= ",areaTeor
print "Iy= ",Iy
print "iyTeor= ",iyTeor
print "Iz= ",Iz
print "izTeor= ",izTeor
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
 '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-2) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
