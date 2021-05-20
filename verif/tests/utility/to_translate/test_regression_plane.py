# -*- coding: utf-8 -*-

from __future__ import print_function
import regression_plane

points = [(1.1,2.1,8.1),
          (3.2,4.2,8.0),
          (5.3,1.3,8.2),
          (3.4,2.4,8.3),
          (1.5,4.5,8.0)]

result= regression_plane.getRegressionPlane(points)

print("A,B,C,D= ", result)

errMax= 0.0
for (x,y,z) in points:
  err= result[0]*x+result[1]*y+result[2]*z+result[3]
  errMax= max(abs(err),errMax)

print("err maximale: ", errMax)
