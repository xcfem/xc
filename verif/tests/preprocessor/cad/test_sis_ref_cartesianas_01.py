# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDiv= 25
CooMax= 10
xOrg= 1
yOrg= 2
zOrg= 3
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

points= preprocessor.getCad.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPntIDPos3d(2,geom.Pos3d(CooMax,CooMax,CooMax))
rsMap= preprocessor.getCad.getReferenceSystems

rs= rsMap.newReferenceSystem("cartesianas")
rs.org= geom.Pos3d(xOrg,yOrg,zOrg)
ratio1= rs.org.x-xOrg
ratio2= rs.org.y-yOrg
ratio3= rs.org.z-zOrg


# print "ratio1= ",(ratio1)
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12) & (abs(ratio3)<1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
