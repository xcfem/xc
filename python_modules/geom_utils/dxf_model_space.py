# -*- coding: utf-8 -*-
'''Convenience functions to write geometric entities in DXF format.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import ezdxf
import geom

class DXFModelSpace(object):
    ''' Interface with ezdxf for the entities of the geom module.

    :ivar msp: ezdxf model space.
    '''
    def __init__(self, msp):
        ''' Constructor.

        :param msp: ezdxf model space.
        '''
        self.msp= msp

    def addPolyline2d(self, polyline2d:geom.Polyline2d):
        ''' Add a 2D polyline to the given model space.

        :param msp: ezdxf model space.
        :param polyline2d: 2D polyline.
        '''
        vertices= polyline2d.getVertexList()
        pts= [(p.x, p.y) for p in vertices]
        self.msp.add_lwpolyline(pts)
        
    def addPolyline3d(self, polyline3d:geom.Polyline3d):
        ''' Add a 3D polyline to the given model space.

        :param msp: ezdxf model space.
        :param polyline3d: 3D polyline.
        '''
        vertices= polyline3d.getVertexList()
        pts= [(p.x, p.y, p.z) for p in vertices]
        self.msp.add_polyline3d(pts)
        
