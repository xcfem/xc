# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import dxfgrabber
import xc_base
import geom
import xc
import re
from scipy.spatial.distance import cdist
from import_export import BlockTopologyEntities as bte

def layerToImport(layerName,namesToImport):
  '''Return true if the layer name matches one of the regular expressions
     contained in the second argument.

     :param layerName: name of the layer.
     :param namesToImport: list of regular expressions to be tested.
  '''
  for regExp in namesToImport:
    if(re.match(regExp,layerName)):
      return True
  return False

class DXFImport(object):
  '''Import DXF entities.'''
  def __init__(self,dxfFileName,layerNamesToImport, getRelativeCoo, threshold= 0.01,importLines= True, importSurfaces= True):
    ''' Constructor.

       :param layerNamesToImport: list of regular expressions to be tested.
       :param getRelativeCoo: coordinate transformation to be applied to the
                              points.
    '''
    self.dxfFile= dxfgrabber.readfile(dxfFileName)
    self.layersToImport= self.getLayersToImport(layerNamesToImport)
    self.getRelativeCoo= getRelativeCoo
    self.threshold= threshold
    self.selectKPoints()
    self.labelDict= {}
    self.points= self.importPoints()
    if(importLines):
      self.importLines()
    else:
      self.lines= {}
    if(importSurfaces):
      self.importFaces()
    else:
      self.facesByLayer= {}
    
  def getIndexNearestPoint(self, pt):
    return cdist([pt], self.kPoints).argmin()

  def getNearestPoint(self, pt):
    return self.kPoints[self.getIndexNearestPoint(pt)]

  def getLayersToImport(self, namesToImport):
    '''Return the layers names that will be imported according to the
       regular expressions contained in the second argument.

       :param namesToImport: list of regular expressions to be tested.
    '''
    retval= []
    for layer in self.dxfFile.layers:
      layerName= layer.name
      if(layerToImport(layer.name,namesToImport)):
        retval.append(layer.name)
    return retval
  
  def extractPoints(self):
    '''Extract the points from the entities argument.'''
    retval= []
    for obj in self.dxfFile.entities:
      type= obj.dxftype
      layerName= obj.layer
      if(layerName in self.layersToImport):
        if(type == '3DFACE'):
          for pt in obj.points:
            retval.append(self.getRelativeCoo(pt))
        elif(type == 'LINE'):
          for pt in [obj.start,obj.end]:
            retval.append(self.getRelativeCoo(pt))
        elif(type == 'POINT'):
          retval.append(self.getRelativeCoo(obj.point))
        elif(type=='POLYLINE'):
          pts= obj.points
          for pt in pts:
            retval.append(self.getRelativeCoo(pt))
    return retval
  
  def selectKPoints(self):
    '''Selects the k-points to be used in the model. All the points that
       are closer than the threshold distance are melted into one k-point.
    '''
    points= self.extractPoints()
    self.kPoints= [points[0]]
    for p in points:
      nearestPoint= self.getNearestPoint(p)
      dist= cdist([p],[nearestPoint])[0][0]
      if(dist>self.threshold):
        self.kPoints.append(p)

  def importPoints(self):
    ''' Import points from DXF.'''
    retval= {}
    for obj in self.dxfFile.entities:
      type= obj.dxftype
      layerName= obj.layer
      if(layerName in self.layersToImport):
        if(type == 'POINT'):
          vertices= [-1]
          p= self.getRelativeCoo(obj.point)
          vertices[0]= self.getIndexNearestPoint(p,kPoints)
          retval[obj.handle]= (layerName, vertices)
    return retval

  def importLines(self):
    ''' Import lines from DXF.'''
    self.lines= {}
    self.polylines= {}
    for obj in self.dxfFile.entities:
      type= obj.dxftype
      lineName= obj.handle
      layerName= obj.layer
      if(layerName in self.layersToImport):
        if(type == 'LINE'):
          vertices= [-1,-1]
          p1= self.getRelativeCoo(obj.start)
          p2= self.getRelativeCoo(obj.end)
          length= cdist([p1],[p2])[0][0]
          vertices[0]= self.getIndexNearestPoint(p1)
          vertices[1]= self.getIndexNearestPoint(p2)
          if(vertices[0]==vertices[1]):
            print 'Error in line ', lineName, ' vertices are equal: ', vertices
          if(length>self.threshold):
            self.lines[lineName]= vertices
            self.labelDict[lineName]= [layerName]
          else:
            print 'line too short: ', p1, p2, length
        elif(type == 'POLYLINE'):
          vertices= set()
          for p in obj.points:
            rCoo= self.getRelativeCoo(p)
            vertices.add(self.getIndexNearestPoint(rCoo))
            self.polylines[lineName]= vertices
            self.labelDict[lineName]= [layerName]
            
  def importFaces(self):
    ''' Import 3D faces from DXF.'''
    self.facesByLayer= {}
    for name in self.layersToImport:
      self.facesByLayer[name]= dict()

    for obj in self.dxfFile.entities:
      type= obj.dxftype
      layerName= obj.layer
      if(layerName in self.layersToImport):
        facesDict= self.facesByLayer[layerName]
        if(type == '3DFACE'):
          vertices= []
          for pt in obj.points:
            p= self.getRelativeCoo(pt)
            idx= self.getIndexNearestPoint(p)
            vertices.append(idx)
          #print layerName, obj.handle
          self.labelDict[obj.handle]= [layerName]
          facesDict[obj.handle]= vertices

  def exportBlockTopology(self, name):
    retval= bte.BlockData()
    retval.name= name

    counter= 0
    for p in self.kPoints:
      retval.appendPoint(id= counter,x= p[0],y= p[1],z= p[2])
      counter+= 1

    counter= 0
    for key in self.lines:
      line= self.lines[key]
      block= bte.BlockRecord(counter,'line',line,self.labelDict[key])
      retval.appendBlock(block)
      counter+= 1

    for name in self.layersToImport:
      fg= self.facesByLayer[name]
      for key in fg:
        face= fg[key]
        block= bte.BlockRecord(counter,'face',face,self.labelDict[key])
        retval.appendBlock(block)
        counter+= 1
    return retval
    


