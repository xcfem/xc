# -*- coding: utf-8 -*-
'''Convenience functions to write geometric entities in DXF format.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import ezdxf
import geom

class DXFDocument(object):
    ''' Interface with ezdxf for the entities of the geom module.

    :ivar msp: ezdxf model space.
    '''
    def __init__(self, ezdxfDoc):
        ''' Constructor.

        :param ezdxfDoc: ezdxf document.
        '''
        self.doc= ezdxfDoc
        self.msp= self.doc.modelspace()

    @staticmethod
    def _set_attributes(entity, layerName= None, color= None, lineType= None, transparency= None, lineWeight= None):
        '''Set the attributes of the given entity.

        :param entity: entity to set attributes for.
        :param color: color of the new entity.
        :param lineType: line type of the new entity.
        :param transparenciy: transparency of the new entity.
        :param lineWeight: line weight of the new entity.
        '''
        if(layerName):
            entity.dxf.layer= layerName
        if(color):
            entity.dxf.color= color
        if(lineType):
            entity.dxf.linetype= lineType
        if(transparency):
            entity.dxf.linetype= transparency
        if(lineWeight):
            entity.dxf.linetype= lineWeight
        
    def newLayer(self, layerName, color= None, lineType= None):
        ''' Create a new layer with the given data.

        :param layerName: name for the new layer.
        :param color: color of the new layer.
        :param lineType: line type of the new layer.
        '''
        newLayer= self.doc.layers.new(name= layerName)
        if(color):
            newLayer.color= color
        if(lineType):
            newLayer.linetype= lineType
        return newLayer

    def addSegment2d(self, segment2d:geom.Segment2d, layerName= None, color= None, lineType= None, transparency= None, lineWeight= None):
        '''Add a 2D segment to the DXF document.

        :param segment2d: 2D segment.
        :param color: color of the new entity.
        :param lineType: line type of the new entity.
        :param transparenciy: transparency of the new entity.
        :param lineWeight: line weight of the new entity.
        '''
        p0= segment2d.getFromPoint()
        p1= segment2d.getToPoint()
        retval= self.msp.add_line((p0.x, p0.y), (p1.x, p1.y))
        DXFDocument._set_attributes(entity= retval, layerName= layerName, color= color, lineType= lineType, transparency= transparency, lineWeight= lineWeight)
        return retval

    def addSegment3d(self, segment3d:geom.Segment3d, layerName= None, color= None, lineType= None, transparency= None, lineWeight= None):
        '''Add a 3D segment to the DXF document.

        :param segment3d: 3D segment.
        :param color: color of the new entity.
        :param lineType: line type of the new entity.
        :param transparenciy: transparency of the new entity.
        :param lineWeight: line weight of the new entity.
        '''
        p0= segment3d.getFromPoint()
        p1= segment3d.getToPoint()
        retval= self.msp.add_line((p0.x, p0.y, p0.z), (p1.x, p1.y, p1.z))
        DXFDocument._set_attributes(entity= retval, layerName= layerName, color= color, lineType= lineType, transparency= transparency, lineWeight= lineWeight)
        return retval

    def addPolyline2d(self, polyline2d:geom.Polyline2d, layerName= None, color= None, lineType= None, transparency= None, lineWeight= None):
        ''' Add a 2D polyline to the DXF document.

        :param msp: ezdxf model space.
        :param polyline2d: 2D polyline.
        :param color: color of the new entity.
        :param lineType: line type of the new entity.
        :param transparenciy: transparency of the new entity.
        :param lineWeight: line weight of the new entity.
        '''
        vertices= polyline2d.getVertexList()
        pts= [(p.x, p.y) for p in vertices]
        retval= self.msp.add_lwpolyline(pts)
        DXFDocument._set_attributes(entity= retval, layerName= layerName, color= color, lineType= lineType, transparency= transparency, lineWeight= lineWeight)
        return retval
        
    def addPolyline3d(self, polyline3d:geom.Polyline3d, layerName= None, color= None, lineType= None, transparency= None, lineWeight= None):
        ''' Add a 3D polyline to the DXF document.

        :param msp: ezdxf model space.
        :param polyline3d: 3D polyline.
        :param color: color of the new entity.
        :param lineType: line type of the new entity.
        :param transparenciy: transparency of the new entity.
        :param lineWeight: line weight of the new entity.
        '''
        vertices= polyline3d.getVertexList()
        pts= [(p.x, p.y, p.z) for p in vertices]
        retval= self.msp.add_polyline3d(pts)
        DXFDocument._set_attributes(entity= retval, layerName= layerName, color= color, lineType= lineType, transparency= transparency, lineWeight= lineWeight)
        return retval
        
