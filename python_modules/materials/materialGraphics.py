# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches

class MPLGraphicDecorations(object):
  ''' Matplotlib graphic decorations (title, labels,...)'''
  def __init__(self,title):
    self.title= title
    self.xLabel= None
    self.yLabel= None
    self.grid= True
  def setLabels(self,plt):
    if(self.xLabel):
      plt.xlabel(self.xLabel)
    if(self.yLabel):
      plt.ylabel(self.yLabel)
  def setGrid(self,plt):
    plt.grid(self.grid)
  def setUp(self,plt):
    self.setLabels(plt)
    self.setGrid(plt)

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
    self.decorations.setUp(plt)
    
class UniaxialMaterialDiagramGraphic:
  def __init__(self,epsMin,epsMax,title):
    self.decorations= MPLGraphicDecorations(title)
    self.decorations.xLabel= 'strain'
    self.decorations.yLabel= 'stress [MPa]'
    self.epsMin= epsMin
    self.epsMax= epsMax
    self.incEps= (epsMax-epsMin)/100.0
    self.stressScaleFactor= 1.0/1e6
  def getStrains(self):
    '''Abcissae for the diagram '''
    retval= np.arange(self.epsMin,self.epsMax,self.incEps)
    #retval.extend(np.arange(self.epsMax,self.epsMin,-self.incEps))
    #retval.extend(np.arange(self.epsMin,self.epsMax,self.incEps))
    return retval
  def getStresses(self,diag):
    self.factoredStresses= []
    self.strainMin= 1e9
    self.strainMax= -self.strainMin
    self.factoredStressMin= 1e9
    self.factoredStressMax= -self.factoredStressMin
    self.strains= self.getStrains()
    for eps in self.strains:
      diag.setTrialStrain(eps, 0.0)
      #diag.commitState()
      self.strainMin= min(eps,self.strainMin)
      self.strainMax= max(eps,self.strainMax)
      factoredStress= diag.getStress()*self.stressScaleFactor
      self.factoredStressMin= min(factoredStress,self.factoredStressMin)
      self.factoredStressMax= max(factoredStress,self.factoredStressMax)
      self.factoredStresses.append(factoredStress)
      #print "strain= ", diag.getStrain(), " stress= ", factoredStress/1e6
    diag.revertToStart()
    return self.factoredStresses
  def setupAxis(self,plt):
    plt.axis([1.05*self.strainMin, 1.05*self.strainMax, 1.05*self.factoredStressMin, 1.05*self.factoredStressMax])
  def setupGraphic(self,plt,materialDiagram):
    factoredStresses= self.getStresses(materialDiagram)
    plt.plot(self.strains, self.factoredStresses)
    self.setupAxis(plt)
    if(self.decorations.title):
      plt.title(self.decorations.title)
    self.decorations.setUp(plt)
  def show(self,plt):
    plt.show()
  def savefig(self,plt,fileName):
    plt.savefig(fileName)
