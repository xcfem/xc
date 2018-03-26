# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

import vtk
from miscUtils import LogMessages as lmsg

def VtkCreaStrArraySetData(setToDraw, entTypeName, attr):
  '''Creates an array of strings with information associated 
  to the points and cells.
  Parameters:
    setToDraw:   set of entities to be displayed
    entTypeName: type of entities to be displayed 
                 ("pnts", "lines", "nodes", "elementos")
    attr:        attribute to be stored in the array
  '''
  # Definimos la matriz
  arr= vtk.vtkStringArray()
  arr.SetName(attr)
  container= None
  if(entTypeName=="pnts"):
    container= setToDraw.getPoints
  elif(entTypeName=="lines"):
    container= setToDraw.getLines
  elif(entTypeName=="nodes"):
    container= setToDraw.getNodes
  elif(entTypeName=="elementos"):
    container= setToDraw.getElements
  else:
    lmsg.error("error: "+entTypeName+" not implemented.")
  for e in container:
    tmp= str(getattr(e,attr))
    arr.InsertValue(e.getIdx,tmp)
  return arr
