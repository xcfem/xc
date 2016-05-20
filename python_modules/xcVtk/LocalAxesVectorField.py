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
from xcVtk import VectorField as vf


class LocalAxesVectorField(vf.VectorField):
  '''Draws a the local axes on elements.'''
  def __init__(self,name,scaleFactor= 1.0):
    '''
    Parameters:
      scaleFactor: scale factor for the size of the vectors.
    '''
    super(LocalAxesVectorField,self).__init__(name,1.0,scaleFactor,False)

  def dumpLocalAxes(self,xcSet):
    ''' Iterate over loaded elements dumping its axes into the graphic.'''
    elemSet= xcSet.getElements
    for e in elemSet:
      p= e.getPosCentroid(True)
      axes= e.getLocalAxes(True)
      vx= axes(0,0); vy= axes(0,1); vz= axes(0,2)
      self.insertNextPair(p.x,p.y,p.z,vx,vy,vz)
      vx= axes(1,0); vy= axes(1,1); vz= axes(1,2)
      self.insertNextPair(p.x,p.y,p.z,vx,vy,vz)
      vx= axes(2,0); vy= axes(2,1); vz= axes(2,2)
      self.insertNextPair(p.x,p.y,p.z,vx,vy,vz)

