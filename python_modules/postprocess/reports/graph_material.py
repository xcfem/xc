# -*- coding: utf-8 -*-
from __future__ import print_function


import numpy as np
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
#from mayavi import mlab # Some strange error messages if we do like that
#import mayavi # This way we haven't error messages, but it doesn't work.
               # Finally I've put the import line in the show method.

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
  def close(self):
    plt.close()
  def savefig(self,fileName):
    self.fig.savefig(fileName)

class InteractionDiagramGraphic(MPLGraphic):
  ''' Matplotlib grahic of a interaction diagram'''
  def __init__(self,title):
    super(InteractionDiagramGraphic, self).__init__(title)
    self.decorations.xLabel= 'Mz [kN m]'
    self.decorations.yLabel= 'N [kN]'
  def setupGraphic(self,diag, internalForces= None):
    path= self.getPathPolygon(diag)
    self.fig = plt.figure()
    ax= self.fig.add_subplot(111)
    patch= patches.PathPatch(path, facecolor='orange', lw=2)
    ax.add_patch(patch)
    if(internalForces):
      N= internalForces[0]
      My= internalForces[1]
      n= []; my= []
      for v in N:
        n.append(v*self.fScale)
      for v in My:
        my.append(v*self.fScale)
      ax.scatter(my,n, c='b', zorder= 10)
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
      print("strain= ", diag.getStrain(), " stress= ", factoredStress/1e6)
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

class InteractionDiagram3DGraphic:
  def __init__(self,intDiagram3D, axialForceScaleFactor= 0.1,title= 'Interaction diagram.', fUnitConv= 1e-3, coloredBy= 0):
    '''Creates a suitable triangle mesh suitable for displaying it
       with mayavi.'''
    self.triangleMap= intDiagram3D.getTriangleMap()
    vertices= self.triangleMap.getVertices()
    self.x= list()
    self.y= list()
    self.z= list()
    self.scalars= list()
    for v in vertices:
      pos= v.data()
      self.x.append(axialForceScaleFactor*fUnitConv*pos[0])
      self.y.append(fUnitConv*pos[1])
      self.z.append(fUnitConv*pos[2])
      self.scalars.append(fUnitConv*pos[0]) #0->N, 1->My, 2->Mz

    self.triangles= list()
    faces= self.triangleMap.getFaces()
    for f in faces:
      self.triangles.append((f.getV1(), f.getV2(), f.getV3()))
    self.title= title
    self.axialForceLabel= 'N x '+str(axialForceScaleFactor)+' (kN)'
    self.bendingMomentYLabel= 'My (kN m)' 
    self.bendingMomentZLabel= 'Mz (kN m)' 
  def show(self):
    '''Show the 3D diagram in the screen.''' 
    from mayavi import mlab
    self.triangleMesh= mlab.triangular_mesh(self.x, self.y, self.z, self.triangles, scalars= self.scalars)
    mlab.colorbar(self.triangleMesh, orientation='vertical')
    mlab.outline(self.triangleMesh)
    mlab.axes(self.triangleMesh, xlabel= self.axialForceLabel, ylabel= self.bendingMomentYLabel, zlabel= self.bendingMomentZLabel)
    #mlab.title(self.title)
    mlab.show()
