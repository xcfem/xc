# -*- coding: utf-8 -*-

import datetime
import vtk
from miscUtils import LogMessages as lmsg
import xc_base
import geom
import xc

class ScreenAnnotation(object):
  lowerLeft= 0 #Lower left corner
  upperLeft= 2 #Upper left corner
  upperRight= 3 #Upper right corner
  
  def __init__(self):
    self.version= 'XC finite element analysis\n'+ xc.getXCVersion()

  def getVtkCornerAnnotation(self,caption= ''):
    self.annotation = vtk.vtkCornerAnnotation()
    self.date= datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    self.annotation.SetText(self.upperRight, self.date) 
    self.annotation.SetText(self.upperLeft, self.version)
    self.annotation.SetText(self.lowerLeft,caption)
    return self.annotation
