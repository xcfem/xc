# -*- coding: utf-8 -*-
''' Boussinesq equation.

   Code based on the chapter 11 of the book "Fondation Analysis 
   and Design". Fifth Edition. Joseph E. Bowles. McGraw-Hill 1982
   ISBN= 9780070067707
   LCCN= lc81013649
   url= https://books.google.es/books?id=eNtRAAAAMAAJ
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

def vertical_stress_increment_under_concentrated_load(P, x, y, z):
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
            pLoad= vertical_stress_increment_under_concentrated_load(P, p.x-xLoad, p.y-yLoad, p.z-zLoad)
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
                pLoad+= vertical_stress_increment_under_concentrated_load(P, p.x-xLoad, p.y-yLoad, p.z-zLoad)
            retval.append(pLoad)
        return retval
    
