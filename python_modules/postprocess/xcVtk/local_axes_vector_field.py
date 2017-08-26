# -*- coding: utf-8 -*-

''' Local axes represented as vectors. '''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import vtk
from miscUtils import LogMessages as lmsg
from postprocess.xcVtk import direction_field_data as dfd

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


  def dumpVectors(self,xcSet):
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

class StrongWeakAxisVectorField(object):
  '''Draws a the strong axis on elements.'''
  strongColor= [1.0,0.0,0.0] # red
  weakColor= [0.0,0.0,1.0] # blue
  def __init__(self,name,scaleFactor= 1.0):
    '''
    Parameters:
      scaleFactor: scale factor for the size of the vectors.
    '''
    super(StrongWeakAxisVectorField,self).__init__()
    self.strongAxis= dfd.DirectionFieldData(name+'Strong',self.strongColor,3,scaleFactor*1.2)
    self.weakAxis= dfd.DirectionFieldData(name+'Weak',self.weakColor,3,scaleFactor*0.8)


  def dumpVectors(self,xcSet):
    ''' Iterate over loaded elements dumping its axes into the graphic.'''
    elemSet= xcSet.getElements
    for e in elemSet:
      p= e.getPosCentroid(True)
      strongAxis= e.getVDirStrongAxisGlobalCoord(True) # initialGeometry= True
      vx= strongAxis[0]; vy= strongAxis[1]; vz= strongAxis[2]
      self.strongAxis.insertNextPair(p.x,p.y,p.z,vx,vy,vz)
      weakAxis= e.getVDirWeakAxisGlobalCoord(True) # initialGeometry= True
      vx= weakAxis[0]; vy= weakAxis[1]; vz= weakAxis[2]
      self.weakAxis.insertNextPair(p.x,p.y,p.z,vx,vy,vz)

  def addToDisplay(self,recordDisplay):
    self.strongAxis.addToDisplay(recordDisplay)
    self.weakAxis.addToDisplay(recordDisplay)

