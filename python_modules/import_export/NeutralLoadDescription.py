# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import copy

class LoadRecord(object):
  def __init__(self, loadCase,bName= 'nil',v= 1.0):
    self.loadCaseId= loadCase.id
    self.loadCaseName= loadCase.name
    self.loadName= bName
    self.value= v
    self.vDir= [0,0,-1]
  def __str__(self):
    return 'id= ' + str(self.loadCaseId) + ' name= ' + self.loadCaseName + ' loadName= ' + str(self.loadName) + ' value= ' +  str(self.value) + ' dir= ' + str(self.vDir)
  
class PointForceRecord(LoadRecord):
  '''Force applied in a point.'''
  def __init__(self, loadCase,bName,pos,v):
    super(PointForceRecord,self).__init__(loadCase, bName, v)
    self.pos= pos
  def __str__(self):
    retval= super(PointForceRecord,self).__str__()
    if(self.pos): retval+= ' ' + str(self.pos)
    return retval

class NodalLoadRecord(PointForceRecord):
  '''Load over a node.'''
  def __init__(self, loadCase,bName,pos,v):
    super(NodalLoadRecord,self).__init__(loadCase, bName, pos, v)
    self.tag= -1 #node tag.
  def __str__(self):
    retval= 'tag= ' + str(self.tag)
    retval+= ' ' + super(NodalLoadRecord,self).__str__()
    return retval
  def searchLoadedElement(self,elemSet):
    pos= geom.Pos3d(self.pos[0],self.pos[1],self.pos[2])
    e= elemSet.getNearestElement(pos)
    self.tag= e.tag
  def searchLoadedNode(self,elemSet):
    pos= geom.Pos3d(self.pos[0],self.pos[1],self.pos[2])
    n= elemSet.getNearestNode(pos)
    self.tag= n.tag

class ElementLoadRecord(LoadRecord):
  def __init__(self, loadCase, bName= 'nil', v= 1.0,mode= 'nil'):
    super(ElementLoadRecord,self).__init__(loadCase,bName,v)
    self.mode= mode
    self.tags= list() #Element tags
  def __str__(self):
    retval= super(ElementLoadRecord,self).__str__()
    retval+= ' ' + str(self.mode) + ' ' + str(self.tags)
    return retval


class SurfaceLoadRecord(ElementLoadRecord):
  def __init__(self, loadCase, bName= 'nil', plg= None,v= 1.0,mode= 'nil'):
    super(SurfaceLoadRecord,self).__init__(loadCase,bName,v)
    if(plg):
      self.setPolygon(plg)
    self.projPlane= "xy"
  def __str__(self):
    retval= super(SurfaceLoadRecord,self).__str__()
    retval+= ' ' + str(len(self.polygon)) + ' ' + str(self.polygon)
    return retval
  def get2DPolygon(self):
    retval= geom.Polygon2d()
    for p in self.polygon:
      coords= [p[0],p[1]]
      if(self.projPlane=='xz'):
        coords[1]= p[2]
      elif(self.projPlane=='yz'):
        coords=  [p[1],p[2]]
      retval.agregaVertice(geom.Pos2d(coords[0],coords[1]))
    return retval
  def searchLoadedElements(self,elemSet):
    ''' Returns elements which have his center inside the polygon'''
    plg= self.get2DPolygon()
    self.tags= list()
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
    self.polygon= list()
    for p in points:
      self.polygon.append(p)

class LoadContainer(object):
  def __init__(self,n):
    self.name= n
    self.punctualLoads= list()
    self.surfaceLoads= list()
    #self.elementSet= 'total'
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
  def __init__(self,id, desc, permanent= False):
    self.id= id
    self.desc= desc #Group description.
    self.permanent= permanent

class LoadCase(object):
  ''' Load case.'''
  def __init__(self,id, name, desc, lg, ltyp):
    self.id= id
    self.name= name
    self.desc= desc #Load description.
    self.loadGroupId= lg
    self.actionType= "Permanent"
    self.ltyp= ltyp
    self.loads= LoadContainer(name)
  def __str__(self):
    return str(self.id) + ' name= ' + self.name + ' desc= ' + self.desc

class LoadCombComponent(object):
  def __init__(self,id, loadCase, coef):
    self.id= id
    self.loadCaseId= loadCase.id
    self.loadCaseName= loadCase.name
    self.coef= coef #Multiplier for load i.
    
def getComponentsFromStr(descompStr,mapLoadCases):
  retval= list()
  components= descompStr.split('+')
  counter= 0
  for c in components:
    factors= c.split('*')
    coef= float(factors[0])
    loadCaseName= factors[1]
    loadCase= mapLoadCases[loadCaseName]
    #print "coef= ", coef, "loadCase= ", loadCaseName, " id= ", loadCase.id
    retval.append(LoadCombComponent(counter,loadCase,coef))
    counter+= 1
  return retval
    

class LoadComb(object):
  ''' Load combination.'''
  def __init__(self,id, name, desc, typ, descomp):
    self.id= id
    self.name= name
    self.desc= desc #Comb description.
    self.typ= typ
    self.descomp= descomp

class LoadData(object):
  '''Container for load groups, cases and combinations.'''
  def __init__(self):
    self.loadGroups= {}
    self.loadCases= {}
    self.loadCombs= {}
  def readLoadCombsFromXC(self,combContainer,mapLoadCases):
    self.loadCombs= combContainer.getNeutralFormat(mapLoadCases)
