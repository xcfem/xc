# -*- coding: utf-8 -*-
'''Soil settlement.

   Code based on the chapter 5 of the book "Principles of 
   Foundation Engineering, SI Edition.
   Seventh edition. Braja M. Das.
   ISBN-13: 978-0-495-66812-1
'''

from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2017, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy as np
import math
import geom

def stress_increment_under_concentrated_load(P, x, y, z):
    ''' Return the increment in the vertical stress of a point inside an
        homogeneous and elastic soil due to a load on the surface.

        :param P: value of the load.
        :param x: x-coordinate of the point relative to the loaded point.
        :param y: y-coordinate of the point relative to the loaded point.
        :param z: z-coordinate of the point relative to the loaded point.
    '''
    retval= 0.0
    r= math.sqrt(x**2+y**2)
    if(z!=0.0):
        denom= 2.0*math.pi*z**2*math.pow(1+(r/z)**2,5.0/2.0)
        retval= 3.0*P/denom
    else:
        if(r==0.0):
            retval= float('inf')
    return retval

def average_stress_increment_under_concentrated_load(P, x, y, z1, z2, zInc= 0.2):
    ''' Return the average increment in the vertical stress of a the points
        between z1 and z2.

        :param P: value of the load.
        :param x: x-coordinate of the point relative to the loaded point.
        :param y: y-coordinate of the point relative to the loaded point.
        :param z1: upper limit of the z-coordinate.
        :param z2: lower limit of the z-coordinate.
        :param zInc: size of the discretization.
    '''
    H= z2-z1
    nDivZ= int(math.ceil(H/zInc))
    retval= 0.0
    z= z1+zInc/2.0
    for i in range(0,nDivZ):
        retval+= stress_increment_under_concentrated_load(P, x, y, z)
        z+= zInc
    retval/= nDivZ
    return retval

def stress_increment_under_rectangular_loaded_area(q, B, L, x, y, z, eSize= 0.1):
    ''' Return the increment in the vertical stress of a point inside an
        homogeneous and elastic soil due to a load distributed on
        a rectangular area.

        :param q: pressure on the loaded area.
        :param B: dimension of the rectangular area in direction x.
        :param L: dimension of the rectangular area in direction y.
        :param eSize: length of the side for the discretization.
        :param x: x-coordinate of the point relative to the rectangle center.
        :param y: y-coordinate of the point relative to the rectangle center.
        :param z: z-coordinate of the point relative to the rectangle center.
    '''
    nDivX= int(math.ceil(B/eSize))
    nDivY= int(math.ceil(L/eSize))
    xLoad= -B/2.0+eSize/2.0 
    yLoad= -L/2.0+eSize/2.0
    P= q*eSize**2 # punctual load.
    retval= 0
    for i in range(0,nDivX):
        for j in range(0,nDivY):
            retval+= stress_increment_under_concentrated_load(P, x-xLoad, y-yLoad, z)
            yLoad+= eSize
        xLoad+= eSize
        yLoad= -L/2.0+eSize/2.0 # from square one.
    return retval

def average_stress_increment_under_rectangular_loaded_area(q, B, L, x, y, z1, z2, eSize= 0.1, zInc= 0.2):
    ''' Return the average increment in the vertical stress of the
        points between z1 and z2 inside an homogeneous and elastic soil 
        due to a load distributed on a rectangular area.

        :param q: pressure on the loaded area.
        :param B: dimension of the rectangular area in direction x.
        :param L: dimension of the rectangular area in direction y.
        :param x: x-coordinate of the point relative to the rectangle center.
        :param y: y-coordinate of the point relative to the rectangle center.
        :param z: z-coordinate of the point relative to the rectangle center.
        :param z1: upper limit of the z-coordinate.
        :param z2: lower limit of the z-coordinate.
        :param zInc: size of the discretization.
    '''
    H= z2-z1
    nDivZ= int(math.ceil(H/zInc))
    retval= 0.0
    z= z1+zInc/2.0
    for i in range(0,nDivZ):
        retval+= stress_increment_under_rectangular_loaded_area(q, B, L, x, y, z, eSize= 0.01)
        z+= zInc
    retval/= nDivZ
    return retval


