# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os

NumDiv= 25
CooMax= 10
xOrg= 1
yOrg= 2
zOrg= 3
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador

puntos= mdlr.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(CooMax,CooMax,CooMax))
rsMap= mdlr.getCad.getReferenceSystems

rs= rsMap.newReferenceSystem("cartesianas")
rs.org= geom.Pos3d(xOrg,yOrg,zOrg)
ratio1= rs.org.x-xOrg
ratio2= rs.org.y-yOrg
ratio3= rs.org.z-zOrg


# print "ratio1= ",(ratio1)
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12) & (abs(ratio3)<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
