# -*- coding: utf-8 -*-
''' Nonlinear non-linear spring model to analyze pile wall structutres.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import sys
import math
import geom
from scipy.constants import g
from misc_utils import log_messages as lmsg

class GravityWall(object):
    ''' Gravity wall design.'''

    def __init__(self, wallContour, backfillContactSurface, passivePressureContactSurface, footingContactSurface, wallUnitWeight=24.0e3):
        ''' Constructor.

        :param wallContour: Polygon2d object defining the contour of the gravity
                            wall.
        :param wallUnitWeight: weight per unit volume of the wall.
        :param backfillContactSurface: 2D polyline representing the contact of
                                       the backfill with the wall.
        :param passivePressureContactSurface: 2D segment representing the 
                                              contact of the wall in the
                                              passive wedge.
        :param footingContactSurface: contact surface of the footing with the
                                      soil.
        '''
        self.contour= wallContour
        self.backfillContactSurface= backfillContactSurface
        self.passivePressureContactSurface= passivePressureContactSurface
        self.footingConctacSurface= footingContactSurface
        self.wallUnitWeight= wallUnitWeight

    def getWallWeightSVS(self):
        ''' Return the sliding vector system corresponding to the self weight
            of the wall.
        '''
        wallPolygonCentroid= self.contour.getCenterOfMass()
        wallPolygonArea= self.contour.getArea()

        wallPolygonWeight= self.wallUnitWeight*wallPolygonArea
        return geom.SlidingVectorsSystem2d(geom.SlidingVector2d(wallPolygonCentroid, geom.Vector2d(0.0,-wallPolygonWeight)))

    def getBackfillContactSurfaceYMax(self):
        ''' Return the maximum ordinate of the backfill contact surface.'''
        retval= self.backfillContactSurface[0].y
        for p in self.backfillContactSurface[1:]:
            retval= max(retval, p.y)
        return retval

    def getEarthPressureSVS(self, backfillSoil, backfillSlope, delta):
        ''' Return the sliding vector system corresponding to earth pressure
            on the wall.

        :param backfillSoil: backfill soil.
        :param backfillSlope: slope of the backfill surface.
        :param delta: friction angle between the backfill and the wall.
        '''
        yMax= self.getBackfillContactSurfaceYMax()
        p0= self.backfillContactSurface[0]
        wallPolygonCentroid= self.contour.getCenterOfMass()

        ## Earth pressure.
        retval= geom.SlidingVectorsSystem2d()
        for p in self.backfillContactSurface[1:]:
            s= geom.Segment2d(p0,p)
            normal= s.getNormal().normalized()
            # Put the normal in the right direction.
            earthPressureDir= wallPolygonCentroid-s.getMidPoint()
            if(normal.dot(earthPressureDir)<0):
                normal= -normal
            # angle of the back of the retaining wall (radians)
            a= math.pi/2.0-s.getXAxisAngle()
            sg_v0= backfillSoil.rho*g*(yMax-p0.y)
            e0= backfillSoil.ea_coulomb(sg_v0,a,backfillSlope,delta)
            sg_v= backfillSoil.rho*g*(yMax-p.y)
            e= backfillSoil.ea_coulomb(sg_v,a,backfillSlope,delta)
            stressArea= geom.Polygon2d()
            stressArea.appendVertex(p0)
            stressArea.appendVertex(p0-e0*normal)
            stressArea.appendVertex(p-e*normal)
            stressArea.appendVertex(p)
            area= stressArea.getArea()
            if(abs(area)>0.0):
                # area2= (e0+e)/2.0*s.getLength()
                E= area*normal
                C= stressArea.getCenterOfMass()
                retval+= geom.SlidingVectorsSystem2d(geom.SlidingVector2d(C,E))
                #stressAreas.append(stressArea)
            p0= p
        return retval

    def getUniformLoadSVS(self, uniformLoad, backfillSoil, backfillSlope, delta):
        ''' Return the sliding vector system corresponding to a uniform load
            on the backfill surface.

        :param uniformLoad: uniform load on the backfill surface.
        :param backfillSoil: backfill soil.
        :param backfillSlope: slope of the backfill surface.
        :param delta: friction angle between the backfill and the wall.
        '''    
        p0= self.backfillContactSurface[0]
        wallPolygonCentroid= self.contour.getCenterOfMass()
        retval= geom.SlidingVectorsSystem2d()
        for p in self.backfillContactSurface[1:]:
            s= geom.Segment2d(p0,p)
            normal= s.getNormal().normalized()
            # Put the normal in the right direction.
            earthPressureDir= wallPolygonCentroid-s.getMidPoint()
            if(normal.dot(earthPressureDir)<0):
                normal= -normal
            # angle of the back of the retaining wall (radians)
            a= math.pi/2.0-s.getXAxisAngle()
            e0= backfillSoil.eq_bell(uniformLoad, a, backfillSlope, delta)
            e= backfillSoil.eq_bell(uniformLoad, a, backfillSlope, delta)
            stressArea= geom.Polygon2d()
            stressArea.appendVertex(p0)
            stressArea.appendVertex(p0-e0*normal)
            stressArea.appendVertex(p-e*normal)
            stressArea.appendVertex(p)
            area= stressArea.getArea()
            if(abs(area)>0.0):
                # area2= (e0+e)/2.0*s.getLength()
                E= area*normal
                C= stressArea.getCenterOfMass()
                retval+= geom.SlidingVectorsSystem2d(geom.SlidingVector2d(C,E))
                #stressAreas.append(stressArea)
            p0= p
        return retval

    def getPassivePressureSVS(self, soilThicknessOverToe, backfillSoil, backfillSlope, delta):
        ''' Return the sliding vector system corresponding to a uniform load
            on the backfill surface.

        :param soilThicknessOverToe: thicness of the soil over the wall toe.
        :param backfillSoil: backfill soil.
        :param backfillSlope: slope of the backfill surface.
        :param delta: friction angle between the backfill and the wall.
        '''
        # Passive pressure.
        retval= geom.SlidingVectorsSystem2d()
        s= self.passivePressureContactSurface
        p0= s.getFromPoint()
        p= s.getToPoint()
        if(p0.y>p.y):
            p, p0= p0, p
        normal= s.getNormal().normalized()
        # Put the normal in the right direction.
        wallPolygonCentroid= self.contour.getCenterOfMass()
        earthPressureDir= s.getMidPoint()-wallPolygonCentroid
        if(normal.dot(earthPressureDir)<0):
            normal= -normal
        a= math.pi/2.0-s.getXAxisAngle()
        toeYMax= p.y+soilThicknessOverToe
        sg_v0= backfillSoil.rho*g*(toeYMax-p0.y)
        e0= backfillSoil.ep_coulomb(sg_v0, a, backfillSlope, -delta)
        sg_v= backfillSoil.rho*g*(toeYMax-p.y)
        e= backfillSoil.ep_coulomb(sg_v, a, backfillSlope, -delta)
        stressArea= geom.Polygon2d()
        stressArea.appendVertex(p0)
        stressArea.appendVertex(p0-e0*normal)
        stressArea.appendVertex(p-e*normal)
        stressArea.appendVertex(p)
        area= stressArea.getArea()
        if(abs(area)>0.0):
            area2= (e0+e)/2.0*s.getLength()
            E= area*normal
            C= stressArea.getCenterOfMass()
            retval+= geom.SlidingVectorsSystem2d(geom.SlidingVector2d(C,-E))
        return retval

    def getEccentricity(self, svs):
        ''' Return the eccentricity of the resultant of the given sliding vector            system.

        :param svs: sliding vector system that represents the forces acting on
                    the retaining wall.
        '''
        R= svs.getResultant()

        #Overturning safety factor.
        contactPlane= self.footingConctacSurface.getSupportLine()
        zml= svs.zeroMomentLine() # Zero moment line.
        p= contactPlane.getIntersection(zml)[0] # Intersection with
                                                # foundation plane.
        # Compute the eccentricity.
        foundationCenter= self.footingConctacSurface.getMidPoint()
        eVector= p-foundationCenter
        rProj= R.dot(eVector)
        e= eVector.getModulus()
        if(rProj<0): # the sliding vector system resultant stabilizes the wall
            e= -e
        return e, zml, p
        
    def getOverturningSafetyFactor(self, svs, gammaR= 1.0):
        ''' Return the overturning safety factor.

        :param svs: sliding vector system that represents the forces acting on
                    the retaining wall.
        :param gammaR: partial resistance factor.
        '''
        foundationCenter= self.footingConctacSurface.getMidPoint()
        svs= svs.reduceTo(foundationCenter)
        R= svs.getResultant()
        M= svs.getMoment()
        e, zml, p= self.getEccentricity(svs)
        b= self.footingConctacSurface.getLength() # Foundation width.
        if(e>0):
          F= (3*(e)*gammaR)/b
        else:
          F= 0.01
        return F, R, zml, p
    
    def getPressureOnFooting(self, svs, foundationSoil):
        ''' Return the pressure on the contact surface with the soil.

        :param svs: sliding vector system that represents the forces acting on 
                    the retaining wall.
        :param foundationSoil: foundation soil.
        '''
        foundationCenter= self.footingConctacSurface.getMidPoint()
        svs= svs.reduceTo(foundationCenter)
        R= svs.getResultant()
        M= svs.getMoment()
        e, zml, p= self.getEccentricity(svs)
        b= self.footingConctacSurface.getLength() # Foundation width.
        bReduced= 2*(b/2.0+e)
        loadedArea= bReduced
        normal= self.footingConctacSurface.getNormal().normalized()
        # Put the normal in the right direction.
        if(normal.y<0.0):
            normal= -normal

        stress= R.dot(normal)/loadedArea
        uStress= foundationSoil.qu(q= 0.0,D= 0.5, Beff= bReduced, Leff= 5.0, Vload= -R.y, HloadB= R.x, HloadL= 0.0)
        F= abs(stress)/uStress
        return F, stress, uStress
    
    def getSlidingSafetyFactor(self, svs, foundationSoil):
        ''' Return the safety factor against sliding.

        :param svs: sliding vector system that represents the forces acting on 
                    the retaining wall.
        :param foundationSoil: foundation soil.
        '''
        foundationCenter= self.footingConctacSurface.getMidPoint()
        svs= svs.reduceTo(foundationCenter)
        R= svs.getResultant()
        V= R.y
        H= R.x
        if(abs(H)>0.0):
          F= abs(V*math.tan(foundationSoil.getDesignPhi())/H)
        else:
          F= 10.0
        return F
