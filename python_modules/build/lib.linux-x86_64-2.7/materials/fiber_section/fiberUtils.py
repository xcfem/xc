# -*- coding: utf-8 -*-
# Definición de un cuadrado.

def getIMaxPropFiber(fibers,methodName):
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