class ConcentratedLoad(object):
    ''' Concentrated load.

    :ivar loadedPoint: position of the load.
    '''
    def __init__(self, p):
        ''' Constructor.

        :param p: position of the load.
        '''
        self.loadedPoint= p
    
    def getStressIncrement(self, P, points):
        ''' Return the increment in the vertical stress for the points inside
            an homogeneous and elastic soil due to a concentrated load.

        :param P: value of the concentrated load.
        :param points: points whose stress increment will be computed.
        '''
        retval= list()
        xLoad= self.loadedPoint.x; yLoad= self.loadedPoint.y; zLoad= self.loadedPoint.z
        for p in points:
            pLoad= stress_increment_under_concentrated_load(P, p.x-xLoad, p.y-yLoad, p.z-zLoad)
            retval.append(pLoad)
        return retval

class QuadLoadedArea(object):
    ''' Four-sided polygon under uniform pressure.

    :ivar vertices: polygon vertices.
    '''
    def __init__(self, p1, p2, p3, p4):
        ''' Constructor.

        :param p1: first vertex.
        :param p2: second vertex.
        :param p3: third vertex.
        :param p4: fourth vertex.
        '''
        self.vertices= [p1, p2, p3, p4]

    def getSamplePoints(self, eSize):
        ''' Return the points uniformly distributed along the surface.

        :param eSize: length of the side for the discretization.
        '''
        def Ni(xi, eta):
            ''' Shape functions.'''
            return [0.25*(1-xi)*(1-eta),
                    0.25*(1+xi)*(1-eta),
                    0.25*(1+xi)*(1+eta),
                    0.25*(1-xi)*(1+eta)]
        def getIntervalCenters(n):
            ''' Get the centers of the intervals in natural coordinates.

            :param n: number of intervals.
            '''
            sz= 2.0/n
            xi= np.linspace(start= -1, stop= 1, num= n+1, endpoint= True)
            return [ x+sz/2.0 for x in xi[:-1]] # Centers of the intervals.       
        avgWidth= (self.vertices[0].dist(self.vertices[1])+self.vertices[2].dist(self.vertices[3]))/2.0
        avgLength= (self.vertices[0].dist(self.vertices[3])+self.vertices[1].dist(self.vertices[2]))/2.0
        nDivWidth= int(math.ceil(avgWidth/eSize))
        nDivLength= int(math.ceil(avgLength/eSize))
        xi_i= getIntervalCenters(n= nDivWidth)
        eta_i= getIntervalCenters(n= nDivLength)
        retval= list()
        for xi in xi_i:
            for eta in eta_i:
                ni= Ni(xi= xi, eta= eta)
                x= 0; y= 0; z= 0
                for shape, vertex in zip(ni, self.vertices):
                    x+= shape*vertex.x
                    y+= shape*vertex.y
                    z+= shape*vertex.z
                retval.append(geom.Pos3d(x,y,z))
        return retval

    def getStressIncrement(self, q, points, eSize):
        ''' Return the increment in the vertical stress for the points inside
            an homogeneous and elastic soil due to a load distributed on
            a rectangular area.

            :param q: pressure on the loaded area.
            :param eSize: length of the side for the discretization.
            :param points: points whose stress increment will be computed.
        '''
        loadedPoints= self.getSamplePoints(eSize)
        area= geom.Polygon3d(self.vertices).getArea()
        tributaryArea= area/len(loadedPoints)
        P= q*tributaryArea # punctual load.
        retval= list()
        for p in points:
            pLoad= 0.0
            for loadedPoint in loadedPoints:
                xLoad= loadedPoint.x; yLoad= loadedPoint.y; zLoad= loadedPoint.z
                pLoad+= stress_increment_under_concentrated_load(P, p.x-xLoad, p.y-yLoad, p.z-zLoad)
            retval.append(pLoad)
        return retval
    
