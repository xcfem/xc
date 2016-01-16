# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.Ortega.Ort@gmail.com"


def getIMaxPropFiber(fibers,methodName):
  '''returns the fiber from a set of fibers where the maximum value of a 
  certain property is reached
  Parameters:
    fibers:     set of fibers
    methodName: name of the method that returns the fiber property searched
  '''
  retval= 0
  sz= len(fibers)
  f= fibers[0]
  methodToCall= getattr(f, methodName)
  maxValue= methodToCall()
  for i in range(1,sz):
    f= fibers[i]
    methodToCall= getattr(f, methodName)
    value= methodToCall()
    if(value>maxValue):
      maxValue= value
      retval= i
  return retval

def getIMinPropFiber(fibers,methodName):
  '''returns the fiber from a set of fibers where the minimum value of a 
  certain property is reached
  Parameters:
    fibers:     set of fibers
    methodName: name of the method that returns the fiber property searched
  '''
  retval= 0
  sz= len(fibers)
  f= fibers[0]
  methodToCall= getattr(f, methodName)
  minValue= methodToCall()
  for i in range(1,sz):
    f= fibers[i]
    methodToCall= getattr(f, methodName)
    value= methodToCall()
    if(value>minValue):
      minValue= value
      retval= i
  return retval


