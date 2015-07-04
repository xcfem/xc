#!/usr/bin/env python

import xc_base
import geom
import xc

class LoadRecord(object):
  value= 1.0
  loadCaseName= 'nil'
  loadName= 'nil'
  vDir= [0,0,-1]
  def __init__(self,lName,bName,v):
    self.loadCaseName= lName
    self.loadName= bName
    self.value= v
  def __str__(self):
    return self.loadCaseName + ' ' + self.loadName + ' ' +  str(self.value)
  

class PunctualLoadRecord(LoadRecord):
  pos= []
  def __init__(self,lName,bName,pos,v):
    super(PunctualLoadRecord,self).__init__(lName,bName,v)
    self.pos= pos
  def __str__(self):
    retval= super(PunctualLoadRecord,self).__str__()
    retval+= ' ' + str(self.pos)
    return retval
  def writeLoadOverElement(self,outFile,mdlr,elemSet):
    pos= geom.Pos3d(self.pos[0],self.pos[1],self.pos[2])
    e= elemSet.getNearestElement(pos)
    outFile.write('e= mdlr.getElementLoader.getElement('+str(e.tag)+')\n')
    vLoad= xc.Vector(self.vDir)*(self.value/e.getArea(True))
    outFile.write('e.vector3dUniformLoadGlobal(xc.Vector(['+str(vLoad[0])+','+str(vLoad[1])+','+str(vLoad[2])+']))\n')


class SurfaceLoadRecord(LoadRecord):
  mode= 'nil'
  polygon= []
  def __init__(self,lName,bName,plg,v,mode):
    super(SurfaceLoadRecord,self).__init__(lName,bName,v)
    self.mode= mode
    self.setPolygon(plg)
  def __str__(self):
    retval= super(SurfaceLoadRecord,self).__str__()
    retval+= ' ' + str(self.mode) + ' ' + str(len(self.polygon)) + ' ' + str(self.polygon)
    return retval
  def setPolygon(self,points):
    self.polygon= []
    for p in points:
      self.polygon.append(p)

class LoadContainer(object):
  name= 'nil'
  punctualLoads= []
  surfaceLoads= []
  def __init__(self,n):
    self.name= n
    self.punctualLoads= []
    self.surfaceLoads= []
  def __str__(self):
    retval= str(self.name)
    if(len(self.punctualLoads)>0):
      retval+= '\n  punctualLoads{' + str(len(self.punctualLoads)) + ' '
      for pl in self.punctualLoads:
        retval+= '<' + str(pl) + '> '
      retval+= '}'
    if(len(self.surfaceLoads)>0):
      retval+= '\n  surfaceLoads{'  + str(len(self.surfaceLoads)) + ' '
      for sl in self.surfaceLoads:
        retval+= '<' + str(sl) + '> '
      retval+= '}'
    return retval
  def getNumberOfLoads(self):
    return len(self.punctualLoads)+len(self.surfaceLoads)
  def empty(self):
    return (self.getNumberOfLoads()==0)
