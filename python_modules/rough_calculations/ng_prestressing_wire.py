# -*- coding: utf-8 -*-

import xc_base
import geom
from geom_utils import parabola as pb
import math
from scipy import integrate


class prestressingWire(object):
  trace= None
  x0= 0.0
  xL= 1.0
  y= 0.0
  def __init__(self,p0,p1,p2,x0,xL,y):
    self.trace= pb.Parabola(p0,p1,p2)
    self.x0= x0
    self.xL= xL
    self.y= y
  def z(self,x):
    return self.trace.y(x)
  def getL(self):
    return self.xL-self.x0
  def alpha(self,x):
    return self.trace.alpha(x)
  def curvature(self,x):
    return self.trace.curvature(x)
  def getPoint(self,x):
    return geom.Pos3d(x,self.y,self.z(x))
  def getForce(self,x,P):
    alpha= self.alpha(x)
    return P*geom.Vector3d(math.cos(alpha),0.0,math.sin(alpha))
  def getU(self,x,P):
    alpha= self.alpha(x)
    r= self.curvature(x)
    return P*r
  def getIntegratedU(self,P):
    numInt= 10
    x= []
    y= []
    L= self.getL()
    for i in range(0,numInt+1):
      v= float(i)/float(numInt)*L
      x.append(v)
      y.append(self.getU(v,P))
    return integrate.simps(y,x)

