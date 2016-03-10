# -*- coding: utf-8 -*-

import math
import cairo
import xc_base
import geom
from geom_utils import auxCairoPlot

import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches

def plotArmaduras(reinforcement, ctx):
  '''draw section rebars in a postcript file.'''
  for reinfLayer in reinforcement:
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
  reinforcement= geomSection.getReinfLayers
  plotArmaduras(reinforcement,ctx)
  trf.plotEjesYZ(ctx)
  surface.set_eps(True)
  ctx.show_page()
  surface.finish()

