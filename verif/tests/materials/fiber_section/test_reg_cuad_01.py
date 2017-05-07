# -*- coding: utf-8 -*-

# Test for checking the reg_cuad command

import math
import xc_base
import geom
import xc
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

width= 10 # Cross-section width [cm]
depth= 20 # Cross-section height [cm]
nDivIJ= 32 # number of cells in IJ direction
nDivJK= 32 # number of cells in JK direction
nFibTeor= nDivIJ*nDivJK
areaTeor= width*depth
iyTeor= 1/12.0*width*depth**3
izTeor= 1/12.0*depth*width**3
y0= 5
z0= 3.2

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

#  Materials definition: uniaxial bilinear steel
fy= 50.0   # yield strength
E= 30000.0 # initial elastic tangent
b=0.001    # strain-hardening ratio: ratio between post-yield tangent and initial elastic tangent
acero= typical_materials.defSteel01(preprocessor=preprocessor,name="acero",E=E,fy=fy,b=b)

# Section geometry
# creation
geomRectang= preprocessor.getMaterialLoader.newSectionGeometry("geomRectang")
y1= width/2.0
z1= depth/2.0
#filling with regions
regiones= geomRectang.getRegions
#generation of a quadrilateral region of the specified sizes and number of
#divisions for the cells (fibers) generation
acero= regiones.newQuadRegion("acero")   
acero.nDivIJ= nDivIJ
acero.nDivJK= nDivJK
acero.pMin= geom.Pos2d(y0-y1,z0-z1)
acero.pMax= geom.Pos2d(y0+y1,z0+z1)

'''
print "num. regiones: ",numRegiones
print "num. capas armado: ",numReinfLayers 
print "num. barras armado: ",getNumReinfBars
print "num. teselas: ",numCells
for_each_region
  print "  REGION "
  print "num. teselas: ",numCells
'''
           
materiales= preprocessor.getMaterialLoader
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
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5) & (abs(ratio5)<1e-3) & (abs(ratio6)<1e-3) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
