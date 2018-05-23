# -*- coding: utf-8 -*-
# Home made test

from __future__ import division
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

alpha= math.radians(45)
R= 5*math.sqrt(2)
r= 4.0*math.sqrt(2)
t= R-r
n1= 1
n2= 100

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

elast= typical_materials.defElasticMaterial(preprocessor, "elast",1.0)
sectionGeometryTest= preprocessor.getMaterialHandler.newSectionGeometry("sectionGeometryTest")
regions= sectionGeometryTest.getRegions
elast= regions.newCircularRegion("elast")
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
numPoints= points.getNumPoints()
area= points.getArea()
G= points.getCenterOfMass()
Iy= points.Iy()
Iz= (points.Ix()+area*(G.y)**2)
Pyz= points.Pxy()
deltaX= points.getX(1,2)-points.getX(1,1)
deltaY= points.getY(2,1)-points.getY(1,1)

''' 
for                          \inicio{i=1} \continua{i<=numFilas} \incremento{i=(i+1)
  for j in range(1,j+1):
                                 \bucle

                                      print "x(",i,",",j,")= ",x(i,j
                                      print " y(",i,",",j,")= ",y(i,j




'''

def sqr(x):
  return x*x

numQuadsTeor= n1*n2
numFilasTeor= n2+1
numColsTeor= n1+1
theorNumPoints= numFilasTeor*numColsTeor
areaTeor= alpha*((R)**2-(r)**2)
areaAcad= 14.1372
yGAcad= 5.7532
yGTeor= R*((2*math.sin(alpha)/(3*alpha)*(1-t/R+1/(2-t/R))))
zGAcad= 0.0
zGTeor= 0.0
iyAcad= 105.3119
F1= (1-3*t/2/R+(t/R)**2-(t/R)**3/4.0)
F2= (alpha+math.sin(alpha)*math.cos(alpha)-2*sqr(math.sin(alpha))/alpha)
F3= (sqr(t)*sqr(math.sin(alpha))/(3*sqr(R)*alpha*(2-t/R)))
F4= (1-t/R+sqr(t)/(6*sqr(R)))
F5= (R**3*t*(F1*F2+F3*F4))
izTeor= (F5+areaTeor*sqr(yGTeor))
izAcad= 474.3119
iyTeor= (R**3*t*F1*(alpha-math.sin(alpha)*math.cos(alpha)))
deltaXTeor= t/n1/math.sqrt(2)
deltaYTeor= math.pi*r/n2/math.sqrt(2)/2.0

ratio1= ((numQuads-numQuadsTeor)/numQuadsTeor)
ratio2= ((numFilas-numFilasTeor)/numFilasTeor)
ratio3= ((numCols-numColsTeor)/numColsTeor)
ratio4= ((numPoints-theorNumPoints)/theorNumPoints)
ratio5= ((area-areaTeor)/areaTeor)
ratio6= ((G.y-yGTeor)/yGTeor)
ratio7= (G.x-zGTeor)
ratio8= ((Iy-iyTeor)/iyTeor)
ratio9= ((Iz-izTeor)/izTeor)
ratio10= ((deltaX-deltaXTeor)/deltaXTeor)
ratio11= ((deltaY-deltaYTeor)/deltaYTeor)

'''
print "numQuads= ",numQuads
print "numPoints= ",numPoints
print "numFilas= ",numFilas
print "numCols= ",numCols
print "area= ",area
print "areaTeor= ",areaTeor
print "areaAcad= ",areaAcad
print "G= ",G
print "yGAcad= ",yGAcad
print "yGTeor= ",yGTeor
print "zGAcad= ",zGAcad
print "zGTeor= ",zGTeor
print "izAcad= ",izAcad
print "izTeor= ",izTeor
print "Iz= ",Iz
print "iyAcad= ",iyAcad
print "iyTeor= ",iyTeor
print "Iy= ",Iy
print "Pyz= ",Pyz
print "deltaX= ",deltaX
print "deltaXTeor= ",deltaXTeor
print "deltaY= ",deltaY
print "deltaYTeor= ",deltaYTeor
print "deltaY/deltaYTeor= ",(deltaY/deltaYTeor)
print "ratio1= ", ratio1
print "ratio2= ", ratio2
print "ratio3= ", ratio3
print "ratio4= ", ratio4
print "ratio5= ", ratio5
print "ratio6= ", ratio6
print "ratio7= ", ratio7
print "ratio8= ", ratio8
print "ratio9= ", ratio9
print "ratio10= ", ratio10
print "ratio11= ", ratio11
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) & (abs(ratio5)<1e-4) & (abs(ratio6)<1e-4) & (abs(ratio7)<1e-13) & (abs(ratio8)<1e-4) & (abs(ratio9)<1e-4) & (abs(ratio10)<1e-15)  & (abs(ratio11)<1e-2) & (abs(Pyz)<1e-13):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
