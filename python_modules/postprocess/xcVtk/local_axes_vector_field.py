# -*- coding: utf-8 -*-

from __future__ import print_function

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
  '''Draws the local axes on elements.'''
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

  def dumpPair(self,centroid,axes):
    ''' Dump the pair into the xAxes, yAxes and zAxes objet.'''
    vx= axes(0,0); vy= axes(0,1); vz= axes(0,2)
    self.xAxes.insertNextPair(centroid.x,centroid.y,centroid.z,vx,vy,vz)
    self.xAxes.insertNextVector(vx,vy,vz)
    vx= axes(1,0); vy= axes(1,1); vz= axes(1,2)
    self.yAxes.insertNextPair(centroid.x,centroid.y,centroid.z,vx,vy,vz)
    self.yAxes.insertNextVector(vx,vy,vz)
    vx= axes(2,0); vy= axes(2,1); vz= axes(2,2)
    self.zAxes.insertNextPair(centroid.x,centroid.y,centroid.z,vx,vy,vz)
    self.zAxes.insertNextVector(vx,vy,vz)
    
    
  def dumpVectors(self,xcSet):
    ''' Iterate over the elements dumping its axes into the graphic.'''
    elemSet= xcSet.elements
    for e in elemSet:
      self.dumpPair(e.getPosCentroid(True),e.getLocalAxes(True))

  def addToDisplay(self,recordDisplay):
    if(self.xAxes.getNumberOfTuples()>0):
      self.xAxes.addToDisplay(recordDisplay)
    if(self.yAxes.getNumberOfTuples()>0):
      self.yAxes.addToDisplay(recordDisplay)
    if(self.yAxes.getNumberOfTuples()>0):
      self.zAxes.addToDisplay(recordDisplay)

class QuadSurfacesLocalAxesVectorField(LocalAxesVectorField):
  '''Draws the local axes on quadrilateral surfaces.'''
  def dumpVectors(self,xcSet):
    ''' Iterate over the surfaces dumping its axes into the graphic.'''
    surfaceSet= xcSet.getSurfaces
    for s in surfaceSet:
      self.dumpPair(s.getPosCentroid(),s.getLocalAxes())

    
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
    self.strongAxis= dfd.DirectionFieldData(name+'Strong',self.strongColor,3,scaleFactor)
    self.weakAxis= dfd.DirectionFieldData(name+'Weak',self.weakColor,3,scaleFactor)


  def dumpPair(self,element):
    ''' dump weak and strong axis into the graphic.'''
    p= element.getPosCentroid(True)
    strongAxis= element.getVDirStrongAxisGlobalCoord(True) # initialGeometry= True
    vx= 1.2*strongAxis[0]; vy= 1.2*strongAxis[1]; vz= 1.2*strongAxis[2]
    self.strongAxis.insertNextPair(p.x,p.y,p.z,vx,vy,vz)
    self.strongAxis.insertNextVector(vx,vy,vz)
    weakAxis= element.getVDirWeakAxisGlobalCoord(True) # initialGeometry= True
    vx= 0.8*weakAxis[0]; vy= 0.8*weakAxis[1]; vz= 0.8*weakAxis[2]
    self.weakAxis.insertNextPair(p.x,p.y,p.z,vx,vy,vz)
    self.weakAxis.insertNextVector(vx,vy,vz)

  def dumpVectors(self,xcSet):
    ''' Iterate over the elements dumping its axes into the graphic.'''
    elemSet= xcSet.elements
    for e in elemSet:
      self.dumpPair(e)

  def addToDisplay(self,recordDisplay):
    if(self.strongAxis.getNumberOfTuples()>0):
      self.strongAxis.addToDisplay(recordDisplay)
    if(self.weakAxis.getNumberOfTuples()>0):
      self.weakAxis.addToDisplay(recordDisplay)



