# -*- coding: utf-8 -*-
'''Definition of the profile of a tendon as a sequence of curves.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
from geom_utils import parabola as pb
import numpy as np

class TendonProfile(object):
    ''' Definition of the profile of a tendon as a sequence of curves.

    :ivar control_points: control points along the tendon profile. Each item
                          in the point list is a geom.Pos2d object with the
                          2D coordinates of the vertices.
    :ivar curve_sequence: sequence of curves along the tendon profile. Each
                          item in the sequence define the indexes of the control
                          points defining the curve and the complementary
                          parameters defining its vertex, its radius, etc.
    '''
    def __init__(self, control_points, curve_sequence):
        ''' Constructor.

        :param control_points: control points along the tendon profile. Each 
                               item in the point list is a geom.Pos2d object
                               with the 2D coordinates of the vertices.
        :param curve_sequence: sequence of curves along the tendon profile. Each
                               item in the sequence define the indexes of the 
                               control points defining the curve and the 
                               complementary parameters defining its vertex, 
                               its radius, etc.
        '''
        self.control_points= control_points
        self.curve_sequence= curve_sequence

    def _get_curve_sequence(self):
        ''' Return a list containing a geometric curve corresponding to each
            item the curve sequence definition and a list of the abscissae
            that separate the domains of each curve.
        '''
        curves= list()
        curve_domains= list()
        for curve_def in self.curve_sequence:
            tp= curve_def['type']
            if(tp=='parabolic'):
                point_indexes= curve_def['point_indexes']
                p0= self.control_points[point_indexes[0]]
                p1= self.control_points[point_indexes[1]]
                curve_domains.append(p1.x)
                b= p1.x-p0.x
                h= p1.y-p0.y
                vertex_index= curve_def['vertex_index']
                if(vertex_index==0):
                    vertex= p0
                else: # vertex_index==1
                    h= -h
                    vertex= p1
                curve= pb.def_tendon_parabolic_segment(vertex= vertex, b=b, h= h)
                curves.append(curve)
        return curves, curve_domains

    def getPoints(self, spacing):
        ''' Return a 2D point list corresponding to the tendon profile.

        :param spacing: horizontal distance between sample points.
        '''
        # Compute the sequence of curves.
        curves, curve_domains= self._get_curve_sequence()
        # Compute the points along the profile.
        x0= self.control_points[0].x
        xL= self.control_points[-1].x
        profileLength= xL-x0
        num= int(profileLength/spacing)
        xi= np.linspace(start= x0, stop= xL, num= num)
        current_curve_index= 0
        current_curve= curves[current_curve_index]
        current_domain= curve_domains[current_curve_index]
        retval= list()
        for x in xi:
            xf= float(x)
            if(xf>current_domain): # update curve.
                current_curve_index+= 1
                current_curve= curves[current_curve_index]
                current_domain= curve_domains[current_curve_index]
            y= current_curve.y(xf)
            retval.append(geom.Pos2d(x, y))
        return retval
