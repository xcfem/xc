# -*- coding: utf-8 -*-
import scipy.interpolate
import math
from geom_utils import acad_script_utils as script
import os
from geom_utils import longitudinal_profile
import xc_base
import geom

def filterDuplicatePoints(points):
  retval= []
  p0= points[0]
  retval.append(p0)
  for p in points:
    dist= longitudinal_profile.getDistXYZ(p,p0) #Distance to the previous point.
    if(dist>=1e-12):
      retval.append(p)
      p0= p
  return retval  

class XYParametrized3DPoly:
  s= []
  xfn= None
  yfn= None
  zfn= None
  def __init__(self,points):
    pline3D= filterDuplicatePoints(points)
    p0= pline3D[0]
    self.s= []
    x= []
    y= []
    z= []
    distOrg= 0
    for p in pline3D:
      dist= longitudinal_profile.getDistXY(p0,p) #Distance in the XY plane.
      distOrg+= dist
      x.append(p.x)
      y.append(p.y)
      z.append(p.z)
      self.s.append(distOrg)
      p0= p
    self.xfn= scipy.interpolate.interp1d(self.s,x)
    self.yfn= scipy.interpolate.interp1d(self.s,y)
    self.zfn= scipy.interpolate.interp1d(self.s,z)
  def __len__(self):
    return min(len(self.xData),len(self.yData),len(self.zData))
  def x(self,s):
    return float(self.xfn(s))
  def y(self,s):
    return float(self.yfn(s))
  def z(self,s):
    return float(self.zfn(s))
  def getLength(self):
    return self.s[-1]-self.s[0]
  def getPoint(self,s):
    return (self.x(s),self.y(s),self.z(s))
  def getPos3d(self,s):
    return geom.Pos3d(self.x(s),self.y(s),self.z(s))
  def getPointsEveryXmetres(self,spacement):
    sMax= self.s[-1]
    retval= []
    s= 0.0
    while(s<sMax):
      p= self.getPoint(s)
      retval.append(p)
      s+=spacement
    p= self.getPoint(sMax)
    retval.append(p)
    return retval
  def getPointsAt(self,factors):
    np= len(factors)
    L= self.getLength()
    retval= []
    for i in range(0,np):
      f= factors[i]
      s= f*L
      p= self.getPoint(s)
      retval.append(p)
    return retval


