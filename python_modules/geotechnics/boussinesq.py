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
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy as np
import math
import geom
import xc
from geom_utils import tensor_transformation as tt

class BoussinesqStresses(object):
    ''' Values of Boussinesq stresses in a point.

    :ivar sigma_zz: vertical component of the stress.
    :ivar sigma_rr: radial component of the stress.
    :ivar sigma_thth: tangential component of the stress.
    :ivar tau_rz: shear (r,z) component of the stress.
    :ivar radiusLowerBound: lower limit to the radius of the point (x^2+y^2+z^2) to avoid
                             almost infinite values of the pressure.
    '''
    radiusLowerBound= 0.5
    
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
            R= max(math.sqrt(R2), self.radiusLowerBound)
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


class BoussinesqLoad(object):
    ''' Base class for object that compute soil stresses using the 
        Boussinesq equations.
    '''

    def computeElementOrientation(self, elements):
        ''' Compute the orientation of the elements with respect to this
        load.

        :param elements: elements to compute the orientation of.
        '''
        # Get the centroid of the load.
        loadCentroid= self.getCentroid()
        # Get loaded points.
        loadedPoints= list()
        unitVectors= list()
        for e in elements:
            pos= e.getPosCentroid(True)
            loadedPoints.append(pos) # Append loaded point.
            # Compute normal vector 
            kVector= e.getKVector3d(True)
            orientation= kVector.dot(loadCentroid-pos)
            if(orientation>0.0): # pressure on the "positive" side of the element.
                unitVectors.append(kVector)
            else: # pressure on the "negative" side of the element.
                unitVectors.append(-kVector)
        return loadedPoints, unitVectors
    
    def computeElementalLoads(self, elements, stressVectors, delta):
        ''' Compute loads on elements from the stress vectors passed as
            parameter.

        :param elements: elements to compute the pressure on.
        :param stressVectors: value of the stress vector (cartesian coord.)
                              on the centroid of each element.
        :param delta: friction angle between the soil and the element material.
        '''
        tanDelta= math.tan(delta)
        retval= list()
        for e, stressVector in zip(elements, stressVectors):
            iVector= e.getIVector3d(True)
            jVector= e.getJVector3d(True)
            kVector= e.getKVector3d(True)
            # Normal pressure.
            normalPressure= stressVector.dot(kVector)
            maxTangentPressure= abs(tanDelta*normalPressure)
            # Pressure parallel to i vector.
            tangentIPressure= stressVector.dot(iVector)
            if(abs(tangentIPressure)>maxTangentPressure):
                tangentIPressure= math.copysign(maxTangentPressure, tangentIPressure)
            # Pressure parallel to j vector.
            tangentJPressure= stressVector.dot(jVector)
            if(abs(tangentJPressure)>maxTangentPressure):
                tangentJPressure= math.copysign(maxTangentPressure, tangentJPressure)
            # Append the computed values to the returned list.
            retval.append([e, tangentIPressure, tangentJPressure, normalPressure])
        return retval
    
    def computePressuresOnElements(self, elements, eta= 1.0, delta= 0.0):
        ''' Compute pressures due to this load on the elements argument.

        :param elements: elements to compute the pressure on.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    commentaries in Bowles book (page 633).
        :param delta: friction angle between the soil and the element material.
        '''
        retval= list()
        if(not self.isZero()): # Don't waste time if load is zero.
            # Compute element orientation with respect to this load.
            loadedPoints, unitVectors= self.computeElementOrientation(elements= elements)
            # Compute the pressure values.
            stressVectors= self.getStressIncrement(points= loadedPoints, unitVectorDirs= unitVectors)
            # Compute loads on elements.
            retval= self.computeElementalLoads(elements= elements, stressVectors= stressVectors, delta= delta)
        return retval
    

    def appendLoadToCurrentLoadPattern(self, elements, eta= 1.0, delta= 0.0):
        ''' Append this load to the current load pattern.

        :param elements: elements to apply the load on.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    commentaries in Bowles book (page 633).
        :param delta: friction angle between the soil and the element material.
        '''
        pressures= self.computePressuresOnElements(elements= elements, eta= eta, delta= delta)
        for pData in pressures:
            e= pData[0]
            e.vector3dUniformLoadLocal(xc.Vector([pData[1],pData[2],pData[3]]))

class ConcentratedLoad(BoussinesqLoad):
    ''' Concentrated load.

    :ivar loadedPoint: position of the vertical load.
    '''
    def __init__(self, p, Q):
        ''' Constructor.

        :param p: position of the load.
        :param Q: load value.
        '''
        self.loadedPoint= p
        self.Q= Q
    
    def isZero(self, tol= 1e-6):
        ''' Return true if the maximum load is smaller that tol.'''
        return (abs(self.Q)<tol)
    
    def getStressIncrement(self, points, unitVectorDirs, eta= 1.0):
        ''' Return the vector increment in the soil stress for the points
            inside an homogeneous and elastic soil due to a concentrated load.

        :param points: points whose stress increment will be computed.
        :param unitVectorDirs: stress direction vectors (must be unit vectors) 
                               corresponding to the points.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    commentaries in Bowles book (page 633).
        '''
        retval= list()
        xLoad= self.loadedPoint.x; yLoad= self.loadedPoint.y; zLoad= self.loadedPoint.z
        for p, vDir in zip(points, unitVectorDirs):
            bs= BoussinesqStresses(P= self.Q, x=(p.x-xLoad), y= (p.y-yLoad), z= (p.z-zLoad), eta= eta)
            stressVector= bs.getCartesianStressVector(unitVectorDir= vDir)
            retval.append(stressVector)
        return retval

    def getCentroid(self):
        ''' Return the position of the load centroid.'''
        return self.loadedPoint
    
