# -*- coding: utf-8 -*-
''' Boussinesq equation.

   Code based on the chapter 11 of the book "Fondation Analysis 
   and Design". Fifth Edition. Joseph E. Bowles. McGraw-Hill 1982
   ISBN= 9780070067707
   LCCN= lc81013649
   url= https://books.google.es/books?id=eNtRAAAAMAAJ

   See also S.P. Timoshenko, Theory of Elasticity, paragraph 123 equation 201
   McGraw-Hill 1987
   ISBN 9780070647206
   LCCN 69013617
   url= https://books.google.es/books?id=GOnFQgAACAAJ
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
from geom_utils import tensor_transformation as tt

class BoussinesqStresses(object):
    ''' Values of Boussinesq stresses in a point.

    :ivar sigma_zz: vertical component of the stress.
    :ivar sigma_rr: radial component of the stress.
    :ivar sigma_thth: tangential component of the stress.
    :ivar tau_rz: shear (r,z) component of the stress.
    '''
    
    def __init__(self, P, x, y, z, eta= 1.0):
        ''' Constructor.

        :param P: value of the load.
        :param x: x-coordinate of the point relative to the loaded point.
        :param y: y-coordinate of the point relative to the loaded point.
        :param z: z-coordinate of the point relative to the loaded point.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    commentaries in Bowles book (page 633).
        '''
        self.setup(P= P, x= x, y= y, z= z, eta= eta)
            
    def setup(self, P, x, y, z, eta= 1.0):
        ''' Computes member values.

        :param P: value of the load.
        :param x: x-coordinate of the point relative to the loaded point.
        :param y: y-coordinate of the point relative to the loaded point.
        :param z: z-coordinate of the point relative to the loaded point.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    commentaries in Bowles book (page 633).
        '''
        # Store the coordinates for later use.
        self.x= x
        self.y= y
        self.z= z
        # Compute stresses.
        self.sigma_zz= 0.0
        self.sigma_rr= 0.0
        self.sigma_thth= 0.0
        self.tau_rz= 0.0
        if(z<0.0): # The point is under the loaded point.
            Z= -z # Z oriented downwards in this function.
            r2= x**2+y**2
            r= math.sqrt(r2)
            R2= r2+Z**2
            R= math.sqrt(R2)
            R5= R**5
            P_twoPi= P/(2.0*math.pi)
            P3_twoPi_R5= 3.0*P_twoPi/R5
            self.sigma_zz= P3_twoPi_R5*Z**3
            self.sigma_rr= P_twoPi*(3*r2*Z/R5-(1-2*eta)/(R*(R+Z)))
            self.sigma_thth= P_twoPi*(1-2*eta)/R2*(R/(R+Z)-Z/R)
            self.tau_rz= P3_twoPi_R5*r*Z**2

    def getStressTensorCylindrical(self):
        ''' Return the stress tensor expressed in cylindrical coordinates.'''
        return np.array([[self.sigma_rr, 0, self.tau_rz],
                         [0, self.sigma_thth, 0],
                         [self.tau_rz, 0, self.sigma_zz]])
    
    def getStressTensorCartesian(self):
        ''' Return the stress tensor expressed in cartesian coordinates.'''
        cylTensor= self.getStressTensorCylindrical()
        theta= math.atan2(self.y, self.x)
        return tt.cylindrical_to_cartesian(tensorCylindrical=cylTensor, theta= theta)
    def getCartesianStressVector(self, unitVectorDir):
        ''' Return the stress vector for the direction argument.

        :param unitVectorDir: stress direction vector (must be a unit vector) 
                              expressed in cartesian coordinates.
        '''
        tmp= self.getStressTensorCartesian().dot(np.array([unitVectorDir.x, unitVectorDir.y, unitVectorDir.z]))
        return geom.Vector3d(float(tmp[0]), float(tmp[1]), float(tmp[2]))


def vertical_stress_increment_under_concentrated_load(P, x, y, z):
    ''' Return the increment in the vertical stress of a point inside an
        homogeneous and elastic soil due to a load on the surface.

    :param P: value of the load.
    :param x: x-coordinate of the point relative to the loaded point.
    :param y: y-coordinate of the point relative to the loaded point.
    :param z: z-coordinate of the point relative to the loaded point.
    '''
    return BoussinesqStresses(P= P, x= x, y= y, z= z).sigma_zz

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
    return BoussinesqStresses(P= P, x= x, y= y, z= z, eta= eta).sigma_rr

def tangential_stress_increment_under_concentrated_load(P, x, y, z, eta= 1.0):
    ''' Return the increment in the tangential stress of a point inside an
        homogeneous and elastic soil due to punctual load in the surface,
        according to equation 11-20a of Bowles book).

    :param P: value of the load.
    :param x: x-coordinate of the point relative to the loaded point.
    :param y: y-coordinate of the point relative to the loaded point.
    :param z: z-coordinate of the point relative to the loaded point.
    :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see commentaries
               in Bowles book (page 633).
    '''
    return BoussinesqStresses(P= P, x= x, y= y, z= z, eta= eta).sigma_thth

def stress_increment_vector_under_concentrated_load(P, x, y, z, unitVectorDir, eta= 1.0):
    ''' Return the increment in the radial stress of a point inside an
        homogeneous and elastic soil due to punctual load in the surface.

    :param P: value of the load.
    :param x: x-coordinate of the point relative to the loaded point.
    :param y: y-coordinate of the point relative to the loaded point.
    :param z: z-coordinate of the point relative to the loaded point.
    :param unitVectorDir: stress direction vector (must be a unit vector) 
                          expressed in cartesian coordinates.
    :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see commentaries
               in Bowles book (page 633).
    '''
    # Compute vertical and radial stresses.
    return BoussinesqStresses(P= P, x= x, y= y, z= z, eta= eta).getCartesianStressVector(unitVectorDir= unitVectorDir)

class ConcentratedLoad(object):
    ''' Concentrated load.

    :ivar loadedPoint: position of the load.
    '''
    def __init__(self, p):
        ''' Constructor.

        :param p: position of the load.
        '''
        self.loadedPoint= p
    
    def getStressIncrement(self, P, points, unitVectorDirs, eta= 1.0):
        ''' Return the vector increment in the vertical stress for the points
            inside an homogeneous and elastic soil due to a concentrated load.

        :param P: value of the concentrated load.
        :param points: points whose stress increment will be computed.
        :param unitVectorDirs: stress direction vectors (must be a unit vector) 
                               corresponding to the points.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    commentaries in Bowles book (page 633).
        '''
        retval= list()
        xLoad= self.loadedPoint.x; yLoad= self.loadedPoint.y; zLoad= self.loadedPoint.z
        for p, vDir in zip(points, unitVectorDirs):
            bs= BoussinesqStresses(P= P, x=(p.x-xLoad), y= (p.y-yLoad), z= (p.z-zLoad), eta= eta)
            stressVector= bs.getCartesianStressVector(unitVectorDir= vDir)
            retval.append(stressVector)
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

    def getPolygon(self):
        ''' Return the polygon whose vertices are the those of the
            loaded area.'''
        return geom.Polygon3d(self.vertices)

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

    def getStressIncrement(self, q, points, unitVectorDirs, eSize, eta= 1.0):
        ''' Return the increment in the vertical stress for the points inside
            an homogeneous and elastic soil due to a load distributed on
            this quadrilateral area.

        :param q: pressure on the loaded area.
        :param eSize: length of the side for the discretization.
        :param points: points whose stress increment will be computed.
        :param unitVectorDirs: stress direction vectors (must be a unit vector) 
                               corresponding to the points.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    commentaries in Bowles book (page 633).
        '''
        loadedPoints= self.getSamplePoints(eSize)
        area= geom.Polygon3d(self.vertices).getArea()
        tributaryArea= area/len(loadedPoints)
        P= q*tributaryArea # punctual load.
        retval= list()
        for p, vDir in zip(points, unitVectorDirs):
            stressVector= geom.Vector3d(0.0,0.0,0.0)
            for loadedPoint in loadedPoints:
                xLoad= loadedPoint.x; yLoad= loadedPoint.y; zLoad= loadedPoint.z
                bs= BoussinesqStresses(P= P, x=(p.x-xLoad), y= (p.y-yLoad), z= (p.z-zLoad), eta= eta)
                stressVector+= bs.getCartesianStressVector(unitVectorDir= vDir)
            retval.append(stressVector)
        return retval
    
