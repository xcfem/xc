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

sideLength= 1.0
n1= 3
n2= 2

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

elast= typical_materials.defElasticMaterial(preprocessor, "elast",1.0)
sectionGeometryTest= preprocessor.getMaterialHandler.newSectionGeometry("sectionGeometryTest")
regions= sectionGeometryTest.getRegions
elast= regions.newQuadRegion("elast")
elast.nDivIJ= n1
elast.nDivJK= n2
elast.pMin= geom.Pos2d(0.0,0.0)
elast.pMax= geom.Pos2d(sideLength,sideLength)
rejilla= elast.genGrid()
points= rejilla.getPoints()

numQuads= points.getNumQuads()
numPoints= points.getNumPoints()
numFilas= points.nRow
numCols= points.nCol
area= points.getArea()
G= points.getCenterOfMass()
Ix= points.Ix()
Iy= points.Iy()
Pxy= points.Pxy()
deltaX= points.getX(1,2)-points.getX(1,1)
deltaY= points.getY(2,2)-points.getY(1,1)

''' 
for  \inicio{i=1} \continua{i<=numFilas} \incremento{i=(i+1)
  for j in range(1,j+1):
                                 \bucle

                                      print "x(",i,",",j,")= ",x(i,j
                                      print " y(",i,",",j,")= ",y(i,j




'''

numQuadsTeor= n1*n2
numFilasTeor= (n2+1)
numColsTeor= (n1+1)
theorNumPoints= numFilasTeor*numColsTeor
areaTeor= sideLength*sideLength
xGTeor= sideLength/2.0
yGTeor= sideLength/2.0
iTeor= (sideLength)**4/12.0
deltaXTeor= sideLength/n1
deltaYTeor= sideLength/n2

ratio1= ((numQuads-numQuadsTeor)/numQuadsTeor)
ratio2= ((numFilas-numFilasTeor)/numFilasTeor)
ratio3= ((numCols-numColsTeor)/numColsTeor)
ratio4= ((numPoints-theorNumPoints)/theorNumPoints)
ratio5= ((area-areaTeor)/areaTeor)
ratio6= ((G.x-xGTeor)/xGTeor)
ratio7= ((G.y-yGTeor)/yGTeor)
ratio8= ((Ix-iTeor)/iTeor)
ratio9= ((Iy-iTeor)/iTeor)
ratio10= ((deltaX-deltaXTeor)/deltaXTeor)
ratio11= ((deltaY-deltaYTeor)/deltaYTeor)

''' 
print "area= ",area
print "numQuads= ",numQuads
print "numPoints= ",numPoints
print "numFilas= ",numFilas
print "numCols= ",numCols
print "area= ",area
print "G= ",G.x, G.y
print "Ix= ",Ix
print "Iy= ",Iy
print "iTeor= ",iTeor
print "Pxy= ",Pxy
print "deltaX= ",deltaX
print "deltaY= ",deltaY
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5
print "ratio6= ",ratio6
print "ratio7= ",ratio7
print "ratio8= ",ratio8
print "ratio9= ",ratio9
print "ratio10= ",ratio10
print "ratio11= ",ratio11
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) & (abs(ratio5)<1e-15) & (abs(ratio6)<1e-15) & (abs(ratio7)<1e-15) & (abs(ratio8)<1e-15) & (abs(ratio9)<1e-14) & (abs(ratio10)<1e-15) & (abs(ratio11)<1e-15) & (abs(Pxy)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
