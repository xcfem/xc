# -*- coding: utf-8 -*-
from __future__ import print_function
import xc_base
import geom
import math

circ1= geom.Circle2d(geom.Pos2d(0.0,0.0),1.0)
alpha= math.radians(0)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
pol1= circ1.getInscribedPolygon(3,0)

ptsTang= pol1.getApproxTangentPositions(v)
pt0= ptsTang[0]
pt0Teor= geom.Pos2d(-0.125,-0.649519)

alpha= math.radians(45)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
ptsTang= pol1.getApproxTangentPositions(v)
pt1Teor= geom.Pos2d(0.4375,-0.32476)
pt1= ptsTang[0]

alpha= math.radians(90)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
ptsTang= pol1.getApproxTangentPositions(v)
pt2Teor= geom.Pos2d(1,0)
pt2= ptsTang[0]

alpha= math.radians(135)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
ptsTang= pol1.getApproxTangentPositions(v)
pt3Teor= geom.Pos2d(0.4375,0.32476)
pt3= ptsTang[0]

alpha= math.radians(180)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
ptsTang= pol1.getApproxTangentPositions(v)
pt3Teor= geom.Pos2d(-0.125,0.649519)
pt3= ptsTang[0]

alpha= math.radians(180)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
ptsTang= pol1.getApproxTangentPositions(v)
pt4Teor= geom.Pos2d(-0.125,0.649519)
pt4= ptsTang[0]

alpha= math.radians(225)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
ptsTang= pol1.getApproxTangentPositions(v)
pt5Teor= geom.Pos2d(-0.5,0.649519)
pt5= ptsTang[0]

alpha= math.radians(270)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
ptsTang= pol1.getApproxTangentPositions(v)
pt6Teor= geom.Pos2d(-0.5,0)
pt6= ptsTang[0]

alpha= math.radians(315)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
ptsTang= pol1.getApproxTangentPositions(v)
pt7Teor= geom.Pos2d(-0.5,-0.649519)
pt7= ptsTang[0]

alpha= math.radians(360)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
ptsTang= pol1.getApproxTangentPositions(v)
pt8Teor= geom.Pos2d(-0.125,-0.649519)
pt8= ptsTang[0]

alpha= math.radians(405)
v= geom.Vector2d(math.cos(alpha),math.sin(alpha))
ptsTang= pol1.getApproxTangentPositions(v)
pt9Teor= geom.Pos2d(0.4375,-0.32476)
pt9= ptsTang[0]

ratio0= pt0.dist(pt0Teor)
ratio1= pt1.dist(pt1Teor)
ratio2= pt2.dist(pt2Teor)
ratio3= pt3.dist(pt3Teor)
ratio4= pt4.dist(pt4Teor)
ratio5= pt5.dist(pt5Teor)
ratio6= pt6.dist(pt6Teor)
ratio7= pt7.dist(pt7Teor)
ratio8= pt8.dist(pt8Teor)
ratio9= pt9.dist(pt9Teor)



'''
print("pt0: ",pt0,"\n")
print("pt1: ",pt1,"\n")
print("pt2: ",pt2,"\n")
print("pt3: ",pt3,"\n")
print("pt4: ",pt4,"\n")
print("pt5: ",pt5,"\n")
print("pt6: ",pt6,"\n")
print("pt7: ",pt7,"\n")
print("pt8: ",pt8,"\n")
print("pt9: ",pt9,"\n")
print("ratio0= ",ratio0,"\n")
print("ratio1= ",ratio1,"\n")
print("ratio2= ",ratio2,"\n")
print("ratio3= ",ratio3,"\n")
print("ratio4= ",ratio4,"\n")
print("ratio5= ",ratio5,"\n")
print("ratio6= ",ratio6,"\n")
print("ratio7= ",ratio7,"\n")
print("ratio8= ",ratio8,"\n")
print("ratio9= ",ratio9,"\n")
'''

import os
fname= os.path.basename(__file__)
if math.fabs(ratio0)<1e-6 and math.fabs(ratio1)<1e-6 and math.fabs(ratio2)<1e-6 and math.fabs(ratio3)<1e-6 and math.fabs(ratio4)<1e-6  and math.fabs(ratio5)<1e-6 and math.fabs(ratio6)<1e-6 and math.fabs(ratio7)<1e-6 and math.fabs(ratio8)<1e-6 and math.fabs(ratio9)<1e-6:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
