# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches

class MPLGraphicDecorations(object):
  ''' Matplotlib graphic decorations (title, labels,...)'''
  def __init__(self,title):
    self.title= title
    self.xLabel= None
    self.yLabel= None
  def setLabels(self,plt):
    if(self.xLabel):
      plt.xlabel(self.xLabel)
    if(self.yLabel):
      plt.ylabel(self.yLabel)    

class MPLGraphic(object):
  ''' Matplotlib grahic'''
  def __init__(self,title):
    self.decorations= MPLGraphicDecorations(title)
    self.fScale= 1e-3
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
    self.decorations.xLabel= 'Mz [kN m]'
    self.decorations.yLabel= 'N [kN]'
  def setupGraphic(self,diag):
    path= self.getPathPolygon(diag)
    self.fig = plt.figure()
    ax = self.fig.add_subplot(111)
    patch = patches.PathPatch(path, facecolor='orange', lw=2)
    ax.add_patch(patch)
    ax.set_xlim(1.05*diag.getYMin*self.fScale,1.05*diag.getYMax*self.fScale)
    ax.set_ylim(1.05*diag.getXMin*self.fScale,1.05*diag.getXMax*self.fScale)
    if(self.decorations.title):
      self.fig.suptitle(self.decorations.title)
    self.decorations.setLabels(plt)
    
