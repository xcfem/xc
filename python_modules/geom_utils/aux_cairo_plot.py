# -*- coding: utf-8 -*-

from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2017 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
import cairo
import geom


class ArrowHead(object):
  def __init__(self,style,lineColor):
    self.style= style
    self.length= 15
    self.degrees= 0.25
    self.lineColor= lineColor
  def calcVertices(self, start, end):
    angle= math.atan2(end.y - start.y, end.x - start.x) + math.pi
    x1 = end.x + self.length * math.cos(angle - self.degrees)
    y1 = end.y + self.length * math.sin(angle - self.degrees)
    x2 = end.x + self.length * math.cos(angle + self.degrees)
    y2 = end.y + self.length * math.sin(angle + self.degrees)
    return [[x1,y1],[x2,y2]]

class ArrowOpen(ArrowHead):
  def __init__(self,lineColor):
    super(ArrowOpen,self).__init__('open',lineColor)
  def draw(self,ctx,start,end):
    vertex= self.calcVertices(start, end)

    ctx.set_source_rgb(self.lineColor[0],self.lineColor[1],self.lineColor[2])
    ctx.move_to(end.x, end.y)
    ctx.line_to(vertex[0][0], vertex[0][1])
    ctx.stroke()

    ctx.move_to(end.x, end.y)
    ctx.line_to(vertex[1][0], vertex[1][1])
    ctx.stroke()

    ctx.move_to(start.x,start.y)
    ctx.line_to(end.x,end.y)
    ctx.stroke()

class ArrowSolidBase(ArrowHead):
  def __init__(self,lineColor,fillColor):
    super(ArrowSolidBase,self).__init__('solid',lineColor)
    self.fillColor= fillColor


class ArrowSolid(ArrowSolidBase):
  def __init__(self,lineColor,fillColor):
    super(ArrowSolid,self).__init__('solid',lineColor,fillColor)
  def draw(self,ctx,start,end):
    vertices= self.calcVertices(start, end)
    x1= vertices[0][0]
    y1= vertices[0][1]
    x2= vertices[1][0]
    y2= vertices[1][1]
    ctx.move_to (end.x, end.y)
    ctx.line_to (x1, y1)
    ctx.line_to (x2, y2)
    ctx.close_path()

    ctx.set_source_rgb(self.lineColor[0],self.lineColor[1],self.lineColor[2])
    ctx.stroke_preserve()

    ctx.set_source_rgb(self.fillColor[0],self.fillColor[1],self.fillColor[2])
    ctx.fill()

class ArrowDiamond(ArrowSolidBase):
  def __init__(self,lineColor,fillColor):
    super(ArrowDiamond,self).__init__('solid',lineColor,fillColor)
  def draw(self,ctx,start,end):
    vertices= self.calcVertices(start, end)
    x1= vertices[0][0]
    y1= vertices[0][1]
    x2= vertices[1][0]
    y2= vertices[1][1]
    x3= end.x + self.length * 2 * math.cos(angle)
    y3= end.y + self.length * 2 * math.sin(angle)

    ctx.move_to (end.x, end.y)
    ctx.line_to (x1, y1)
    ctx.line_to (x3, y3)
    ctx.line_to (x2, y2)
    ctx.close_path()

    ctx.set_source_rgb(self.lineColor[0],self.lineColor[1],self.lineColor[2])
    ctx.stroke_preserve()

    ctx.set_source_rgb(self.fillColor[0],self.fillColor[1],self.fillColor[2])
    ctx.fill()

class ArrowCircle(ArrowSolidBase):
  def __init__(self,lineColor,fillColor):
    super(ArrowCircle,self).__init__('solid',lineColor,fillColor)
  def draw(self,ctx,start,end):
    xc = end.x + self.length * math.cos(self.angle)
    yc = end.y + self.length * math.sin(self.angle)

    ctx.arc(xc, yc, self.length, 0.0, 2 * math.pi)
    ctx.set_source_rgb(self.lineColor[0],self.lineColor[1],self.lineColor[2])
    ctx.stroke_preserve()
    ctx.set_source_rgb(self.fillColor[0],self.fillColor[1],self.fillColor[2])
    ctx.fill()

class Boundary(object):
  def __init__(self,xmin,ymin,xmax,ymax):
    self.xmin= xmin
    self.xmax= xmax
    self.ymin= ymin
    self.ymax= ymax
  def getXCenter(self):
    return (self.xmax+self.xmin)/2.0
  def getYCenter(self):
    return (self.ymax+self.ymin)/2.0
  def getXWidth(self):
    return self.xmax-self.xmin
  def getYWidth(self):
    return self.ymax-self.ymin

class TransformParams(object):
  def __init__(self,w,h,bnd):
    self.w= w #User space width.
    self.h= h #User space height.
    self.bnd= bnd
    self.x0= self.bnd.getXCenter()
    self.y0= self.bnd.getYCenter()
    eX= self.w/self.bnd.getXWidth()
    eY= self.h/self.bnd.getYWidth()
    self.scale= min(eX,eY)
  def applyTransform(self,ctx):
    ctx.translate(self.x0+self.w/2.0, self.y0+self.h/2)
    ctx.scale(self.scale, self.scale)
    ctx.set_font_size(self.scale/10000)

  def plotPolygon(self, plg,ctx):
    ''' Draws polygon countour'''
    nv= plg.getNumVertices()
    ptPlot= plg.getVertice(0)
    ctx.move_to(ptPlot.x,ptPlot.y)
    for i in range(1,nv):
      ptPlot= plg.getVertice(i)
      ctx.line_to(ptPlot.x,ptPlot.y)
    ptPlot= plg.getVertice(0)
    ctx.line_to(ptPlot.x,ptPlot.y)
  def plotYZAxis(self,ctx):
    '''Draws Y and Z axis.'''
    long= self.scale/5000
    xArrow= ArrowOpen([0,1,0])# Green color
    xArrow.length= long/2
    yArrow= ArrowOpen([0,0,1])# Blue color
    yArrow.length= long/2
    ctx.set_line_width(long/40)
    xArrow.draw(ctx,geom.Pos2d(0,0),geom.Pos2d(long,0))
    ctx.set_source_rgb(0, 1.0, 0) # Green color
    ctx.move_to(long,0)
    ctx.text_path("Y")
    ctx.stroke()
    yArrow.draw(ctx,geom.Pos2d(0,0),geom.Pos2d(0,long))
    ctx.set_source_rgb(0, 0, 1.0) # Blue color
    ctx.move_to(long/10,long)
    ctx.text_path("Z")
    ctx.stroke()

