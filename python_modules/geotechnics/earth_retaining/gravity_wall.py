# -*- coding: utf-8 -*-
''' Nonlinear non-linear spring model to analyze pile wall structutres.

References: 

[1] Chapter 4-3 of Foundation Analysis and Design, Ed. 5 by Joseph E. Bowles.
[2] Brinch Hansen. A general formula for bearing capacity. The Danish Geotechnical Institute. Bulletin 11. Copenhagen 1961.
[3] Guía de cimentaciones en obras de carretera. Ministerio de Fomento (spain). 2002 (https://books.google.ch/books?id=a0eoygAACAAJ).
[4] Eurocódigo 7. Proyecto geotécnico. Parte 1: reglas generales.
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import sys
import math
import geom
import numpy as np
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
        # Put the backfill contact surfaces in the right direction.
        self.backfillContactSurface= list()
        for (s, soil) in backfillContactSurface:
            upsideDown= (s.getFromPoint().y>s.getToPoint().y)
            if(upsideDown):
                s.swap()
            self.backfillContactSurface.append((s, soil))
        self.passivePressureContactSurface= passivePressureContactSurface
        self.footingConctactSurface= footingContactSurface
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
        contactSegments, soils= zip(*self.backfillContactSurface)
        firstSegment= contactSegments[0]
        retval= max(firstSegment.getFromPoint().y, firstSegment.getToPoint().y)
        for cs in contactSegments[1:]:
            retval= max(cs.getFromPoint().y, cs.getToPoint().y)
        return retval

    def getEarthPressureSVS(self, backfillSlope, k_delta= 2/3.0):
        ''' Return the sliding vector system corresponding to earth pressure
            on the wall.

        :param backfillSlope: slope of the backfill surface.
        :param k_delta: factor to apply to the soil friction angle to get
                        the friction angle between the backfill and the wall.
        '''
        wallPolygonCentroid= self.contour.getCenterOfMass()

        ## Earth pressure.
        retval= geom.SlidingVectorsSystem2d()
        sg_v0= 0.0
        for (s, soil) in self.backfillContactSurface:
            normal= s.getNormal().normalized()
            soilLayerDepth= abs(s.getFromPoint().y-s.getToPoint().y)
            # Put the normal in the right direction.
            earthPressureDir= wallPolygonCentroid-s.getMidPoint()
            if(normal.dot(earthPressureDir)<0):
                normal= -normal
            # angle of the back of the retaining wall (radians)
            a= math.pi/2.0-s.getXAxisAngle()
            delta= k_delta*soil.getDesignPhi()
            # Pressure at the top of the soil layer.
            e0= soil.ea_coulomb(sg_v0,a,backfillSlope, d= delta, designValue= True)
            # Pressure at the bottom of the soil layer.
            sg_v= sg_v0+soil.rho*g*soilLayerDepth
            e= soil.ea_coulomb(sg_v,a,backfillSlope, d= delta, designValue= True)
            stressArea= geom.Polygon2d()
            p0= s.getToPoint()
            stressArea.appendVertex(p0)
            stressArea.appendVertex(p0-e0*normal)
            p= s.getFromPoint()
            stressArea.appendVertex(p-e*normal)
            stressArea.appendVertex(p)
            area= stressArea.getArea()
            if(abs(area)>0.0):
                # area2= (e0+e)/2.0*s.getLength()
                E= area*normal
                C= stressArea.getCenterOfMass()
                retval+= geom.SlidingVectorsSystem2d(geom.SlidingVector2d(C,E))
                #stressAreas.append(stressArea)
            sg_v0= sg_v
        return retval

    def getUniformLoadSVS(self, uniformLoad, backfillSlope, k_delta):
        ''' Return the sliding vector system corresponding to a uniform load
            on the backfill surface.

        :param uniformLoad: uniform load on the backfill surface.
        :param backfillSlope: slope of the backfill surface.
        :param k_delta: factor to apply to the soil friction angle to get
                        the friction angle between the backfill and the wall.
        '''    
        wallPolygonCentroid= self.contour.getCenterOfMass()
        retval= geom.SlidingVectorsSystem2d()
        for (s, soil) in self.backfillContactSurface:
            normal= s.getNormal().normalized()
            soilLayerDepth= abs(s.getFromPoint().y-s.getToPoint().y)
            # Put the normal in the right direction.
            earthPressureDir= wallPolygonCentroid-s.getMidPoint()
            if(normal.dot(earthPressureDir)<0):
                normal= -normal
            # angle of the back of the retaining wall (radians)
            a= math.pi/2.0-s.getXAxisAngle()
            delta= k_delta*soil.getDesignPhi()
            e0= soil.eq(q= uniformLoad, a= a, b= backfillSlope, d= delta, designValue= True)
            e= soil.eq(q= uniformLoad, a= a, b= backfillSlope, d= delta, designValue= True)
            stressArea= geom.Polygon2d()
            p0= s.getToPoint()
            stressArea.appendVertex(p0)
            stressArea.appendVertex(p0-e0*normal)
            p= s.getFromPoint()
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
        foundationSegment= self.footingConctactSurface[0]
        contactPlane= foundationSegment.getSupportLine()
        zml= svs.zeroMomentLine() # Zero moment line.
        p= contactPlane.getIntersection(zml)[0] # Intersection with
                                                # foundation plane.
        # Compute the eccentricity.
        foundationCenter= foundationSegment.getMidPoint()
        eVector= p-foundationCenter
        rProj= R.dot(eVector)
        e= eVector.getModulus()
        if(rProj<0): # the sliding vector system resultant stabilizes the wall
            e= -e
        return e, zml, p
        
    def getClassicOverturningSafetyFactorForLoadCombination(self, pointOfRotation, svsList):
        ''' Return the overturning safety factor according to the classic
            formula: Mestab/Mdestab.

        :param pointOfRotation: point of rotation
        :param svsList: list of sliding vector systems action on the retaining
                        wall corresponding to a load combination. The first one
                        must be stabilising.
        '''
        retval= None
        if(svsList):
            Mstab= svsList[0].getMoment(pointOfRotation)
            signStab= np.sign(Mstab)
            Mdestab= 0.0
            for svs in svsList[1:]:
                M= svs.getMoment(pointOfRotation)
                if(np.sign(M)== signStab):
                    Mstab+= M
                else:
                    Mdestab+= M
            retval= abs(Mstab/Mdestab)
        return retval
        
    def getOverturningSafetyFactor(self, svs, gammaRv= 1.0):
        ''' Return the overturning safety factor.

        :param svs: sliding vector system that represents the forces acting on
                    the retaining wall.
        :param gammaRv: partial safety factor for the soil resistance.
        '''
        foundationSegment= self.footingConctactSurface[0]
        foundationCenter= foundationSegment.getMidPoint()
        svs= svs.reduceTo(foundationCenter)
        R= svs.getResultant()
        M= svs.getMoment()
        e, zml, p= self.getEccentricity(svs)
        b= foundationSegment.getLength() # Foundation width.
        if(e>0):
          F= b/(3*(e)*gammaR)
        else:
          F= 10.0
        return F, R, zml, p
        
    def getSlidingSafetyFactor(self, svs, k_delta= 2/3.0):
        ''' Return the safety factor against sliding.

        :param svs: sliding vector system that represents the forces acting on 
                    the retaining wall.
        :param k_delta: factor to apply to the soil friction angle to get
                        the friction angle between the backfill and the wall.
                        Its value is normally 1.0 for cast-in-situ concrete 
                        foundations and 2/3 for smooth precast foundations.
        '''
        foundationSegment, foundationSoil= self.footingConctactSurface
        foundationCenter= foundationSegment.getMidPoint()
        svs= svs.reduceTo(foundationCenter)
        R= svs.getResultant()
        totalHorizontalThrust= R.x
        delta= k_delta*foundationSoil.getDesignPhi()
        designResistance= R.y*math.tan(delta)
        if(abs(totalHorizontalThrust)>0.0):
            F= abs(designResistance/totalHorizontalThrust)
        else:
            F= 10.0
        return F

    def getPressureOnFooting(self, svs, D, q= 0.0, Leff= 1.0, NgammaCoef= 1.5, gammaRv= 1.4):
        ''' Return the pressure on the contact surface with the soil.

        :param svs: sliding vector system that represents the forces acting on 
                    the retaining wall.
        :param D: foundation depth.
        :param q: overburden load.
        :param Leff: Length of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param NgammaCoef: 1.5 in reference [1], 1.8 in reference 2 
                           and 2 in reference 3
        :param gammaRv: partial safety factor for the soil resistance.
        '''
        foundationSegment, foundationSoil= self.footingConctactSurface
        foundationCenter= foundationSegment.getMidPoint()
        svs= svs.reduceTo(foundationCenter)
        R= svs.getResultant()
        M= svs.getMoment()
        b= foundationSegment.getLength() # Foundation width.
        e, zml, p= self.getEccentricity(svs)
        bReduced= 2*(b/2.0-e) # see figure 12 in page 44 (8 in the PDF) of reference 2.
        loadedArea= bReduced*Leff
        normal= foundationSegment.getNormal().normalized()
        # Put the normal in the right direction.
        if(normal.y<0.0):
            normal= -normal

        qEd= R.dot(normal)/loadedArea
        q+= foundationSoil.gamma()*D # dverburden at foundation base
        q_ult= foundationSoil.qu(q= q, D= D, Beff= bReduced, Leff= Leff, Vload= -R.y, HloadB= R.x, HloadL= 0.0, NgammaCoef= NgammaCoef)
        qRd= q_ult/gammaRv
        F= abs(qEd)/qRd
        return F, qEd, qRd
    