class OldDxfReader(object):
  '''Reading of DXF entities for further processing.'''
  def __init__(self,tol= 1e-3):
    self.tol= tol
  def newKeyPoint(self,pt):
    retval= None
    pos3D= geom.Pos3d(pt[0],pt[1],pt[2])
    dist= 1e9
    nearestPoint= self.pointHandler.getNearest(pos3D)
    if(nearestPoint):
      dist= nearestPoint.getPos.distPos3d(pos3D)
    if(dist>self.tol):
      retval= self.pointHandler.newPntFromPos3d(pos3D)
    return retval;
  def newLine(self,l):
    start= l.start
    posStart= geom.Pos3d(start[0],start[1],start[2])
    nearestPointStart= self.pointHandler.getNearest(posStart)
    end= l.end
    posEnd= geom.Pos3d(end[0],end[1],end[2])
    nearestPointEnd= self.pointHandler.getNearest(posEnd)
    return self.lineHandler.newLine(nearestPointStart.tag,nearestPointEnd.tag)
  def read_points(self,entities,layerName):
    retval= []
    for obj in entities:
      if(obj.layer == layerName):
        type= obj.dxftype
        color= obj.color
        if(type=='LINE'):
          pt= self.newKeyPoint(obj.start)
          if(pt):
            retval.append(pt.tag)
          pt= self.newKeyPoint(obj.end)
          if(pt):
            retval.append(pt.tag)
        elif(type=='POLYLINE'):
          pts= obj.points
          for p in pts:
            pt= self.newKeyPoint(p)
            if(pt):
              retval.append(pt.tag)
    return retval
  def read_lines(self,entities,layerName):
    retval= []
    for obj in entities:
      if(obj.layer == layerName):
        type= obj.dxftype
        color= obj.color
        if(type=='LINE'):
          retval.append(self.newLine(obj).tag)
        if(type=='POLYLINE'):
          pts= obj.points
          sz= len(pts)
          for i in range(0,sz):
            l= dxfgrabber.Line()
            l.start= pts[i]
            l.end= pts[i+1]
            retval.append(self.newLine(l).tag)
    return retval
  def read(self,dxf_file_name,xc_preprocessor,layers):
    self.dxf= dxfgrabber.readfile(dxf_file_name)
    self.preprocessor= xc_preprocessor
    self.pointHandler= self.preprocessor.getMultiBlockTopology.getPoints
    self.lineHandler= self.preprocessor.getMultiBlockTopology.getLines
    self.pointsInLayer= {}
    self.blocksInLayer= {}
    for l in layers:
      self.pointsInLayer[l]= self.read_points(self.dxf.entities,l)
      self.blocksInLayer[l]= self.read_lines(self.dxf.entities,l)


