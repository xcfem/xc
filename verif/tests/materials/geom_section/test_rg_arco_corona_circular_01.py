# -*- coding: utf-8 -*-
# Home made test

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

alpha= math.radians(45)
R= 5*math.sqrt(2)
r= 4*math.sqrt(2)
t= R-r
n1= 1
n2= 1

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

elast= typical_materials.defElasticMaterial(preprocessor, "elast",1.0)
geomPrueba= preprocessor.getMaterialLoader.newSectionGeometry("geomPrueba")
regiones= geomPrueba.getRegions
elast= regiones.newCircularRegion("elast")
elast.nDivRad= n1
elast.nDivCirc= n2
elast.center= geom.Pos2d(0,0)
elast.extRad= R
elast.intRad= r
elast.initAngle= math.pi/2.0-alpha
elast.finalAngle= math.pi/2.0+alpha
rejilla= elast.genGrid()
points= rejilla.getPoints()
numFilas= points.nRow
numCols= points.nCol
numQuads= points.getNumQuads()
numPuntos= points.getNumPoints()
area= points.getArea()
G= points.getCdg()
Iy= points.Iy()
Iz= (points.Ix()+area*(G.y)**2)
Pyz= points.Pxy()
deltaX= points.getX(1,2)-points.getX(1,1)
deltaY= points.getY(2,2)-points.getY(1,1)


''' 
for i in range(1,i+1):
  for j in range(1,j+1):
                                 \bucle

                                      print "x(",i,",",j,")= ",x(i,j
                                      print " y(",i,",",j,")= ",y(i,j




'''

numQuadsTeor= n1*n2
numFilasTeor= n2+1
numColsTeor= n1+1
numPuntosTeor= numFilasTeor*numColsTeor
areaAcad= 9
yGAcad= 4.5185
zGAcad= 0.0
zGTeor= 0.0
iyAcad= 61.5000
izAcad= 184.5000

ratio1= ((numQuads-numQuadsTeor)/numQuadsTeor)
ratio2= ((numFilas-numFilasTeor)/numFilasTeor)
ratio3= ((numCols-numColsTeor)/numColsTeor)
ratio4= ((numPuntos-numPuntosTeor)/numPuntosTeor)
ratio5= ((area-areaAcad)/areaAcad)
ratio7= (G.x-zGAcad)
ratio6= ((G.y-yGAcad)/yGAcad)
ratio8= ((Iy-iyAcad)/iyAcad)
ratio9= ((Iz-izAcad)/izAcad)


''' 
print "numQuads= ",numQuads
print "numQuadsTeor= ",numQuadsTeor
print "numPuntos= ",numPuntos
print "numFilas= ",numFilas
print "numCols= ",numCols
print "area= ",area
print "areaAcad= ",areaAcad
print "G= ",G
print "yGAcad= ",yGAcad
print "zGAcad= ",zGAcad
print "izAcad= ",izAcad
print "Iz= ",Iz
print "iyAcad= ",iyAcad
print "Iy= ",Iy
print "Pyz= ",Pyz
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5
print "ratio6= ",ratio6
print "ratio7= ",ratio7
print "ratio8= ",ratio8
print "ratio9= ",ratio9
 '''

import os
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) & (abs(ratio5)<1e-15) & (abs(ratio6)<1e-5)& (abs(ratio7)<1e-15) & (abs(ratio8)<1e-15) & (abs(ratio9)<1e-15) & (abs(Pyz)<1e-13)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