class LinearLoad(BoussinesqLoad):
    ''' Polyline under vertical linear load.

    :ivar segment: segment representing the load position.
    :ivar loadValues: load values at the segment endpoints.
    '''
    def __init__(self, segment, loadValues, eSize):
        ''' Constructor.

        :param segment: 3D segment representing the load position.
        :param loadValues: load values at the segment endpoints.
        :param eSize: length of the side for the discretization.
        '''
        self.segment= segment
        self.loadValues= loadValues
        self.eSize= eSize

    def getCentroid(self):
        ''' Return the position of the load centroid.'''
        return self.segment.getMidPoint()
    
    def isZero(self, tol= 1e-6):
        ''' Return true if the maximum load is smaller that tol.'''
        retval= True
        for lv in self.loadValues:
            if(abs(lv)>tol):
                retval= False
                break
        return retval
    
    def getSamplePoints(self):
        ''' Return the points uniformly distributed along the line.'''
        # Compute positions along the polyline.
        L= self.segment.getLength()
        numParts= int(math.ceil(L/self.eSize))
        points= self.segment.Divide(numParts)
        # Compute load values for each position.
        origin= points[0]
        slope= (self.loadValues[1]-self.loadValues[0])/L
        retval= list()
        # First sample point.
        retval.append((origin, self.loadValues[0]*self.eSize/2.0))
        # Intermediate sample points.
        for p in points[1:-1]:
            dist= p.dist(origin)
            Q= (slope*dist+self.loadValues[0])*self.eSize # load value.
            retval.append((p, Q))
        # Last sample point
        Q= self.loadValues[1]*self.eSize/2.0
        retval.append((points[-1], Q))
        return retval
    
    def getStressIncrement(self, points, unitVectorDirs, eta= 1.0):
        ''' Return the increment in the vertical stress for the points inside
            an homogeneous and elastic soil due to a load distributed on
            this quadrilateral area.

        :param points: points whose stress increment will be computed.
        :param unitVectorDirs: stress direction vectors (must be a unit vector) 
                               corresponding to the points.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    commentaries in Bowles book (page 633).
        '''
        loadedPoints= self.getSamplePoints()
        retval= list()
        for p, vDir in zip(points, unitVectorDirs):
            stressVector= geom.Vector3d(0.0,0.0,0.0)
            for loadedPoint in loadedPoints:
                position= loadedPoint[0]
                load= loadedPoint[1]
                xLoad= position.x; yLoad= position.y; zLoad= position.z
                bs= BoussinesqStresses(P= load, x=(p.x-xLoad), y= (p.y-yLoad), z= (p.z-zLoad), eta= eta)
                stressVector+= bs.getCartesianStressVector(unitVectorDir= vDir)
            retval.append(stressVector)
        return retval
        
class QuadLoadedArea(BoussinesqLoad):
    ''' Four-sided polygon under vertical uniform pressure.

    :ivar vertices: polygon vertices.
    :ivar q: pressure on the loaded area.
    :ivar eSize: length of the side for the discretization.
    '''
    def __init__(self, p1, p2, p3, p4, q, eSize):
        ''' Constructor.

        :param p1: first vertex.
        :param p2: second vertex.
        :param p3: third vertex.
        :param p4: fourth vertex.
        :param q: pressure on the loaded area.
        :param eSize: length of the side for the discretization.
        '''
        self.vertices= [p1, p2, p3, p4]
        self.q= q
        self.eSize= eSize

    def isZero(self, tol= 1e-6):
        ''' Return true if the load is smaller that tol.'''
        return (abs(self.q)<tol)

    def getPolygon(self):
        ''' Return the polygon whose vertices are the those of the
            loaded area.'''
        return geom.Polygon3d(self.vertices)

    def getCentroid(self):
        ''' Return the position of the load centroid.'''
        return self.getPolygon().getCenterOfMass()
    
    def getSamplePoints(self):
        ''' Return the points uniformly distributed along the surface.'''
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
        nDivWidth= int(math.ceil(avgWidth/self.eSize))
        nDivLength= int(math.ceil(avgLength/self.eSize))
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

    def getStressIncrement(self, points, unitVectorDirs, eta= 1.0):
        ''' Return the increment in the vertical stress for the points inside
            an homogeneous and elastic soil due to a load distributed on
            this quadrilateral area.

        :param points: points whose stress increment will be computed.
        :param unitVectorDirs: stress direction vectors (must be a unit vector) 
                               corresponding to the points.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    commentaries in Bowles book (page 633).
        '''
        loadedPoints= self.getSamplePoints()
        area= geom.Polygon3d(self.vertices).getArea()
        tributaryArea= area/len(loadedPoints)
        P= self.q*tributaryArea # punctual load.
        retval= list()
        for p, vDir in zip(points, unitVectorDirs):
            stressVector= geom.Vector3d(0.0,0.0,0.0)
            for loadedPoint in loadedPoints:
                xLoad= loadedPoint.x; yLoad= loadedPoint.y; zLoad= loadedPoint.z
                bs= BoussinesqStresses(P= P, x=(p.x-xLoad), y= (p.y-yLoad), z= (p.z-zLoad), eta= eta)
                stressVector+= bs.getCartesianStressVector(unitVectorDir= vDir)
            retval.append(stressVector)
        return retval



        

    
