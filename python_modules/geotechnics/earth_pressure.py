# -*- coding: utf-8 -*-
'''earth_pressure.py: earth pressures for retaining structures.'''

from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import sys
from geotechnics import frictional_soil as fs
from scipy import interpolate
from scipy import optimize
import numpy
import geom
from misc_utils import log_messages as lmsg

class RankineSoil(fs.FrictionalSoil):
    '''From Wikipedia: Rankine's theory, developed in 1857, is a stress field solution that predicts active and passive earth pressure. It assumes that the soil is cohesionless, the wall is frictionless, the soil-wall interface is vertical, the failure surface on which the soil moves is planar, and the resultant force is angled parallel to the backfill surface. The equations for active and passive lateral earth pressure coefficients are given below. Note that φ is the angle of shearing resistance of the soil and the backfill is inclined at angle β to the horizontal.

    :ivar phi:    internal friction angle of the soil
    :ivar beta:   angle of backfill with horizontal
    '''
    def __init__(self,phi,beta= 0.0,rho= 2100.0, rhoSat= None, gammaMPhi= 1.0):
        ''' Constructor.

        :param phi: internal friction angle of the soil
        :param beta: angle of backfill with horizontal
        :param rho: soil density.
        :param rhoSat: saturated density of the soil (mass per unit volume)
        :param gammaMPhi: (float) partial reduction factor for internal 
                          friction angle of the soil.
        '''
        super(RankineSoil,self).__init__(phi= phi, rho= rho, rhoSat= rhoSat, gammaMPhi= gammaMPhi)
        self.beta= beta
        
    def Ka(self):
        '''Returns Rankine's active earth pressure coefficient.'''
        cBeta= math.cos(self.beta)
        cPhi= math.cos(self.phi)
        r= math.sqrt(cBeta**2-cPhi**2)
        return cBeta*(cBeta-r)/(cBeta+r)
      
    def Kp(self):
        '''Returns Rankine's passive earth pressure coefficient.'''
        cBeta= math.cos(self.beta)
        cPhi= math.cos(self.phi)
        r= math.sqrt(cBeta**2-cPhi**2)
        return cBeta*(cBeta+r)/(cBeta-r)

    def getActivePressureAtDepth(self, z, waterTableDepth= 6.023e23):
        ''' Returns the active presure at depth z

        :param z: depth to compute the pressure.
        :param waterTableDepth: depth of the water table.
        '''
        if(z<=waterTableDepth):
            retval= self.gamma()*z*self.Ka()
        else:
            retval= self.gamma()*waterTableDepth
            retval+= self.submergedGamma()*(z-waterTableDepth)
            retval*= self.Ka()
        return retval
          
      

# Earth pressure according to Coulomb's Theory.
# This theory is valid if the backfill surface is plane and the wall-bacfill
# contact surface is also plane.
# Expressions taken from the books:
# @book{calavera2001muros,
#   title={Muros de contenci{\'o}n y muros de s{\'o}tano},
#   author={CALAVERA, J.A. and Ruiz, J.C. and Instituto Técnico de Materiales y Construcciones (Madrid)},
#   isbn={9788488764102},
#   url={https://books.google.ch/books?id=f41cOgAACAAJ},
#   year={2001},
#   publisher={INTEMAC INSTITUTO TECNICO DE MATERIALES Y CONSTRUCCIONES}
# # }
# @book{salas1971geotecnia,
#   title={Geotecnia y cimientos [por] J.A. Jim{\'e}nez Salas [y] J.L. de Justo Alpa{\~n}es},
#   author={Salas, J.A.J. and de Justo Alpa{\~n}es, J.L.},
#   url={https://books.google.ch/books?id=80tsHAAACAAJ},
#   year={1971},
#   publisher={Editorial Rueda}
# }

# Earth pressure coefficients.

def ka_coulomb(a,b,fi,d):
    '''Return the active earth pressure coefficient according to Coulomb theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param fi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).See Jiménez Salas, Geotecnia y Cimientos page 682.

    '''
    fSoil= fs.FrictionalSoil(fi)
    return fSoil.Ka_coulomb(a, b, d)

def kah_coulomb(a,b,fi,d):
    '''
    Return the horizontal component of the active earth pressure coefficient
    according to Coulomb's theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param fi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).
    '''
    fSoil= fs.FrictionalSoil(fi)
    return fSoil.Kah_coulomb(a, b, d)

def kav_coulomb(a,b,fi,d):
    '''
    Return the vertical component of the active earth pressure coefficient
    according to Coulomb's theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param fi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).
    '''
    fSoil= fs.FrictionalSoil(fi)
    return fSoil.Kav_coulomb(a, b, d)

def k_janssen(k,d,B,z):
    '''
    Lateral earth pressure coefficient for limited backfills according
    to Janssen's Theory (1895) and Kniss et Al (2007):

    :param k: lateral earth pressure (usually k= K_0).
    :param d: friction angle between soil an back of retaining wall (radians).
    :param B: width of the backfill (radians).
    :param z: depth from top of wall.
    '''
    retval= 0.0
    if(z!=0.0):
        tanD= math.tan(d)
        retval= 1.0/(2*tanD)*(B/z)*(1-math.exp(-2*k*z/B*tanD))
    return retval

def kp_coulomb(a,b,fi,d):
    '''
    Return the passive earth pressure coefficient according to Coulomb's theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param fi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).
    '''
    fSoil= fs.FrictionalSoil(fi)
    return fSoil.Kp_coulomb(a, b, d)

