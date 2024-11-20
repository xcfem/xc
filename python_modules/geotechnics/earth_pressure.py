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
from geotechnics import frictional_cohesive_soil as fcs
from scipy import interpolate
from scipy import optimize
import geom
from misc_utils import log_messages as lmsg

class SoilModel(object):
    '''Soil response abstract base class.

    :ivar beta: angle of backfill with horizontal
    ;ivar Kh: horizontal reaction modulus of the soil.
    :ivar deltaAngle: friction angle between the soil and the back surface
                      of the retaining wall.
    '''
    def __init__(self, beta= 0.0, Kh= None, deltaAngle= 0.0):
        ''' Constructor.

        :param beta: angle of backfill with horizontal
        ;param Kh: horizontal reaction modulus of the soil.
        :param deltaAngle: friction angle between the soil and the back surface
                           of the retaining wall.
        '''
        self.beta= beta
        self.Kh= Kh
        self.deltaAngle= deltaAngle

    def rho(self):
        ''' Return the soil density.'''
        return self.soil.rho

    def gamma(self):
        ''' Return the specific weight of soil.'''
        return self.soil.gamma()
    
    def submergedGamma(self):
        ''' Return the submerged gamma of the soil.'''
        return self.soil.submergedGamma()
    
    def phi(self, designValue= False):
        ''' Return the specific weight of soil.

        :param designValue: if true use the design value of the angle of 
                            internal friction.
        '''
        if(designValue):
            retval= self.soil.getDesignPhi()
        else:
            retval= self.soil.phi
        return retval

    def getDeltaAngleActivePressure(self, alphaAngle= 0.0):
        '''Returns the angle of the earth pressure with respect to the
           wall back face.

        :param alphaAngle: inclination of the back face.
        '''
        if(alphaAngle==0.0):
            retval= self.beta
        else:
            rhoAngle= 2*alphaAngle-self.beta+math.asin(math.sin(self.beta)/math.sin(self.soil.phi))
            sPhi= math.sin(self.soil.phi)
            retval= math.atan(sPhi*math.sin(rhoAngle)/(1-sPhi*math.cos(rhoAngle)))
        return retval

    def getVerticalStressAtDepth(self, z, waterTableDepth= 6371e3):
        ''' Returns the vertical presure at depth z assuming that the soil is
        homogeneous along the given depth.

        :param z: depth to compute the pressure.
        :param waterTableDepth: depth of the water table.
        '''
        if(z<=waterTableDepth):
            retval= self.soil.gamma()*z
        else:
            retval= self.soil.gamma()*waterTableDepth
            retval+= self.submergedGamma()*(z-waterTableDepth)
        return retval

    def K0Jaky(self, designValue= False):
        '''Returns Jaky's coefficient (earth pressure at rest).

        :param designValue: if true use the design value of the internal 
                            friction.
        '''
        sPhi= math.sin(self.phi(designValue))
        return 1.0-sPhi
    
    def getActivePressure(self, sg_v, alphaAngle= 0.0, designValue= False):
        ''' Returns the active presure corresponding to the given vertical
            pressure.

        :param sg_v: vertical stress.
        :param alphaAngle: inclination of the back face.
        :param designValue: if true use the design value of the internal 
                            friction.
        '''
        Ka= self.Ka(sg_v= sg_v, alphaAngle= alphaAngle, designValue= designValue)
        return Ka*sg_v
    
    def getPassivePressure(self, sg_v, alphaAngle= 0.0, designValue= False):
        ''' Returns the passive presure corresponding to the given vertical
            pressure.

        :param sg_v: vertical stress.
        :param alphaAngle: inclination of the back face.
        :param designValue: if true use the design value of the internal 
                            friction.
        '''
        Kp= self.Kp(sg_v= sg_v, alphaAngle= alphaAngle, designValue= designValue)
        return Kp*sg_v
    
    def getAtRestPressure(self, sg_v, designValue= False):
        ''' Returns the at-rest presure corresponding to the given vertical
            pressure.

        :param z: depth to compute the pressure.
        :param designValue: if true use the design value of the internal 
                            friction.
        '''
        K0= self.K0Jaky(designValue= designValue)
        return K0*sg_v

    def getEarthThrusts(self, sg_v, tributaryArea, alphaAngle= 0.0, designValue= False):
        ''' Returns the active, at-rest and passive presure corresponding to 
            the given vertical pressure.

        :param sg_v: vertical stress.
        :param tributaryArea: area on which the pressure acts.
        :param alphaAngle: inclination of the back face.
        :param designValue: if true use the design value of the internal 
                            friction.
        '''
        # Get friction angle between the soil and the back surface
        # of the retaining wall (radians).
        Ea= self.getActivePressure(sg_v= sg_v, alphaAngle= alphaAngle, designValue= designValue)*tributaryArea # active.
        E0= self.getAtRestPressure(sg_v= sg_v, designValue= designValue)*tributaryArea # at rest.
        Ep= self.getPassivePressure(sg_v= sg_v, alphaAngle= alphaAngle, designValue= designValue)*tributaryArea # passive.
        return Ea, E0, Ep
    
    def defHorizontalSubgradeReactionNlMaterial(self, preprocessor, name, sg_v, tributaryArea, alphaAngle= 0.0, designValue= False):
        ''' Return the points of the force-displacement diagram.

        :param preprocessor: preprocessor of the finite element problem.
        :param name: name identifying the material (if None compute a suitable name)
        :param sg_v: vertical stress.
        :param tributaryArea: area on which the pressure acts.
        :param alphaAngle: inclination of the back face.
        :param designValue: if true use the design value of the internal 
                            friction.
        '''
        # Compute corresponding earth thrusts (active, at rest, passive).
        Ea, E0, Ep= self.getEarthThrusts(sg_v= sg_v, alphaAngle= alphaAngle, tributaryArea= tributaryArea, designValue= designValue)
        # Define nonlinear spring material
        matName= name
        if(not matName):
            matName= uuid.uuid1().hex            
        eyMatName= 'ey'+matName
        eyBasicMaterial= def_ey_basic_material(preprocessor, name= eyMatName, E= self.Kh, upperYieldStress= -Ea, lowerYieldStress= -Ep)
        # Create initial stress material.
        materialHandler= preprocessor.getMaterialHandler
        retval= materialHandler.newMaterial("init_stress_material", matName)
        retval.setMaterial(eyBasicMaterial.name)
        retval.setInitialStress(-E0)
        return retval
      
