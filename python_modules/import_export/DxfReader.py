# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import math
import dxfgrabber
import xc_base
import geom
import xc
import re
from scipy.spatial.distance import cdist
from import_export import BlockTopologyEntities as bte
from miscUtils import LogMessages as lmsg

class FloatList(list):
    '''List of floats that are more than
       "tol" apart.
    '''
    def __init__(self,tol= .01):
      super(FloatList,self).__init__()
      self.tol= tol
    def append(self,num):
        dMin= 1e23
        for v in self:
            dMin= min(abs(num-v), dMin)
        if(dMin>self.tol):
            super(FloatList,self).append(num)


def get_candidate_2Dquads(sisRef, points, tol):
    '''Return candidate quads in 2D.'''
    # Compute relative coordinates.
    x_i= FloatList(tol)
    y_i= FloatList(tol)
    for p in points:
        pRel= sisRef.getPosLocal(geom.Pos3d(p[0],p[1],p[2]))
        x_i.append(pRel[0])
        y_i.append(pRel[1])
    x_i.sort()
    nx= len(x_i)
    y_i.sort()
    ny= len(y_i)
    # Create candidate surfaces.
    candidates= list()
    for i in range(0,nx-1):
        for j in range(0, ny-1):
            x= x_i[i]; y= y_i[j]
            p1= geom.Pos2d(x,y)
            x= x_i[i+1]; y= y_i[j]
            p2= geom.Pos2d(x,y)
            x= x_i[i+1]; y= y_i[j+1]
            p3= geom.Pos2d(x,y)
            x= x_i[i]; y= y_i[j+1]
            p4= geom.Pos2d(x,y)
            face= geom.Polygon2d()
            face.appendVertex(p1)
            face.appendVertex(p2)
            face.appendVertex(p3)
            face.appendVertex(p4)
            candidates.append(face)
    return candidates

def quads2d_to_global_coordinates(sisRef, selected_quads):
    '''Convert the quads from 2D to 3D.'''
    retval= list()
    for face in selected_quads:
        vList= list()
        vertices= face.getVertices()
        for v in vertices:
            vList.append(sisRef.getPosGlobal(geom.Pos3d(v.x,v.y,0.0)))
        retval.append(vList)
    return retval
    
def get_polygon_axis(points, tol):
    '''Compute the polygon axis on the assumption that
       they all the sides are orthogonal.'''
    pline= geom.Polyline3d()
    for p in points:
        pline.appendVertex(geom.Pos3d(p[0],p[1],p[2]))
    pline.simplify(tol)
    pt0= pline[1]
    pt1= pline[2]
    pt2= pline[3]
    v1= pt0-pt1
    v2= pt2-pt1
    return geom.Ref3d3d(pt1,v1,v2)    
    
def decompose_polyline(polyline, tol= .01):
    '''Return the quadrilateral surfaces that
       compose the polyline.
    '''
    retval= list()
    if((len(polyline.points)>2) and polyline.is_closed):
        # Compute the local axis.
        points= list()
        for pt in polyline.points:
            points.append([pt[0],pt[1],pt[2]])            
        sisRef= get_polygon_axis(points,tol)

        # Create candidate surfaces.
        candidates= get_candidate_2Dquads(sisRef, points, tol)

        # Create polygon in local coordinates.
        polygon= geom.Polygon2d()
        for pt in polyline:
            ptLocal= sisRef.getPosLocal(geom.Pos3d(pt[0],pt[1],pt[2]))
            polygon.appendVertex(geom.Pos2d(ptLocal.x,ptLocal.y))

        # Select surfaces inside polygon.
        selected= list()
        for face in candidates:
            c= face.getCenterOfMass()
            if(polygon.In(c,tol/5.0)):
                selected.append(face)
        retval= quads2d_to_global_coordinates(sisRef, selected)

    return retval

def get_polyface_points_axis(polyface_points):
    ''' Return the axis for the polyface.'''
    p= geom.Plane3d()
    p.linearLeastSquaresFitting(polyface_points)
    global_z= geom.Vector3d(0.0,0.0,1.0)
    local_z= p.getNormal()
    angle= local_z.getAngle(global_z)
    local_y= global_z
    global_y= geom.Vector3d(0.0,1.0,0.0)    
    if(abs(angle)<1e-3 or abs(angle-math.pi)<1e-3):
        local_y= global_y
    local_x= local_y.cross(local_z)
    local_y= local_z.cross(local_x)
    org= polyface_points[0]
    return geom.Ref3d3d(org,local_x,local_y)    
    
