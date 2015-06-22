# -*- coding: utf-8 -*-
CooMax= 10
import xc_base
import geom
import xc
import math
from model import setMgmtUtils as sUtils


prueba= xc.ProblemaEF()
mdlr= prueba.getModelador

puntos= mdlr.getCad.getPoints
pt1= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt2= puntos.newPntIDPos3d(2,geom.Pos3d(CooMax/2,CooMax/2,CooMax/2))
pt3= puntos.newPntIDPos3d(3,geom.Pos3d(CooMax,CooMax,CooMax))

s1= mdlr.getSets.defSet("S1")
sUtils.appendPoints(s1,[pt1,pt2])

s2= mdlr.getSets.defSet("S2")
sUtils.appendPoints(s2,[pt2,pt3])

s3= mdlr.getSets.defSet("S3")
sUtils.appendSets(s3,[s1,s2])
sz= s3.getPoints.size

#for p in pnts:
#  print codigo

#print "sz= ", sz

import os
fname= os.path.basename(__file__)
if (sz==3):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
