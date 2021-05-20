# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom

pos1= geom.Pos2d(1,2)

j= 0
cooY= -1.0
mycode = 'global j;j= j+1;global cooY;cooY= self.y'

#exec mycode
pos1.execPy(mycode)

ratio1= j-1
ratio2= cooY-pos1.y

'''
print("j= ", j)
print("ratio1= ", ratio1)
print("y= ", pos1.y)
print("cooY= ", cooY)
print("ratio2= ", ratio2)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-15 and abs(ratio2)<1e-15:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
