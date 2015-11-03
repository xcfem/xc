# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os

R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(R,0.0,0.0))
puntos.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.0))
puntos.newPntFromPos3d(geom.Pos3d(0.0,R,0.0))

lineas= preprocessor.getCad.getLines
lineas.defaultTag= 1
l= lineas.newCircleArc(1,2,3)
angc= l.getAngle()
long= l.getLong()

        # \arco_circunf[1]
        #   {
        #     \p1{1} \p2{3} \p3{2}
        #     angc= getAngComp
        #     long= long
        #   }


angteor= math.pi/2.0
ratio1= (angteor/angc)
lteor= 2*math.pi*R/4.0
ratio2= (lteor/long)

# print "Ángulo comprendido: ",math.degrees(angc)
# print "Ángulo teórico: ",math.degrees(angteor)
# print "Longitud del arco: ",long
# print "Longitud teórica: ",lteor

fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) and (abs(ratio2-1.0)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

