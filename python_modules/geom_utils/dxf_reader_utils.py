# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import re
import dxfgrabber
from dxfwrite import DXFEngine as dxf
import xc_base
import geom
from misc_utils import log_messages as lmsg

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

class DXFReader(object):
    '''Import/exports DXF entities.'''
    def __init__(self,dxfFileName,layerNamesToImport):
        ''' Constructor.

           :param layerNamesToImport: list of regular expressions to be tested.
           :param getRelativeCoo: coordinate transformation to be applied to the
                                  points.
        '''
        self.options= {"grab_blocks":True,"assure_3d_coords":True,"resolve_text_styles":True}
        self.dxfFile= dxfgrabber.readfile(filename= dxfFileName, options= self.options)
        self.layersToImport= self.getLayersToImport(layerNamesToImport)
        self.points= {}
        self.lines= {}
        self.polylines= {}
        self.surfaces= {}
        self.labelDict= {}
        self.importPoints()
        self.importLines()
        self.importPolylines()
        self.importSurfaces()

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

    def getPoint3d(self,dxfPoint):
        ''' Return a Point3d object from DXF coordinates.'''
        return geom.Point3d(dxfPoint[0],dxfPoint[1],dxfPoint[2])

    def importPoints(self):
      ''' Import points from DXF.'''
      for obj in self.dxfFile.entities:
        type= obj.dxftype
        pointName= obj.handle
        layerName= obj.layer
        if(layerName in self.layersToImport):
          if(type == 'POINT'):
            self.points[pointName]= self.getPoint3d(obj.point)
            self.labelDict[pointName]= [layerName]
      return self.points

    def importLines(self):
      ''' Import lines from DXF.'''
      for obj in self.dxfFile.entities:
        type= obj.dxftype
        lineName= obj.handle
        layerName= obj.layer
        if(layerName in self.layersToImport):
            if(type == 'LINE'):
                vertices= [-1,-1]
                p1= self.getPoint3d(obj.start)
                p2= self.getPoint3d(obj.end)
                self.lines[lineName]= geom.Segment3d(p1.getPos3d(),p2.getPos3d())
                self.labelDict[lineName]= [layerName]
                      
    def importPolylines(self):
      ''' Import polylines from DXF.'''
      for obj in self.dxfFile.entities:
          type= obj.dxftype
          plineName= obj.handle
          layerName= obj.layer
          if(layerName in self.layersToImport):
              if((type == 'POLYLINE') or (type == 'LWPOLYLINE')):
                  polyline= geom.Polyline3d()
                  for p in obj.points:
                      rCoo= self.getPoint3d(p)
                      polyline.append(self.getIndexNearestPoint(rCoo))
                  self.labelDict[plineName]= [layerName]

    def importSurfaces(self):
      ''' Import surfaces from DXF.'''

      for obj in self.dxfFile.entities:
        type= obj.dxftype
        layerName= obj.layer
        if(layerName in self.layersToImport):
            if(type == '3DFACE'):
                polygon= geom.Polygon3d()
                for pt in obj.points:
                    p= self.getPoint3d(pt)
                    polygon.append(p)
                self.labelDict[obj.handle]= [layerName]
            elif(type == 'POLYFACE'):
                lmsg.warning('Import of polyface objects not implemented yet.')

    def exportToDXF(self, fileName):
        ''' Export entities to DXF.'''
        drawing = dxf.drawing(fileName)
        for layer in self.layersToImport:
            drawing.add_layer(layer)
        for key in self.points:
            p= self.points[key]
            p1= l.getPos3d()
            layerName= self.labelDict[key]
            drawing.add(dxf.point((p1.x, p1.y), layer=layerName))
        for key in self.lines:
            l= self.lines[key]
            p1= l.getFromPoint()
            p2= l.getToPoint()
            layerName= self.labelDict[key]
            drawing.add(dxf.line((p1.x, p1.y), (p2.x, p2.y), layer=layerName))
        if(len(self.polylines)>0):
            lmsg.warning('Export of polyline objects not implemented yet.')
        drawing.save()
