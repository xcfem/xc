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
    if(z>=0.0): # The point is above the loaded point.
        retval= 0.0
    else: # z < 0.0
        denom= 2.0*math.pi*z**2*math.pow(1+(r/-z)**2,5.0/2.0)
        retval= 3.0*P/denom
    return retval

def radial_stress_increment_under_concentrated_load(P, x, y, z, eta= 1.0):
    ''' Return the increment in the radial stress of a point inside an
        homogeneous and elastic soil due to punctual load in the surface,
        according to equation 11-20a of Bowles book).

    :param P: value of the load.
    :param x: x-coordinate of the point relative to the loaded point.
    :param y: y-coordinate of the point relative to the loaded point.
    :param z: z-coordinate of the point relative to the loaded point.
    :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see commentaries
               in Bowles book (page 633).
    '''
    retval= 0.0
    r= max(math.sqrt(x**2+y**2), 1e-6)
    if(z<0.0): # The point is under the loaded point.
        R= math.sqrt(r**2+z**2)
        denom= R*(R-z) # Z oriented upwards in function.
        retval= P/2.0/math.pi*(3*r**2*z/math.pow(R,5)-(1-2*eta)/denom)
    return retval

def stress_increment_vector_under_concentrated_load(P, x, y, z, eta= 1.0):
    ''' Return the increment in the radial stress of a point inside an
        homogeneous and elastic soil due to punctual load in the surface.

    :param P: value of the load.
    :param x: x-coordinate of the point relative to the loaded point.
    :param y: y-coordinate of the point relative to the loaded point.
    :param z: z-coordinate of the point relative to the loaded point.
    :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see commentaries
               in Bowles book (page 633).
    '''
    # Compute vertical and radial stresses.
    sigma_z= vertical_stress_increment_under_concentrated_load(P, x, y, z)
    sigma_r= radial_stress_increment_under_concentrated_load(P, x, y, z, eta)
    r= max(math.sqrt(x**2+y**2), 1e-6)
    return geom.Vector3d(sigma_r*abs(x)/r, sigma_r*abs(y)/r, sigma_z)

class ConcentratedLoad(object):
    ''' Concentrated load.

    :ivar loadedPoint: position of the load.
    '''
    def __init__(self, p):
        ''' Constructor.

        :param p: position of the load.
        '''
        self.loadedPoint= p
    
    def getVerticalStressIncrement(self, P, points):
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
    
    def getStressIncrement(self, P, points, eta= 1.0):
        ''' Return the vector increment in the vertical stress for the points
            inside an homogeneous and elastic soil due to a concentrated load.

        :param P: value of the concentrated load.
        :param points: points whose stress increment will be computed.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see commentaries
                   in Bowles book (page 633).
        '''
        retval= list()
        xLoad= self.loadedPoint.x; yLoad= self.loadedPoint.y; zLoad= self.loadedPoint.z
        for p in points:
            pLoad= stress_increment_vector_under_concentrated_load(P= P, x=(p.x-xLoad), y= (p.y-yLoad), z= (p.z-zLoad), eta= eta)
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

    def getVerticalStressIncrement(self, q, points, eSize):
        ''' Return the increment in the vertical stress for the points inside
            an homogeneous and elastic soil due to a load distributed on
            this quadrilateral area.

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
    
    def getStressIncrement(self, q, points, eSize, eta= 1.0):
        ''' Return the increment in the vertical stress for the points inside
            an homogeneous and elastic soil due to a load distributed on
            this quadrilateral area.

        :param q: pressure on the loaded area.
        :param eSize: length of the side for the discretization.
        :param points: points whose stress increment will be computed.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    commentaries in Bowles book (page 633).
        '''
        loadedPoints= self.getSamplePoints(eSize)
        area= geom.Polygon3d(self.vertices).getArea()
        tributaryArea= area/len(loadedPoints)
        P= q*tributaryArea # punctual load.
        retval= list()
        for p in points:
            pLoad= geom.Vector3d(0.0,0.0,0.0)
            for loadedPoint in loadedPoints:
                xLoad= loadedPoint.x; yLoad= loadedPoint.y; zLoad= loadedPoint.z
                pLoad+= stress_increment_vector_under_concentrated_load(P, p.x-xLoad, p.y-yLoad, p.z-zLoad)
            retval.append(pLoad)
        return retval
    
