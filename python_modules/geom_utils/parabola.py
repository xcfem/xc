# -*- coding: utf-8 -*-
''' Parabola class.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy as np
import math

class Parabola(object):
    ''' Parabola

    :ivar a: x^2 factor.
    :ivar b: x factor.
    :ivar c: constant.
    '''
    def __init__(self,p0,p1,p2):
        self.from3Points(p0,p1,p2)
      
    def from3Points(self,p0,p1,p2):
        "Parabola through three points"
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
      
    def y(self,x):
        ''' Return the ordinate value for x.'''
        return self.a*x*x+self.b*x+self.c
    
    def yP(self,x):
        ''' Return the first derivative value for x.'''
        return 2.0*self.a*x+self.b
    
    def yPP(self,x):
        ''' Return the second derivative value for x.'''
        return 2.0*self.a
    
    def curvature(self,x):
        ''' Return the value of the curvature for x.'''
        return self.yPP(x)/pow((1+self.yP(x)**2),3.0/2.0)
    
    def alpha(self,x):
        ''' Return the angle of the tangent for x.'''
        return math.atan2(self.yP(x),1)
  

# Exact parabola
def fit_parabola(x, y):
    '''Fits the equation "y = ax^2 + bx + c" given exactly 3 points as two
    lists or arrays of x & y coordinates
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
    S is a coordinate by the straight line projection of the parabola on the XY plane.
    angSX is the angle (in rad) between that line and the X-axis
    '''
    s_parab=np.linspace(startS,stopS,numPts)
    x_parab=np.linspace(startS*math.cos(angSX),stopS*math.cos(angSX),numPts)
    y_parab=np.linspace(startS*math.sin(angSX),stopS*math.sin(angSX),numPts)
    z_parab=a*s_parab**2+b*s_parab+c
    return x_parab,y_parab,z_parab