class RankineSoil(SoilModel):
    '''From Wikipedia: Rankine's theory, developed in 1857, is a stress field
       solution that predicts active and passive earth pressure. It assumes
       that the soil is cohesionless, the wall is frictionless, the soil-wall
       interface is vertical, the failure surface on which the soil moves is 
       planar, and the resultant force is angled parallel to the backfill
       surface. The equations for active and passive lateral earth pressure 
       coefficients are given below. Note that φ is the angle of shearing 
       resistance of the soil and the backfill is inclined at angle β to 
       the horizontal.
    '''
    def __init__(self, phi, beta= 0.0, rho= 2100.0, rhoSat= None, gammaMPhi= 1.0, Kh= None, deltaAngle= 0.0):
        ''' Constructor.

        :param phi: internal friction angle of the soil.
        :param beta: angle of backfill with horizontal.
        :param rho: soil density.
        :param rhoSat: saturated density of the soil (mass per unit volume).
        :param gammaMPhi: (float) partial reduction factor for internal
                          friction angle of the soil.
        ;param Kh: horizontal reaction modulus of the soil.
        :param deltaAngle: friction angle between the soil and the back surface
                           of the retaining wall.
        '''
        super(RankineSoil,self).__init__(beta= beta, Kh= Kh, deltaAngle= deltaAngle)
        self.soil= fs.FrictionalSoil(phi= phi, rho= rho, rhoSat= rhoSat, gammaMPhi= gammaMPhi)
        
    def Ka(self, sg_v= None, alphaAngle= 0.0, designValue= False):
        '''Returns Rankine's active earth pressure coefficient.

        :param sg_v: vertical stress (not used for Rankine soils).
        :param alphaAngle: inclination of the back face.
        :param designValue: if true use the design value of the internal friction.
        '''
        if(self.deltaAngle != 0.0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; friction with the wall not implemented yet.')
        phi= self.phi(designValue)
        if(phi<self.beta):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; backfill soil internal friction angle: '+str(math.degrees(phi))+" can't be smaller than the slope of the backfill: "+str(math.degrees(self.beta))+' otherwise the backfill slope is not stable. Assuming a big enough internal friction angle: '+str(math.degrees(self.beta))+' for the purpose of determine the active pressure coefficient only.')
            phi= self.beta
        cPhi= math.cos(phi)
        cBeta= math.cos(self.beta)
        if(alphaAngle==0.0):
            r= math.sqrt(cBeta**2-cPhi**2)
            retval= cBeta*(cBeta-r)/(cBeta+r)
        else:
            # See figure 3.7.13 in ROM 0.5-05 chapter 3
            # https://www.puertos.es/es-es/BibliotecaV2/ROM%200.5-05%20(EN).pdf
            rhoAngle= 2*alphaAngle-self.beta+math.asin(math.sin(self.beta)/math.sin(phi))
            sPhi= math.sin(phi)
            r= math.sqrt(sPhi**2- math.sin(self.beta)**2)
            n= math.sqrt(1+sPhi**2-2*sPhi*math.cos(rhoAngle))
            retval= math.cos(alphaAngle-self.beta)/(math.cos(alphaAngle)**2)*n/(math.cos(self.beta)*r)
        return retval
          
    def Kp(self, sg_v= None, alphaAngle= 0.0, designValue= False):
        '''Returns Rankine's passive earth pressure coefficient.

        :param sg_v: vertical stress (not used for Rankine soils).
        :param alphaAngle: angle of the back of the retaining wall (radians).
        :param designValue: if true use the design value of the internal 
                            friction.
        '''
        if(alphaAngle != 0.0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; inclined wall back surface not implemented yet.')
        if(self.deltaAngle != 0.0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; friction with the wall not implemented yet.')
        cBeta= math.cos(self.beta)
        cPhi= math.cos(self.phi(designValue))
        r= math.sqrt(cBeta**2-cPhi**2)
        return cBeta*(cBeta+r)/(cBeta-r)
    
class CoulombSoil(SoilModel):
    '''Soil response according to Coulomb's theory.

    '''
    def __init__(self, phi, beta= 0.0, rho= 2100.0, rhoSat= None, phi_cv= None, gammaMPhi= 1.0, gammaMc= 1.0, E= 1e8, nu= 0.3, Kh= None, deltaAngle= 0.0):
        ''' Constructor.

        :param phi: internal friction angle of the soil
        :param beta: angle of backfill with horizontal
        :param rho: soil density.
        :param rhoSat: saturated density of the soil (mass per unit volume)
        :param phi_cv: critical state (constant volume) angle of shearing resistance of the soil. See clause 6.5.3 (10) of Eurocode 7 part 1. 
        :param gammaMPhi: (float) partial reduction factor for internal 
                          friction angle of the soil.
        :param gammaMc: (float) partial reduction factor for soil cohesion.
        :param E: Young's modulus (defaults to 1e8 Pa).
        :param nu: Poisson's ratio (defaults to 0.3).
        ;param Kh: horizontal reaction modulus of the soil.
        :param deltaAngle: friction angle between the soil and the back surface
                           of the retaining wall.
        '''
        super(CoulombSoil,self).__init__(beta= beta, Kh= Kh, deltaAngle= deltaAngle)
        self.soil= fs.FrictionalSoil(phi= phi, rho= rho, rhoSat= rhoSat, phi_cv= phi_cv, gammaMPhi= gammaMPhi, E= E, nu= nu)

    def Ka(self, sg_v= None, alphaAngle= 0.0, designValue= False):
        '''
        Return the horizontal component of the active earth pressure coefficient
        according to Coulomb's theory.

        :param sg_v: vertical stress (not used for Coulomb soils).
        :param alphaAngle: angle of the back of the retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        return self.soil.Kah_coulomb(a= alphaAngle, b= self.beta, d= self.deltaAngle, designValue= designValue)

    def Kp(self, sg_v= None, alphaAngle= 0.0, designValue= False):
        '''
        Return the horizontal component of the passive earth pressure 
        coefficient according to Coulomb's theory.

        :param sg_v: vertical stress (not used for Coulomb soils).
        :param alphaAngle: angle of the back of the retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        return self.soil.Kp_coulomb(a= alphaAngle, b= self.beta, d= self.deltaAngle, designValue= designValue)
    
class BellSoil(SoilModel):
    '''Soil response according to Bell's theory of earth pressure for clay.

    '''
    def __init__(self, phi, c, beta= 0.0, rho= 2100.0, rhoSat= None, phi_cv= None, gammaMPhi= 1.0, gammaMc= 1.0, E= 1e8, nu= 0.3, Kh= None, deltaAngle= 0.0):
        ''' Constructor.

        :param phi: internal friction angle of the soil
        :param c: (float) soil cohesion.
        :param beta: angle of backfill with horizontal
        :param rho: soil density.
        :param rhoSat: saturated density of the soil (mass per unit volume)
        :param phi_cv: critical state (constant volume) angle of shearing resistance of the soil. See clause 6.5.3 (10) of Eurocode 7 part 1. 
        :param gammaMPhi: (float) partial reduction factor for internal 
                          friction angle of the soil.
        :param gammaMc: (float) partial reduction factor for soil cohesion.
        :param E: Young's modulus (defaults to 1e8 Pa).
        :param nu: Poisson's ratio (defaults to 0.3).
        ;param Kh: horizontal reaction modulus of the soil.
        :param deltaAngle: friction angle between the soil and the back surface
                           of the retaining wall.
        '''
        super(BellSoil,self).__init__(beta= beta, Kh= Kh, deltaAngle= deltaAngle)
        self.soil= fcs.FrictionalCohesiveSoil(phi= phi, c= c, rho= rho, rhoSat= rhoSat, phi_cv= phi_cv, gammaMPhi= gammaMPhi, gammaMc= gammaMc, E= E, nu= nu)

    def getActivePressure(self, sg_v, alphaAngle= 0.0, designValue= False):
        ''' Returns the active presure corresponding to the given vertical
            pressure.

        :param sg_v: vertical stress.
        :param alphaAngle: inclination of the back face.
        :param designValue: if true use the design value of the internal 
                            friction.
        '''
        return self.soil.eah_coulomb(sg_v= sg_v, a= alphaAngle, b= self.beta, d= self.deltaAngle, designValue= designValue)
    
    def getPassivePressure(self, sg_v, alphaAngle= 0.0, designValue= False):
        ''' Returns the passive presure corresponding to the given vertical
            pressure.

        :param sg_v: vertical stress.
        :param alphaAngle: inclination of the back face.
        :param designValue: if true use the design value of the internal 
                            friction.
        '''
        return self.soil.eph_coulomb(sg_v= sg_v, a= alphaAngle, b= self.beta, d= self.deltaAngle, designValue= designValue)
    
    def getCoulombTensionCrackDepth(self, sg_v, alphaAngle= 0.0, designValue= False):
        ''' Return the depth of the tension crack (the depth at which
            active lateral earth pressure is cero due to soil cohesion).

        :param sg_v: vertical stress.
        :param alphaAngle: angle of the back of the retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction and the cohesion.
        '''
        return self.soil.getCoulombTensionCrackDepth(sg_v, a= alphaAngle, b= self.beta, d= self.deltaAngle, designValue= False)      
    

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
def ka_rankine(b,phi):
    '''Return the active earth pressure coefficient according to Rankine theory.

    :param b:  slope of the backfill (radians).
    :param phi: internal friction angle of the soil (radians).

    '''
    fSoil= RankineSoil(phi= phi, beta= b)
    return fSoil.Ka()

def kp_rankine(b,phi):
    '''Return the passive earth pressure coefficient according to Rankine theory.

    :param b:  slope of the backfill (radians).
    :param phi: internal friction angle of the soil (radians).

    '''
    fSoil= RankineSoil(phi= phi, beta= b)
    return fSoil.Kp()

def phi_rankine_from_active_coefficient(b, ka):
    '''Return the soil internal friction angle of the soil, given its active
       earth pressure coefficient according to Rankine theory.

    :param b:  slope of the backfill (radians).
    :param ka: active earth pressure coefficiente according to Rankine theory.
    '''
    def rankine_ka(phi):
        '''Returns Rankine's active earth pressure coefficient.'''
        cBeta= math.cos(b)
        cPhi= math.cos(phi)
        r= math.sqrt(cBeta**2-cPhi**2)
        return (cBeta*(cBeta-r)/(cBeta+r)-ka)
    result= optimize.root_scalar(rankine_ka, bracket=[b, math.pi/2.0], method='brentq')
    retval= None
    if(result.converged):
        retval= result.root
    return retval
    

def ka_coulomb(a,b,phi,d):
    '''Return the active earth pressure coefficient according to Coulomb theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param phi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).See Jiménez Salas, Geotecnia y Cimientos page 682.

    '''
    fSoil= fs.FrictionalSoil(phi)
    return fSoil.Ka_coulomb(a, b, d)

def kah_coulomb(a,b,phi,d):
    '''
    Return the horizontal component of the active earth pressure coefficient
    according to Coulomb's theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param phi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).
    '''
    fSoil= fs.FrictionalSoil(phi)
    return fSoil.Kah_coulomb(a, b, d)

def kav_coulomb(a,b,phi,d):
    '''
    Return the vertical component of the active earth pressure coefficient
    according to Coulomb's theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param phi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).
    '''
    fSoil= fs.FrictionalSoil(phi)
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

def kp_coulomb(a,b,phi,d):
    '''
    Return the passive earth pressure coefficient according to Coulomb's theory.

    :param a:  angle of the back of the retaining wall (radians).
    :param b:  slope of the backfill (radians).
    :param phi: internal friction angle of the soil (radians).
    :param d:  friction angle between soil an back of retaining wall (radians).
    '''
    fSoil= fs.FrictionalSoil(phi)
    return fSoil.Kp_coulomb(a, b, d)

#Empujes unitarios debidos a cargas sobre el terreno.

def eq_coulomb(a,b,phi,d,p):
    '''
    Return the lateral earth pressure caused by a uniform load q
    action over the backfill surface according to Coulomb's theory.

    :param a: angle of the back of the retaining wall (radians).
    :param b: slope of the backfill (radians).
    :param phi: internal friction angle of the soil (radians).
    :param d: friction angle between soil an back of retaining wall (radians).
    :param p: Uniform load.
    '''
    fSoil= fs.FrictionalSoil(phi)
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
    def getPolygon(cutPoint):
        ''' Return the polygon defined by the backfill surface between
            the wall back and the cutPoint.'''
        retval= geom.Polygon2d()
        terrainSurface= backfillProfile.getLeftChunk(cutPoint, 1e-3)
        for pt in terrainSurface:
            retval.appendVertex(pt)
        retval.appendVertex(ptB)
        return retval
        
    ptB= wallBack.getToPoint() # Bottom of the wall.
    alphaAngle= wallBack.getAngle(geom.Vector2d(0.0,-1.0)) # Angle with respect to the vertical.
    TwoPi= 2*math.pi
    alphaAngle%=TwoPi
    if(abs(alphaAngle-TwoPi)<.01):
        alphaAngle-= TwoPi
    # Soil natural slope line.
    naturalSlopeLine= geom.Ray2d(ptB, geom.Vector2d(math.cos(soil.phi), math.sin(soil.phi)))

    # Get points along the backfill profile

    ## Points along the backfill surface.
    startAngle= alphaAngle+math.pi/2.0
    endAngle= soil.phi
    angleIncrement= (endAngle-startAngle)/(numValues+1)
    angles= list()
    for i in range(1,numValues+1):
        angles.append(startAngle+i*angleIncrement)

    ### Compute points
    pointsAlongBackfill= list()
    raysToBackfill= list()
    for angle in angles:
        ray= geom.Ray2d(ptB, geom.Vector2d(math.cos(angle), math.sin(angle)))
        raysToBackfill.append(ray)
        intersections= backfillProfile.getIntersection(ray)
        if(len(intersections)>0):
            pt= intersections[0]
            pointsAlongBackfill.append(pt)
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+'; intersection with direction line not found for ray: '+str(ray)+ ' angle: '+str(math.degrees(angle)))
            exit(1)

    ### Compute prism weights.
    weights= list()
    gamma= soil.gamma()
    for pt in pointsAlongBackfill:
        plg= getPolygon(pt)
        area= plg.getArea()
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
    for ray, pt in zip(raysToBackfill, weightPoints):
        ln= geom.Line2d(pt, directionLine.getVDir())
        intersections= ray.getIntersection(ln)
        if(len(intersections)>0):
            pressurePoint= intersections[0]
            pressurePoints.append(pressurePoint)
            pressureDistances.append(naturalSlopeLine.dist(pressurePoint))
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+'; intersection with direction line not found for ray: '+str(ray))
            exit(1)

    pressureFunction= interpolate.interp1d(weights, pressureDistances, kind='quadratic', fill_value="extrapolate")

    xMax= optimize.fminbound(lambda x: -pressureFunction(x), x1= minWeight, x2= maxWeight)
    maxPressure= pressureFunction(xMax)
    return maxPressure, pressureFunction, minWeight, maxWeight

def get_earth_thrusts(sg_v, tributaryArea, Ka, K0, Kp):
    ''' Return the earth thrusts corresponding to the given argumenst.

    :param sg_v: vertical stress.
    :param tributaryArea: area on which the pressure acts.
    :param Ka: active earth pressure coefficient.
    :param K0: earth pressure at rest coefficient.
    :param Kp: passive earth pressure coefficient.
    '''
    factor= sg_v*tributaryArea
    Ea= Ka*factor # active.
    E0= K0*factor # at rest.
    Ep= Kp*factor # passive.
    return Ea, E0, Ep
    
def get_horizontal_soil_reaction_diagram(sg_v, tributaryArea, Ka, K0, Kp, Kh):
    ''' Return the points of the force-displacement diagram.

    :param sg_v: vertical stress.
    :param tributaryArea: area on which the pressure acts.
    :param Ka: active earth pressure coefficient.
    :param K0: earth pressure at rest coefficient.
    :param Kp: passive earth pressure coefficient.
    :param Kh: horizontal Winkler modulus.
    '''
    Ea, E0, Ep= get_earth_thrusts(sg_v= sg_v, tributaryArea= tributaryArea, Ka= Ka, K0= K0, Kp= Kp)
    # Compute active and passive limits.
    activeLimit= (Ea-E0)/Kh
    passiveLimit= (Ep-E0)/Kh
    # Compute diagram bounds.
    bandWidth= 20.0*max(abs(activeLimit), abs(passiveLimit))
    lowerBound= -bandWidth/2.0
    upperBound= bandWidth/2.0
    # Move the diagram towards positive domain.
    initStrain= -math.floor(lowerBound*1e3)/1e3

    # Compute diagram points.
    ## Point at rest
    lowerBoundPt= (lowerBound+initStrain, 0.99*Ea)
    activeLimitPt= (activeLimit+initStrain, Ea)
    atRestPt= (initStrain, E0)
    passiveLimitPt= (passiveLimit+initStrain, Ep)
    upperBoundPt= (upperBound+initStrain, 1.01*Ep)

    return [lowerBoundPt, activeLimitPt, atRestPt, passiveLimitPt, upperBoundPt], initStrain


def def_ey_basic_material(preprocessor, name, E, upperYieldStress, lowerYieldStress):
    '''Constructs an elastic perfectly-plastic uniaxial material adapted
       to represent the horizontal thrust of a soil.

    :param preprocessor: preprocessor of the finite element problem.
    :param name: name identifying the material (if None compute a suitable name)
    :param E: tangent in the elastic zone of the stress-strain diagram
    :param upperYieldStress: stress at which material reaches plastic state 
                             in decompression.
    :param lowerYieldStress: stress at which material reaches plastic state 
                             in compression.
    '''
    materialHandler= preprocessor.getMaterialHandler
    matName= name
    if(not matName):
        matName= uuid.uuid1().hex
    retval= materialHandler.newMaterial("EyBasic", matName)
    retval.E= E
    retval.setLowerYieldStress(lowerYieldStress) # passive state yield stress.
    retval.setUpperYieldStress(upperYieldStress) # active state yield stress.
    retval.revertToStart() # Compute material derived parameters.
    return retval

