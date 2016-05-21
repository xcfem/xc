# -*- coding: utf-8 -*-

''' Loads represented as vectors. '''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import vtk
from miscUtils import LogMessages as lmsg
from xcVtk import DirectionFieldData as dfd


class LocalAxesVectorField(object):
  '''Draws a the local axes on elements.'''
  xColor= [1.0,0.0,0.0] # red
  yColor= [0.0,1.0,0.0] # green
  zColor= [0.0,0.0,1.0] # blue
  def __init__(self,name,scaleFactor= 1.0):
    '''
    Parameters:
      scaleFactor: scale factor for the size of the vectors.
    '''
    super(LocalAxesVectorField,self).__init__()
    self.xAxes= dfd.DirectionFieldData(name+'X',self.xColor,3,scaleFactor)
    self.yAxes= dfd.DirectionFieldData(name+'Y',self.yColor,3,scaleFactor)
    self.zAxes= dfd.DirectionFieldData(name+'Z',self.zColor,3,scaleFactor)


  def dumpLocalAxes(self,xcSet):
    ''' Iterate over loaded elements dumping its axes into the graphic.'''
    elemSet= xcSet.getElements
    for e in elemSet:
      p= e.getPosCentroid(True)
      axes= e.getLocalAxes(True)
      vx= axes(0,0); vy= axes(0,1); vz= axes(0,2)
      self.xAxes.insertNextPair(p.x,p.y,p.z,vx,vy,vz)
      vx= axes(1,0); vy= axes(1,1); vz= axes(1,2)
      self.yAxes.insertNextPair(p.x,p.y,p.z,vx,vy,vz)
      vx= axes(2,0); vy= axes(2,1); vz= axes(2,2)
      self.zAxes.insertNextPair(p.x,p.y,p.z,vx,vy,vz)

  def addToDisplay(self,recordDisplay):
    self.xAxes.addToDisplay(recordDisplay)
    self.yAxes.addToDisplay(recordDisplay)
    self.zAxes.addToDisplay(recordDisplay)
