# -*- coding: utf-8 -*-
''' Simple utilities to draw longitudinal sections.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import geom

class EmbankmentProfile(object):
    ''' 2D polyline representing the profile of the backfill.

    :ivar soil: soil model.
    :ivar pline: plane 3D polyline representing the profile.
    '''
    def __init__(self, soil, pline):
        ''' Constructor.
        
        :param soil: soil model.
        :param pline: plane 3D polyline representing the profile.
        '''
        self.soil= soil
        self.pline= pline

    def getXMin(self):
        ''' Return the minimum x coordinate of the profile.'''
        return self.pline.getXMin

    def getXMax(self):
        ''' Return the maximum x coordinate of the profile.'''
        return self.pline.getXMax

    def getDepths(self, pointList, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the depths corresponding to the points in the list
            argument.

        :param pointList: points whose depths will be computed.
        '''
        # Compute boundaries.
        xMin= self.getXMin()
        xMax= self.getXMax()
        retval= list()
        for p in pointList:
            prj= self.pline.getPlane().getProjection(p)
            x= prj.x
            if(x<=xMax and x>=xMin): # Check boundaries.
                vLine= geom.Line3d(prj, 100*verticalDir) # Vertical line.
                intPoints= self.pline.getIntersection(vLine)
                for ip in intPoints:
                    v= ip-prj # Vector from the intersection to the proj. pt.
                    dist= verticalDir.dot(v) # Dot product.
                    retval.append(dist)
        return retval
                
                
        