def decompose_polyface(polyface, tol= .01):
    '''Return the quadrilateral surfaces that
       compose the polyface.
    '''
    # Compute the reference axis.
    points= geom.polyPos3d()
    for face in polyface:
        for pt in face:
            points.append(geom.Pos3d(pt[0],pt[1],pt[2]))
    sisRef= get_polyface_points_axis(points)

    # Create candidate surfaces.
    candidates= get_candidate_2Dquads(sisRef,points, tol)

    # Create polygons in local coordinates.
    polyfaces2d= list()
    for face in polyface:
        polygon= geom.Polygon2d()
        for pt in face:
            ptLocal= sisRef.getPosLocal(geom.Pos3d(pt[0],pt[1],pt[2]))
            polygon.appendVertex(geom.Pos2d(ptLocal.x,ptLocal.y))
        polyfaces2d.append(polygon)
    # Select surfaces inside polyface.
    selected= list()
    for face in candidates:
        c= face.getCenterOfMass()
        for polyface in polyfaces2d:
            if(polyface.In(c,tol/5.0)):
                selected.append(face)
                break
    return quads2d_to_global_coordinates(sisRef, selected)

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
    def __init__(self,dxfFileName,layerNamesToImport, getRelativeCoo, threshold= 0.01,importLines= True, importSurfaces= True, polylinesAsSurfaces= False, tolerance= .01):
        ''' Constructor.

           :param layerNamesToImport: list of regular expressions to be tested.
           :param getRelativeCoo: coordinate transformation to be applied to the
                                  points.
        '''
        self.options= {"grab_blocks":True,"assure_3d_coords":True,"resolve_text_styles":True}
        self.dxfFile= dxfgrabber.readfile(filename= dxfFileName, options= self.options)
        self.tolerance= tolerance
        self.impLines= importLines
        self.impSurfaces= importSurfaces
        self.polylinesAsSurfaces= polylinesAsSurfaces
        self.layersToImport= self.getLayersToImport(layerNamesToImport)
        self.polyfaceQuads= dict()
        self.polylineQuads= dict()
        self.getRelativeCoo= getRelativeCoo
        self.threshold= threshold
        self.selectKPoints()
        self.labelDict= {}
        self.points= self.importPoints()
        if(self.impLines):
            self.importLines()
        else:
            self.lines= {}
        if(self.impSurfaces):
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
                if(type == 'POINT'):
                    retval.append(self.getRelativeCoo(obj.point))
                if(self.impSurfaces):
                    if(type == '3DFACE'):
                        for pt in obj.points:
                            retval.append(self.getRelativeCoo(pt))
                    elif(type == 'POLYFACE'):
                        self.polyfaceQuads[obj.handle]= decompose_polyface(obj, tol= self.tolerance)
                        for q in self.polyfaceQuads[obj.handle]:
                            for pt in q:
                                retval.append(self.getRelativeCoo(pt))
                if(type == 'LINE'):
                    for pt in [obj.start,obj.end]:
                        retval.append(self.getRelativeCoo(pt))
                elif((type == 'POLYLINE') or (type == 'LWPOLYLINE')):
                    if(self.polylinesAsSurfaces):
                        self.polylineQuads[obj.handle]= decompose_polyline(obj, tol= self.tolerance)
                        for q in self.polylineQuads[obj.handle]:
                            for pt in q:
                                retval.append(self.getRelativeCoo(pt))                        
                    else:
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
              lmsg.error('Error in line '+lineName+' vertices are equal: '+str(vertices))
            if(length>self.threshold):
              self.lines[lineName]= vertices
              self.labelDict[lineName]= [layerName]
            else:
              lmsg.error('line too short: '+str(p1)+','+str(p2)+str(length))
          elif((type == 'POLYLINE') or (type == 'LWPOLYLINE')):
              if(not self.polylinesAsSurfaces): # Import as lines
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
                vertices= list()
                for pt in obj.points:
                    p= self.getRelativeCoo(pt)
                    idx= self.getIndexNearestPoint(p)
                    vertices.append(idx)
                self.labelDict[obj.handle]= [layerName]
                facesDict[obj.handle]= vertices
            elif(type == 'POLYFACE'):
                count= 0
                for q in self.polyfaceQuads[obj.handle]:
                    vertices= list()
                    for pt in q:
                        p= self.getRelativeCoo(pt)
                        idx= self.getIndexNearestPoint(p)
                        if not idx in vertices:
                            vertices.append(idx)
                        else:
                            lmsg.error('Point p: '+str(p)+' idx: '+str(idx)+' repeated in '+str(q)+' vertices: '+str(vertices))
                    count+= 1
                    id= obj.handle+'_'+str(count)
                    self.labelDict[id]= [layerName]
                    facesDict[id]= vertices
            elif((type == 'POLYLINE') or (type == 'LWPOLYLINE')):
                count= 0
                if(self.polylinesAsSurfaces): # Import as surfaces
                    for q in self.polylineQuads[obj.handle]:
                        vertices= list()
                        for pt in q:
                            p= self.getRelativeCoo(pt)
                            idx= self.getIndexNearestPoint(p)
                            if not idx in vertices:
                                vertices.append(idx)
                            else:
                                lmsg.error('Point p: '+str(p)+' idx: '+str(idx)+' repeated in '+str(q)+' vertices: '+str(vertices))
                        count+= 1
                        id= obj.handle+'_'+str(count)
                        self.labelDict[id]= [layerName]
                        facesDict[id]= vertices
            elif(type == 'LINE'):
                count= 0
                # Nothing to do with lines for the moment.
            else:
              lmsg.log('Entity of type: '+type+' ignored.')      


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
    return retval
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
        elif((type == 'POLYLINE') or (type == 'LWPOLYLINE')):
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
        if((type == 'POLYLINE') or (type == 'LWPOLYLINE')):
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


