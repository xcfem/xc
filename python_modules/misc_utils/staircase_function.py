# -*- coding: utf-8 -*-
#Function defined as a series of constant values in consecutive intervals:
#
#          ________V1_________               ____V4____
#          |                 |____V2_____    |        |
#  ___V0___|                 |          |_V3_|        |
#  |                         |          |    |        |
#  +-------+-----------------+----------+----+--------+
#  x0      x1                x2         x3   x4       x5

import bisect

class StairCaseFunction(object):
  '''Function defined as a series of constant values in consecutive intervals'''
  def __init__(self,xi,vi,leftDefaultValue= 0.0, rightDefaultValue= 0.0):
    self.lastX= xi[-1]
    self.xi= xi
    self.values= vi
    self.leftDefaultValue= leftDefaultValue
    self.rightDefaultValue= rightDefaultValue
    
  def __call__(self,x):
    if(x<self.xi[0]):
      return self.leftDefaultValue
    elif(x>self.xi[-1]):
      return self.rightDefaultValue
    else:
      i= bisect.bisect_left(self.xi, x)
      return self.values[i]
    
    
