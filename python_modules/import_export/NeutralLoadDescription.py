#!/usr/bin/env python

import xc_base
import geom
import xc

class LoadRecord(object):
  def __init__(self,lName= 'nil',bName= 'nil',v= 1.0):
    self.loadCaseName= lName
    self.loadName= bName
    self.value= v
    self.vDir= [0,0,-1]
  def __str__(self):
    return self.loadCaseName + ' ' + self.loadName + ' ' +  str(self.value)
  

class PunctualLoadRecord(LoadRecord):
  tag= -1 #Element or node tag.
  def __init__(self,lName,bName,pos,v):
    super(PunctualLoadRecord,self).__init__(lName,bName,v)
    self.pos= pos
  def __str__(self):
    retval= super(PunctualLoadRecord,self).__str__()
    retval+= ' ' + str(self.pos)
    return retval
  def searchLoadedElement(self,elemSet):
    pos= geom.Pos3d(self.pos[0],self.pos[1],self.pos[2])
    e= elemSet.getNearestElement(pos)
    self.tag= e.tag
  def searchLoadedNode(self,elemSet):
    pos= geom.Pos3d(self.pos[0],self.pos[1],self.pos[2])
    n= elemSet.getNearestNode(pos)
    self.tag= n.tag

class SurfaceLoadRecord(LoadRecord):
  def __init__(self,lName= 'nil', bName= 'nil', plg= [],v= 1.0,mode= 'nil'):
    super(SurfaceLoadRecord,self).__init__(lName,bName,v)
    self.mode= mode
    self.setPolygon(plg)
    self.tags= [] #Element or node tags
    self.projPlane= "xy"
  def __str__(self):
    retval= super(SurfaceLoadRecord,self).__str__()
    retval+= ' ' + str(self.mode) + ' ' + str(len(self.polygon)) + ' ' + str(self.polygon)
    return retval
  def get2DPolygon(self):
    retval= geom.Poligono2d()
    for p in self.polygon:
      coords= [p[0],p[1]]
      if(self.projPlane=='xz'):
        coords[1]= p[2]
      elif(self.projPlane=='yz'):
        coords=  [p[1],p[2]]
      retval.agregaVertice(geom.Pos2d(coords[0],coords[1]))
    return retval
  def searchLoadedElements(self,elemSet):
    ''' Returns element which have his center inside the polygon'''
    plg= self.get2DPolygon()
    self.tags= []
    for e in elemSet.getElements:
      pos= e.getPosCentroid(True)
      projPos= geom.Pos2d(pos.x,pos.y)
      if(self.projPlane=='xz'):
        projPos= geom.Pos2d(pos.x,pos.z)
      elif(self.projPlane=='yz'):
        projPos= geom.Pos2d(pos.y,pos.z)
      if plg.In(projPos,0.01):
        self.tags.append(e.tag)
    return self.tags
  def setPolygon(self,points):
    self.polygon= []
    for p in points:
      self.polygon.append(p)

class LoadContainer(object):
  setName= 'total'
  def __init__(self,n):
    self.name= n
    self.punctualLoads= []
    self.surfaceLoads= []
    self.elementSet= 'total'
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
  def searchLoadedElements(self,elementSet):
    '''Get load distribution over elements taken from the set.'''
    if(len(self.punctualLoads)>0):
      for pl in self.punctualLoads:
        pl.searchLoadedElement(elementSet)
    if(len(self.surfaceLoads)>0):
      for sl in self.surfaceLoads:
        sl.searchLoadedElements(elementSet)
    
class LoadGroup(object):
  ''' Loads wich share some property (origin,...).'''
  def __init__(self,id, desc):
    self.id= id
    self.desc= desc #Group description.

class LoadCase(object):
  ''' Load case.'''
  def __init__(self,id, desc, lg, ltyp, loads= LoadContainer('')):
    self.id= id
    self.desc= desc #Load description.
    self.loadGroupId= lg
    self.ltyp= ltyp
    self.loads= loads
  

class LoadData(object):
  def __init__(self):
    self.loadGroups= {}
    self.loadCases= {}
