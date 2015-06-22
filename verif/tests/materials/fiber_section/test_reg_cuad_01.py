# -*- coding: utf-8 -*-
# Test de funcionamiento del comando reg_cuad
import math
import xc_base
import geom
import xc
from materials import typical_materials

ancho= 10
canto= 20
ndivIJ= 32
ndivJK= 32
nFibTeor= ndivIJ*ndivJK
areaTeor= ancho*canto
iyTeor= 1/12.0*ancho*canto**3
izTeor= 1/12.0*canto*ancho**3
y0= 5
z0= 3.2

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
# Definimos materiales         
fy= 50.0 # Tensión de cedencia del acero.
E= 30000.0 # Módulo de Young del acero.
acero= typical_materials.defSteel01(mdlr,"acero",E,fy,0.001)

# Secciones
geomRectang= mdlr.getMaterialLoader.newSectionGeometry("geomRectang")
y1= ancho/2.0
z1= canto/2.0
regiones= geomRectang.getRegions
acero= regiones.newQuadRegion("acero")
acero.nDivIJ= ndivIJ
acero.nDivJK= ndivJK
acero.pMin= geom.Pos2d(y0-y1,z0-z1)
acero.pMax= geom.Pos2d(y0+y1,z0+z1)

'''
print "num. regiones: ",numRegiones
print "num. capas armado: ",numReinfLayers 
print "num. barras armado: ",numReinfBars
print "num. teselas: ",numCells
for_each_region
  print "  REGION "
  print "num. teselas: ",numCells
'''
           
materiales= mdlr.getMaterialLoader
rectang= materiales.newMaterial("fiber_section_3d","rectang")
fiberSectionRepr= rectang.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomRectang")
rectang.setupFibers()
fibras= rectang.getFibers()

nfibras= fibras.getNumFibers()
sumAreas= fibras.getSumaAreas(1)
Iz= fibras.getIz(1.0,y0)
Iy= fibras.getIy(1.0,z0)
zCdg= fibras.getCdgZ()
yCdg= fibras.getCdgY()


ratio1= (nfibras-nFibTeor)/nFibTeor
ratio2= (sumAreas-areaTeor)/areaTeor
ratio3= (yCdg-y0)/max(1.0,y0)
ratio4= (zCdg-z0)/max(1.0,z0)
ratio5= (Iz-izTeor)/izTeor
ratio6= (Iy-iyTeor)/iyTeor

''' 
print "num. fibras: ",(nfibras)
print "ratio1= ",(ratio1)

print "suma áreas: ",(sumAreas)
print "área teórica: ",(areaTeor)
print "ratio2= ",(ratio2)

print "yCdg= ",yCdg
print "y0= ",y0
print "ratio3= ",ratio3

print "zCdg= ",(zCdg)
print "z0= ",z0
print "ratio4= ",ratio4

print "Iz= ",(Iz)
print "IzTeor= ",izTeor
print "ratio5= ",(ratio5)

print "Iy= ",Iy
print "IyTeor= ",iyTeor
print "ratio6= ",(ratio6)
 '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5) & (abs(ratio5)<1e-3) & (abs(ratio6)<1e-3) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
