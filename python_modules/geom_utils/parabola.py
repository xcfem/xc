# -*- coding: utf-8 -*-
''' Parabola class.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy as np
import math
import geom

class Parabola(object):
    ''' Parabola

    :ivar a: x^2 factor.
    :ivar b: x factor.
    :ivar c: constant.
    '''
    def __init__(self,p0,p1,p2):
        '''Parabola through three points.

        :param p0: first point.
        :param p1: second point.
        :param p2: third point.
        '''
        self.from3Points(p0,p1,p2)
      
    def from3Points(self,p0,p1,p2):
        '''Parabola through three points.

        :param p0: first point.
        :param p1: second point.
        :param p2: third point.
        '''
        x0= p0[0]
        y0= p0[1]
        x1= p1[0]
        y1= p1[1]
        x2= p2[0]
        y2= p2[1]
        a= np.array([[x0**2,x0,1],[x1**2,x1,1],[x2**2,x2,1]])
        b= np.array([y0,y1,y2])
        abc= np.linalg.solve(a,b)
        self.a= abc[0]
        self.b= abc[1]
        self.c= abc[2]
      
    def y(self, x):
        ''' Return the ordinate value for the given x.

        :param x: abscissa.
        '''
        return self.a*x*x+self.b*x+self.c
    
    def yP(self, x):
        ''' Return the first derivative value for the given x.

        :param x: abscissa.
        '''
        return 2.0*self.a*x+self.b
    
    def yPP(self, x):
        ''' Return the second derivative value for the given x.

        :param x: abscissa.
        '''
        return 2.0*self.a
    
    def curvature(self, x):
        ''' Return the value of the curvature for the given x.

        :param x: abscissa.
        '''
        return self.yPP(x)/pow((1+self.yP(x)**2),3.0/2.0)
    
    def alpha(self, x):
        ''' Return the angle of the tangent for the given x.

        :param x: abscissa.
        '''
        return math.atan2(self.yP(x),1)

def define_parabola_from_vertex_and_point(vertex, pt):
    ''' Return the parabola defined by the given vertex and point.

    :param vertex: position of the parabola vertex.
    :param pt: point on the parabola (different than vertex).
    '''
    x= abs(pt.x)
    p0= geom.Pos2d(-x,pt.y)
    p1= vertex
    p2= geom.Pos2d(x, pt.y)
    return Parabola(p0, p1, p2)
    
  
class GeneralizedParabola(Parabola):
    ''' Moved and/or rotated parabola.

    :ivar ref: reference system.
    '''
    def __init__(self, ref, p0, p1, p2):
        '''Parabola through three points.

        :param ref: reference system of the parabola.
        :param p0: first point.
        :param p1: second point.
        :param p2: third point.
        '''
        self.from3Points(ref, p0,p1,p2)
      
    def from3Points(self, ref, p0, p1, p2):
        '''Parabola through three points.

        :param ref: reference system of the parabola.
        :param p0: first point.
        :param p1: second point.
        :param p2: third point.
        '''
        self.ref= ref
        localP0= ref.getLocalPosition(p0)
        localP1= ref.getLocalPosition(p1)
        localP2= ref.getLocalPosition(p2)
        if(localP0.x>localP2.x):
            localP0, localP2= localP2, localP0
        super().from3Points(p0= localP0, p1= localP1, p2= localP2)

    def y(self, pos):
        ''' Return the position on the parabola corresponding to the
            the given 2D position.

        :param pos: 2D position expressed in global coordinates.
        '''
        localPos= self.ref.getLocalPosition(pos)
        x= localPos[0]
        y= super().y(x)
        return self.ref.getGlobalPosition(geom.Pos2d(x,y))
    
    def yP(self, pos):
        ''' Return the first derivative value for the given 2D position.

        :param pos: 2D position expressed in global coordinates.
        '''
        localPos= self.ref.getLocalPosition(pos)
        x= localPos[0]
        return super().yP(x)
    
    def yPP(self, pos):
        ''' Return the second derivative value for the given 2D position.

        :param pos: 2D position expressed in global coordinates.
        '''
        return super().yPP(0.0)
    
    def curvature(self, pos):
        ''' Return the value of the curvature for the given 2D position.

        :param pos: 2D position expressed in global coordinates.
        '''
        localPos= self.ref.getLocalPosition(pos)
        x= localPos[0]
        return super().yPP(x)/pow((1+super().yP(x)**2),3.0/2.0)
    
    def alpha(self, x):
        ''' Return the angle of the tangent for the given 2D position.

        :param pos: 2D position expressed in global coordinates.
        '''
        localPos= self.ref.getLocalPosition(pos)
        x= localPos[0]
        return super().alpha(x)
    
def define_generalized_parabola_from_vertex_and_point(ref, vertex, pt):
    ''' Return the parabola defined by the given vertex and point.

    :param ref: local reference system of the parabola.
    :param vertex: position of the parabola vertex in global coordinates.
    :param pt: point on the parabola (different than vertex) in global
               coordinates.
    '''
    localPt= ref.getLocalPosition(pt)
    local_x= abs(localPt.x)
    p0= ref.getGlobalPosition(geom.Pos2d(-local_x, localPt.y))
    p1= vertex
    p2= ref.getGlobalPosition(geom.Pos2d(local_x, localPt.y))
    return GeneralizedParabola(ref, p0, p1, p2)
    
# Exact parabola
def fit_parabola(x, y):
    '''Fits the equation "y = ax^2 + bx + c" given exactly 3 points as two
    lists or arrays of x & y coordinates.

    :param x: abscissa.
    :param y: ordinate.
    '''
    A = np.zeros((3,3), dtype=float)
    A[:,0] = x**2
    A[:,1] = x
    A[:,2] = 1
    a, b, c = np.linalg.solve(A, y)
    return a, b, c

def eq_points_parabola(startS,stopS,numPts,a,b,c,angSX):
    '''Returns equispaced nPts points of the parabola "z=as^2 + bs + c" 
    in the range [startS,stopS].
    S is a coordinate by the straight line projection of the parabola on 
    the XY plane.
    angSX is the angle (in rad) between that line and the X-axis
    '''
    s_parab= np.linspace(startS,stopS,numPts)
    x_parab= np.linspace(startS*math.cos(angSX),stopS*math.cos(angSX),numPts)
    y_parab= np.linspace(startS*math.sin(angSX),stopS*math.sin(angSX),numPts)
    z_parab= a*s_parab**2+b*s_parab+c
    return x_parab,y_parab,z_parab