#Empujes unitarios debidos a cargas sobre el terreno.

def eq_coulomb(a,b,fi,d,p):
    '''
    Return the lateral earth pressure caused by a uniform load q
    action over the backfill surface according to Coulomb's theory.

    :param a: angle of the back of the retaining wall (radians).
    :param b: slope of the backfill (radians).
    :param fi: internal friction angle of the soil (radians).
    :param d: friction angle between soil an back of retaining wall (radians).
    :param p: Uniform load.
    '''
    fSoil= fs.FrictionalSoil(fi)
    return fSoil.eq_coulomb(a, b, d, p)

def eql_coulomb(x,H,z,ql):
    '''
    Return the earth pressure due to a linear load ql acting on a line
    parallel to the top of the wall on the surface of the retained backfill.

    :param x:  Distance from the top of the back and the linea where the load is applied.
    :param H:  Depth of the bottom end of the wall back.
    :param z:  depth of the point for which the pressure is computed.
    :param l: value of the linear load.
    '''
    m=x/float(H)
    n=z/float(H)
    if m<=0.4:
        return(0.2*m/float(math.pow((0.16+n*n),2))*ql/float(H))
    else:
        return(1.28*m**2*n/float((m**2+n**2)**2)*ql/float(H))
                


def eqp_coulomb(x,H,z,qp):
    ''' Return the earth pressure resulting from a vertical point load qp
    acting behind a retaining wall and is contained in the same vertical
    plane of the wall section.

    :param x:  Distance from the top of the trasdós to the point of application 
        of the load.
    :param H:  Profundidad del extremo inferior del trasdós.
    :param z:  depth of the point for which the pressure is computed.
    :param qp: punctual load
    '''
    m=x/float(H)
    n= z/float(H)
    if m<=0.4:
        return(0.28*n**2/float(math.pow((0.16+n**2),3))*qp/float(H**2))
    else:
        return(1.77*m**2*n**2/float(math.pow((m**2+n**2),3))*qp/float(H**2))

def active_pressure_culmann_method(soil, wallBack, backfillProfile, delta= 0.0, numValues= 8):
    ''' Return the active earth pressure according to Culmann Method.

    :param soil: soil model.
    :param wallBack: 2D segment (geom.Segment2d) representing the back of the
                     wall. The first point of the segment is the top of the 
                     wall and the second one corresponds to the bottom.
    :param backfillProfile: 2D polyline representing the profile of the backfill.
    :param delta: friction angle of the soil and the wall.
    :param numValues: number of test values along the backfill.
    '''
    ptA= wallBack.getFromPoint() # Top of the wall.
    ptB= wallBack.getToPoint() # Bottom of the wall.
    alphaAngle= wallBack.getAngle(geom.Vector2d(0.0,-1.0)) # Angle with respect to the vertical.
    # Soil natural slope line.
    naturalSlopeLine= geom.Ray2d(ptB, geom.Vector2d(math.cos(soil.phi), math.sin(soil.phi)))

    # Get points along the backfill profile
    ## Compute intersection of the backfill profile with the natural slope line.
    ptC= backfillProfile.getIntersection(naturalSlopeLine)[0]

    ## Points along the backfill surface.
    startAngle= alphaAngle+math.pi/2.0
    endAngle= soil.phi
    angleIncrement= (endAngle-startAngle)/(numValues+1)
    angles= list()
    for i in range(1,numValues+1):
        angles.append(startAngle+i*angleIncrement)

    ### Compute points
    pointsAlongBackfill= list()
    raysToBackFill= list()
    for angle in angles:
        ray= geom.Ray2d(ptB, geom.Vector2d(math.cos(angle), math.sin(angle)))
        raysToBackFill.append(ray)
        pt= backfillProfile.getIntersection(ray)[0]
        pointsAlongBackfill.append(pt)

    ### Compute prism weights.
    weights= list()
    gamma= soil.gamma()
    for pt in pointsAlongBackfill:
        triangle= geom.Triangle2d(ptA, ptB, pt)
        area= triangle.getArea()
        w= gamma*area
        weights.append(w)
    minWeight= min(weights)
    maxWeight= max(weights)

    ### Compute weight points along natural slope line.
    weightPoints= list()
    for w in weights:
        pt= naturalSlopeLine.getPoint(w)
        weightPoints.append(pt)

    ### DirectionLine
    directionLineAngle= soil.phi-(math.pi/2.0-alphaAngle-delta)
    directionLine=  geom.Ray2d(ptB, geom.Vector2d(math.cos(directionLineAngle), math.sin(directionLineAngle)))

    ### Compute pressure points.
    pressurePoints= list()
    pressureDistances= list() # distances to natural slope line
    for ray, pt in zip(raysToBackFill, weightPoints):
        ln= geom.Line2d(pt, directionLine.getVDir())
        pressurePoint= ray.getIntersection(ln)[0]
        pressurePoints.append(pressurePoint)
        pressureDistances.append(naturalSlopeLine.dist(pressurePoint))

    pressureFunction= interpolate.interp1d(weights, pressureDistances, kind='quadratic', fill_value="extrapolate")

    xMax= optimize.fminbound(lambda x: -pressureFunction(x), x1= minWeight, x2= maxWeight)
    maxPressure= pressureFunction(xMax)
    return maxPressure, pressureFunction, minWeight, maxWeight
    
    
