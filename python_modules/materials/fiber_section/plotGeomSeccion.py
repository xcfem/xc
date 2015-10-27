# -*- coding: utf-8 -*-

import math
import cairo
import xc_base
import geom
from geom_utils import auxCairoPlot

import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches

def plotArmaduras(armaduras, ctx):
  '''draw section rebars in a postcript file.'''
  for reinfLayer in armaduras:
    barras= reinfLayer.getReinfBars
    for b in barras:
      ptPlot= b.getPos2d
      rPlot= b.diameter/2.0
      labelPlot= str(int(round(b.diameter*1e3)))
      ctx.move_to(ptPlot.x+rPlot,ptPlot.y)
      ctx.arc(ptPlot.x,ptPlot.y,rPlot,0.0,2*math.pi)
      ctx.stroke_preserve()
      ctx.fill()
      ctx.move_to(ptPlot.x+1.3*rPlot,ptPlot.y)
      ctx.text_path(labelPlot)
      ctx.stroke()

def plotGeomSeccion(geomSection, path):
  ''' draws section geometry in a postscript file'''
  WIDTH, HEIGHT = 256, 256
  surface = cairo.PSSurface(path, WIDTH, HEIGHT)
  ctx = cairo.Context(surface)
  regiones= geomSection.getRegions
  bnd= regiones.getBnd
  trf= auxCairoPlot.TransformParams(WIDTH, HEIGHT,auxCairoPlot.Boundary(bnd.getXMin,bnd.getYMin,bnd.getXMax,bnd.getYMax))
  trf.applyTransform(ctx)# Normalizing the canvas
  ctx.set_line_width(trf.scale/100000)
  ctx.set_source_rgb(0.0, 0.0, 0.0) # Black solid color
  for r in regiones:
    trf.plotPolygon(r.getPoligono(),ctx)
  ctx.stroke()
  ctx.set_line_width(trf.scale/300000)
  armaduras= geomSection.getReinfLayers
  plotArmaduras(armaduras,ctx)
  trf.plotEjesYZ(ctx)
  surface.set_eps(True)
  ctx.show_page()
  surface.finish()

class MPLGraphic(object):
  ''' Matplotlib grahic'''
  def __init__(self,title):
    self.title= title
    self.fScale= 1e-3
    self.xLabel= None
    self.yLabel= None
    self.fig= None
  def getPathPolygon(self,plg):
    nv= plg.getNumVertices()
    v0= plg.getVertice(0)
    vertices= [(v0.y*self.fScale,v0.x*self.fScale)]
    codes=[Path.MOVETO]
    for i in range(1,nv):
      v= plg.getVertice(i)
      vertices.append((v.y*self.fScale,v.x*self.fScale))
      codes.append(Path.LINETO)
    vertices.append((v0.y*self.fScale,v0.x*self.fScale))
    codes.append(Path.CLOSEPOLY)
    path = Path(vertices, codes)
    return path

  def show(self):
    plt.show()
  def savefig(self,fileName):
    self.fig.savefig(fileName)

class InteractionDiagramGraphic(MPLGraphic):
  ''' Matplotlib grahic of a interaction diagram'''
  def __init__(self,title):
    super(InteractionDiagramGraphic, self).__init__(title)
    self.xLabel= 'Mz [kN m]'
    self.yLabel= 'N [kN]'
  def setupGraphic(self,diag):
    path= self.getPathPolygon(diag)
    self.fig = plt.figure()
    ax = self.fig.add_subplot(111)
    patch = patches.PathPatch(path, facecolor='orange', lw=2)
    ax.add_patch(patch)
    ax.set_xlim(1.05*diag.getYMin*self.fScale,1.05*diag.getYMax*self.fScale)
    ax.set_ylim(1.05*diag.getXMin*self.fScale,1.05*diag.getXMax*self.fScale)
    if(self.title):
      self.fig.suptitle(self.title)
    if(self.xLabel):
      plt.xlabel(self.xLabel)
    if(self.yLabel):
      plt.ylabel(self.yLabel)
    
