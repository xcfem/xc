# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2018, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es" 

from rough_calculations import predim_diaphragm_wall as pdw

diafWall1=pdw.DiaphragmWallRoughModel(digHeight=9,phreaticLevel=2.4,distAnch=9,firstExcavHeight=1,distLastAnchExc=9,unifLoad=60,distLoadWall=0)
vCalcDW1=[diafWall1.getEffectDigHeight(),diafWall1.getFffectDistAnch(),diafWall1.getWallEmbedmentDepth(),diafWall1.getWallThickness()]
vRefDW1=[12,11,3.6,0.8]

diafWall2=pdw.DiaphragmWallRoughModel(digHeight=15,phreaticLevel=3.0,distAnch=15,firstExcavHeight=1,distLastAnchExc=15,unifLoad=100,distLoadWall=0)
vCalcDW2=[diafWall2.getEffectDigHeight(),diafWall2.getFffectDistAnch(),diafWall2.getWallEmbedmentDepth(),diafWall2.getWallThickness()]
vRefDW2=[20,17,5,1.0]


diafWall3=pdw.DiaphragmWallRoughModel(digHeight=9,phreaticLevel=2,distAnch=3,firstExcavHeight=1,distLastAnchExc=0,unifLoad=60,distLoadWall=0)
vCalcDW3=[diafWall3.getEffectDigHeight(),diafWall3.getFffectDistAnch(),diafWall3.getWallEmbedmentDepth(),diafWall3.getWallThickness()]
vRefDW3=[12,3,3,0.45]


diafWall4=pdw.DiaphragmWallRoughModel(digHeight=19,phreaticLevel=-2,distAnch=8,firstExcavHeight=1,distLastAnchExc=0,unifLoad=(10*5+100*(0.75*19-5))/(0.75*19),distLoadWall=0)
vCalcDW4=[diafWall4.getEffectDigHeight(),diafWall4.getFffectDistAnch(),diafWall4.getWallEmbedmentDepth(),diafWall4.getWallThickness()]
vRefDW4=[22.42105,8,6.33333,1.0]

ratio1=(((vCalcDW1[0]-vRefDW1[0])**2+(vCalcDW1[1]-vRefDW1[1])**2+(vCalcDW1[1]-vRefDW1[1])**2+(vCalcDW1[2]-vRefDW1[2])**2)/4.0)**0.5
ratio2=(((vCalcDW2[0]-vRefDW2[0])**2+(vCalcDW2[1]-vRefDW2[1])**2+(vCalcDW2[1]-vRefDW2[1])**2+(vCalcDW2[2]-vRefDW2[2])**2)/4.0)**0.5
ratio3=(((vCalcDW3[0]-vRefDW3[0])**2+(vCalcDW3[1]-vRefDW3[1])**2+(vCalcDW3[1]-vRefDW3[1])**2+(vCalcDW3[2]-vRefDW3[2])**2)/4.0)**0.5
ratio4=(((vCalcDW4[0]-vRefDW4[0])**2+(vCalcDW4[1]-vRefDW4[1])**2+(vCalcDW4[1]-vRefDW4[1])**2+(vCalcDW4[2]-vRefDW4[2])**2)/4.0)**0.5

# print 'ratio1= ', ratio1
# print 'ratio2= ', ratio2
# print 'ratio3= ', ratio3
# print 'ratio4= ', ratio4

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-15 and abs(ratio2)<1e-15 and abs(ratio3)<1e-15 and abs(ratio4)<1e-5:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


