# -*- coding: utf-8 -*-
'''frictional_soil.py: frictional (or cohesionless) soil model.

References: 

[1] Chapter 4-3 of Foundation Analysis and Design, Ed. 5 by Joseph E. Bowles.
[2] Brinch Hansen. A general formula for bearing capacity. The Danish Geotechnical Institute. Bulletin 11. Copenhagen 1961.
[3] Guía de cimentaciones en obras de carretera. Ministerio de Fomento (spain). 2002 (https://books.google.ch/books?id=a0eoygAACAAJ).
[4] Eurocódigo 7. Proyecto geotécnico. Parte 1: reglas generales.
'''

from __future__ import division
from __future__ import print_function

import sys
import math
from scipy.constants import g
from geotechnics import mononobe_okabe
from misc_utils import log_messages as lmsg

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

class FrictionalSoil(object):
    '''Free-running type of soil, such as sand or gravel, whose strength depends on friction between particles.

    :ivar phi: internal friction angle of the soil.
    :ivar rho: soil density (mass per unit volume).
    :ivar E: Young's modulus.
    :ivar nu: Poisson's ratio.
    '''
    def __init__(self, phi:float, rho= 2100.0, rhoSat= None, phi_cv= None, gammaMPhi= 1.0, E= 1e8, nu= 0.3):
        '''Constructor.

        :param phi: (float) internal friction angle of the soil.
        :param rho: (float) soil density (mass per unit volume).
        :param rhoSat: saturated density of the soil (mass per unit volume).
        :param phi_cv: critical state (constant volume) angle of shearing resistance of the soil. See clause 6.5.3 (10) of Eurocode 7 part 1. 
        :param gammaMPhi: (float) partial reduction factor for internal 
                          friction angle of the soil.
        :param E: Young's modulus (defaults to 1e8 Pa).
        :param nu: Poisson's ratio (defaults to 0.3).
        '''
        self.phi= phi
        self.gammaMPhi= gammaMPhi
        self.rho= rho
        self.rhoSat= rhoSat
        self.phi_cv= phi_cv
        self.E= E
        self.nu= nu

    def K0Jaky(self, designValue= False):
        '''Returns Jaky's coefficient (earth pressure at rest).

        :param designValue: if true use the design value of the internal
                            friction.
        '''
        if(designValue):
            phi= self.getDesignPhi()
        else:
            phi= self.phi
        return 1.0-math.sin(phi)

    def Ka(self, designValue= False):
        '''Active earth pressure coefficient.

        :param designValue: if true use the design value of the internal
                            friction.
        '''
        if(designValue):
            phi= self.getDesignPhi()
        else:
            phi= self.phi
        sinPhi= math.sin(phi)
        return (1-sinPhi)/(1+sinPhi)
      
    def Kp(self, designValue= False):
        '''Passive earth pressure coefficient.

        :param designValue: if true use the design value of the internal
                            friction.
        '''
        if(designValue):
            phi= self.getDesignPhi()
        else:
            phi= self.phi
        sinPhi= math.sin(phi)
        return (1+sinPhi)/(1-sinPhi)
      
    def Ka_coulomb(self, a, b, d= 0.0, designValue= False):
        '''
        Return the active earth pressure coefficient according to Coulomb's 
        theory (see Recomendaciones para obras marítimas ROM 0.5-05 
        figure 3.7.12.).

        :param a:  angle of the back of the retaining wall (radians, 0 if 
                   vertical).
        :param b:  slope of the backfill (radians, 0 if horizontal).
        :param d:  friction angle between soil and the back of retaining 
                   wall (radians).
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        if(designValue):
            phi= self.getDesignPhi()
        else:
            phi= self.phi
        if(phi<b):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; backfill soil internal friction angle: '+str(math.degrees(phi))+" can't be smaller than the slope of the backfill: "+str(math.degrees(b))+' otherwise the backfill slope is not stable. Assuming a big enough internal friction angle: '+str(math.degrees(b))+' for the purpose of determine the active pressure coefficient only.')
            phi= b
        num= 1.0/math.cos(a)*math.cos(phi-a)
        r1= math.sqrt(math.cos(a+d))
        r2= math.sqrt(math.sin(phi+d)*math.sin(phi-b)/math.cos(b-a))
        return (math.pow((num/(r1+r2)),2))

    def Kah_coulomb(self,a,b,d, designValue= False):
        '''
        Return the horizontal component of the active earth pressure coefficient
        according to Coulomb's theory.

        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param d:  friction angle between soil an back of retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        return (self.Ka_coulomb(a,b,d, designValue= designValue)*math.cos(a+d))
      
    def Kav_coulomb(self,a,b,d, designValue= False):
        '''
        Return the vertical component of the active earth pressure coefficient
        according to Coulomb's theory.

        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param phi: internal friction angle of the soil (radians).
        :param d:  friction angle between the soil an the back surface of the
                   retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        return (self.Ka_coulomb(a,b,d, designValue= False)*math.sin(a+d))

    def Kp_coulomb(self, a, b, d= 0.0, designValue= False):
        '''
        Return the passive earth pressure coefficient according to Coulomb's theory.

        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param d:  friction angle between the soil and the back surface
                   of the retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        if(designValue):
            phi= self.getDesignPhi()
        else:
            phi= self.phi
        if(b>phi):
            lmsg.error('The angle of the backfill: '+str(math.degrees(b))+' is greater than the friction angle: '+str(math.degrees(phi)))
        p1= 1.0/(math.cos(a)**2)/math.cos(d-a)
        num= math.cos(phi+a)
        r1= math.sqrt(math.sin(phi+d)*math.sin(phi+b)/(math.cos(d-a)*math.cos(b-a)))
        denom= 1.0-r1
        retval= p1*((num/denom)**2)
        return retval

    def eq_coulomb(self, a, b, d, q, designValue= False):
        '''
        eq_coulomb(a,b,d,q):
        Return the lateral earth pressure caused by a uniform load q
        action over the backfill surface according to Coulomb's theory.

        :param a: angle of the back of the retaining wall (radians).
        :param b: slope of the backfill (radians).
        :param d: friction angle between soil an back of retaining wall (radians).
        :param q: Uniform load.
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        return (self.Ka_coulomb(a,b,d, designValue= designValue)*q*math.cos(a)/float(math.cos(b-a)))

    def eq(self, a, b, d, q, designValue= False):
        '''
        eq_coulomb(a,b,d,q):
        Return the lateral earth pressure caused by a uniform load q
        action over the backfill surface.

        :param a: angle of the back of the retaining wall (radians).
        :param b: slope of the backfill (radians).
        :param d: friction angle between soil an back of retaining wall (radians).
        :param q: Uniform load.
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        return self.eq_coulomb(a= a, b= b, d= d, q= q, designValue= designValue)
      
    def ea_coulomb(self, sg_v, a, b, d= 0.0, designValue= False):
        ''' Return the lateral earth active pressure.

        :param sg_v:  vertical stress.
        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param d:  friction angle between soil and the back surface of the 
                   retaining wall (radians). See Jiménez Salas, Geotecnia y 
                   Cimientos page 682 and Bell's relationship.
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        retval= 0.0
        if(sg_v>0.0):
            ka= self.Ka_coulomb(a,b,d, designValue= designValue)
            retval= ka*sg_v
        return retval

    def eah_coulomb(self, sg_v, a, b, d= 0.0, designValue= False):
        '''
        Return the horizontal component of the lateral earth active pressure.

        :param sg_v:  vertical stress.
        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param d:  friction angle between soil and the back surface of the 
                   retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        return (self.ea_coulomb(sg_v= sg_v, a= a, b= b, d= d, designValue= designValue)*math.cos(a+d))

    def eav_coulomb(self, sg_v, a, b, d= 0.0, designValue= False):
        '''
        Return the vertical component of the active earth pressure coefficient
        according to Coulomb's theory.

        :param sg_v:  vertical stress.
        :param a: angle of the back of the retaining wall (radians).
        :param b: slope of the backfill (radians).
        :param fi: internal friction angle of the soil (radians).
        :param d:  friction angle between soil and the back surface of the 
                   retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        return (self.ea_coulomb(sg_v= sg_v, a= a, b= b, d= d, designValue= designValue)*math.sin(a+d))
      
    def ep_coulomb(self, sg_v, a, b, d= 0.0, designValue= False):
        '''
        Return the lateral earth passive pressure.

        :param sg_v:  vertical stress.
        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param d:  friction angle between the soil an the back surface of the
                   retaining wall (radians). See Jiménez Salas, Geotecnia y 
                   Cimientos page 682 and Bell's relationship.
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        retval= 0.0
        kp= self.Kp_coulomb(a,b,d, designValue= designValue)
        retval= max(kp*sg_v, 0.0) # No tension.
        return retval

    def eph_coulomb(self, sg_v, a, b, d, designValue= False):
        '''
        Return the horizontal component of the lateral earth passive pressure.

        :param sg_v:  vertical stress.
        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param d:  friction angle between soil and the back surface of the 
                   retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        return (self.ep_coulomb(sg_v= sg_v, a= a, b= b, d= d, designValue= designValue)*math.cos(a+d))

    def epv_coulomb(self, sg_v, a, b, d, designValue= False):
        '''
        Return the vertical component of the passive earth pressure
        according to Coulomb's theory.

        :param sg_v:  vertical stress.
        :param a: angle of the back of the retaining wall (radians).
        :param b: slope of the backfill (radians).
        :param fi: internal friction angle of the soil (radians).
        :param d:  friction angle between soil and the back surface of the 
                   retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction.
        '''
        return (self.ep_coulomb(sg_v= sg_v, a= a, b= b, d= d, designValue= designValue)*math.sin(a+d))
    
    def gamma(self):
        '''Unit weight of soil'''
        return self.rho*g

    def submergedGamma(self, waterDensity= 1e3):
        '''Return the submerged specific gravity of the soil.

        :param waterDensity: water density.
        '''
        retval= None
        if(self.rhoSat):
            retval= (self.rhoSat-waterDensity)*g
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; soil saturated density (rhoSat) is not defined.')
        return retval            
      
    def getDesignPhi(self):
        '''Return the design value of the soil internal friction angle.'''
        return math.atan(math.tan(self.phi)/self.gammaMPhi)
      
    def getDesignC(self):
        '''Return the design value of the soil cohesion.'''
        return 0.0
    
    def sgamma(self, Beff, Leff):
        '''Factor that introduces the effect of foundation shape on
           the self weight component.

           :param Beff: Width of the effective foundation area
                        (see figure 12 in page 44 of reference[2]).
           :param Leff: Length of the effective foundation area
                        (see figure 12 in page 44 of reference[2]).
        '''
        return 1.0-0.3*Beff/Leff

    def igamma(self, Vload, HloadB, HloadL, Beff, Leff):
        '''Factor that introduces the effect of load inclination on
           the self weight component. Computed according to section
           D.4 of Eurocode 7 part 1.

        :param Vload: Vertical load. 
        :param HloadB: Horizontal load on Beff direction. 
        :param HloadL: Horizontal load on Leff direction. 
        :param Beff: Width of the effective foundation area
                    (see figure 12 in page 44 of reference[2]).
        :param Leff: Length of the effective foundation area
                    (see figure 12 in page 44 of reference[2]).
        '''
        #return (1-math.tan(deltaB))**3*(1-math.tan(deltaL))
        m= self.m(HloadB, HloadL, Beff, Leff)
        iq= self.iq(Vload= Vload, HloadB= HloadB, HloadL= HloadL)
        retval= pow(iq, (m+1)/m)
        return retval 

    def dgamma(self):
        '''Factor that introduces the effect of foundation depth on
           the self weight component.'''
        return 1.0

    def tgamma(self, psi= 0.0):
        '''Factor that introduces the effect of the proximity of an slope.

           :param psi: angle of the line on which the q load acts 
                       (see figure 4.7 in page 102 of reference [3])
                       must be determined by iterations.
        '''
        return self.tq(psi)

    def rgamma(self, eta= 0.0):
        '''Factor that introduces the effect of sloped footing.

        :param eta: angle between the normal to the foundation plane and 
                    the vertical  (see figure 4.8 in page 104 of reference [3])
                    favourable effect when eta<0.0.
        '''
        return self.rq(eta)

    def Ngamma(self, NgammaCoef= 1.5):
        '''Returns the wedge weight multiplier for the Brinch-Hasen formula.

        :param NgammaCoef: 1.5 in reference [1], 1.8 in reference [2] 
                           and 2 in references [3] and [4].
        '''
        return NgammaCoef*(self.Nq()-1.0)*math.tan(self.getDesignPhi())
    
    def quGamma(self, D, Beff, Leff, Vload, HloadB, HloadL, NgammaCoef= 1.5, psi= 0.0,eta= 0.0):
        '''Gamma "component" of the ultimate bearing capacity pressure of 
           the soil.

        :param D: foundation depth.
        :param Beff: Width of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Leff: Length of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Vload: Vertical load. 
        :param HloadB: Horizontal load on Beff direction. 
        :param HloadL: Horizontal load on Leff direction. 
        :param NgammaCoef: 1.5 in reference [1], 1.8 in reference [2] 
                           and 2 in references [3] and [4]
        :param psi: angle of the line on which the q load acts 
                    (see figure 4.7 in page 102 of reference [3])
                    must be determined by iterations.
        :param eta: angle between the normal to the foundation plane and 
                    the vertical  (see figure 4.8 in page 104 of reference [3])
                    favourable effect when eta<0.0.
        '''
        igamma= self.igamma(Vload= Vload, HloadB= HloadB, HloadL= HloadL, Beff= Beff, Leff= Leff) # inclination factor.
        return 0.5*self.gamma()*Beff*self.Ngamma(NgammaCoef)*self.dgamma()*igamma*self.sgamma(Beff,Leff)*self.tgamma(psi)*self.rgamma(eta)

    def Nc(self):
        '''Returns the cohesion multiplier for the Brinch-Hasen formula.'''
        dphi= self.getDesignPhi()
        if(dphi!=0.0):
            return (self.Nq()-1.0)*(1.0/math.tan(dphi))
        else:
            return math.pi+2.0    
    
    def sq(self, Beff, Leff):
        '''Factor that introduces the effect of foundation shape on
           the overburden component.

           :param Beff: Width of the effective foundation area
                        (see figure 12 in page 44 (8 in the PDF) of ref. 2).
           :param Leff: Length of the effective foundation area
                        (see figure 12 in page 44 (8 in the PDF) of ref. 2).
        '''
        return 1.0+Beff/Leff*self.Nq()/self.Nc()

    def iq(self, Vload, HloadB, HloadL):
        '''Factor that introduces the effect of load inclination on
           the overburden component. Computed according the expression
        (11) of the reference [2].

        :param Vload: Vertical load. 
        :param HloadB: Horizontal load on Beff direction. 
        :param HloadL: Horizontal load on Leff direction. 
        '''
        #return (1.0-0.7*math.tan(deltaB))**3*(1.0-math.tan(deltaL))
        # Compute the alpha angle.
        Hload= math.sqrt(HloadB**2+HloadL**2)
        delta= math.atan(Hload/Vload)
        tanDelta= math.tan(delta)
        cotPhi= 0.0
        sinPhi= 0.0
        if(abs(self.phi)>1e-6):
            cotPhi= 1.0/math.tan(self.phi)
            sinPhi= math.sin(self.phi)
        tg= 0.0
        square= (tanDelta*cotPhi)**2
        if(square<1):
            sqrt= math.sqrt(1-square)
            if(sinPhi!=0.0):
                tg= math.atan((sqrt-tanDelta)/(1+tanDelta/sinPhi))
        alphaAngle= tg+self.phi/2.0
        retval= (1+sinPhi*math.sin(2*alphaAngle-self.phi))/(1+sinPhi)*math.exp(-(0.5*math.pi+self.phi-2*alphaAngle)*math.tan(self.phi))
        return retval

    def dq(self, D, Beff):
        '''Overburden factor for foundation depth.

           :param D: foundation depth.
           :param Beff: Width of the effective foundation area
                        (see figure 12 in page 44 (8 in the PDF) of ref. 2).
        '''
        k= min(D,2.0*Beff)/Beff
        dphi= self.getDesignPhi()
        return 1+2*math.tan(dphi)*(1-math.sin(dphi))**2*math.atan(k)

    def tq(self, psi= 0.0):
        '''Factor that introduces the effect of the proximity of an slope.

           :param psi: angle of the line on which the q load acts 
                       (see figure 4.7 in page 102 of reference [3])
                       must be determined by iterations.
        '''
        return (1-0.5*math.tan(psi))**5

    def rq(self, eta= 0.0):
        '''Factor that introduces the effect of sloped footing.

           :param eta: angle between the foundation plane with and the 
                       (see figure 4.8 in page 104 of reference [3])
                       favourable effect when eta<0.0.
        '''
        dphi= self.getDesignPhi()
        if(dphi!=0.0):
            return math.exp(-2*eta*math.tan(dphi))
        else:
            return 1.0

    def Nq(self):
        '''Returns the overburden multiplier for the Brinch-Hansen formula.'''
        return self.Kp()*math.exp(math.pi*math.tan(self.getDesignPhi()))
    
    def m(self, HloadB, HloadL, Beff, Leff):
        ''' Return the value of the parameter m as defined in section
            D.4 of Eurocode 7 part 1.

        :param HloadB: Horizontal load on Beff direction. 
        :param HloadL: Horizontal load on Leff direction. 
        :param Beff: Width of the effective foundation area
                   (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Leff: Length of the effective foundation area
                    (see figure 12 in page 44 (8 in the PDF) of reference 2).
        '''
        B_L= Beff/Leff
        mB= (2+B_L)/(1+B_L)
        L_B= 1.0/B_L
        mL= (2+L_B)/(1+L_B)        
        if(abs(HloadL)<1e-6): # H acts in the direction of Beff
            retval= mB
        elif(abs(HloadB)<1e-6): # H acts in the direction of Leff
            retval= mL
        else:
            theta= math.atan(HloadL/HloadB)
            retval= mL*math.cos(theta)**2+mB*math.sin(theta)**2
        return retval

    def quQ(self, q, D, Beff, Leff, Vload, HloadB, HloadL, psi= 0.0, eta= 0.0):
        '''Overburden component of the ultimate bearing capacity pressure 
           of the soil.

        :param q: overburden load.
        :param D: foundation depth.
        :param Beff: Width of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Leff: Length of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Vload: Vertical load. 
        :param HloadB: Horizontal load on Beff direction. 
        :param HloadL: Horizontal load on Leff direction. 
        :param psi: angle of the line on which the q load acts 
                    (see figure 4.7 in page 102 of reference [3])
                    must be determined by iterations.
        '''
        retval= 0.0
        if(q!=0.0):
            iq= self.iq(Vload= Vload, HloadB= HloadB, HloadL= HloadL)
            retval= q*self.Nq()*self.dq(D,Beff)*iq*self.sq(Beff,Leff)*self.tq(psi)*self.rq(eta)
        return retval
    
    def qu(self, q, D, Beff, Leff, Vload, HloadB, HloadL, NgammaCoef= 1.5,psi= 0.0, eta= 0.0):
        '''Ultimate bearing capacity pressure of the soil.

        :param q: overburden load.
        :param D: foundation depth.
        :param Beff: Width of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Leff: Length of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Vload: Vertical load (positive downwards). 
        :param HloadB: Horizontal load on Beff direction. 
        :param HloadL: Horizontal load on Leff direction. 
        :param NgammaCoef: 1.5 in reference [1], 1.8 in reference 2 
                           and 2 in reference 3
        :param psi: angle of the line on which the q load acts 
                    (see figure 4.7 in page 102 of reference [3])
                    must be determined by iterations.
        '''
        if(Vload<0.0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; negative vertical load (V= '+str(Vload)+') means pulling upwards.')
        # Body mass component.
        gammaComp= self.quGamma(D,Beff,Leff,Vload,HloadB,HloadL,NgammaCoef,psi,eta)
        # Overburden component.
        qComp= self.quQ(q= q, D= D, Beff= Beff, Leff= Leff, Vload= Vload, HloadB= HloadB, HloadL= HloadL, psi= psi, eta= eta)
        retval= gammaComp+qComp
        return retval
      
    def getMononobeOkabeDryOverpressure(self,H,kv,kh,psi= math.radians(90),delta_ad= 0.0,beta= 0.0,Kas= None,g= 9.81):
        ''' Overpressure due to seismic action according to Mononobe-Okabe

        :param H: height of the structure.
        :param kv: seismic coefficient of vertical acceleration.
        :param kh: seismic coefficient of horizontal acceleration.
        :param psi: back face inclination of the structure (< PI/2)
        :param beta: slope inclination of backfill.
        :param delta_ad: angle of friction soil - structure.
        :param Kas: static earth pressure coefficient 
        '''
        gamma_soil= self.rho*g
        phi_d= math.atan(math.tan(self.phi)/1.25)
        if(Kas is None):
            Kas= self.Ka()
        return mononobe_okabe.overpressure_dry(H, gamma_soil, kv, kh, psi, phi_d, delta_ad,beta,Kas)
  
    def getBulkModulus(self):
        ''' Return the bulk modulus of the soil.
        See https://en.wikipedia.org/wiki/Bulk_modulus
        '''
        return self.E / (3 * (1 - 2 * self.nu))

    def getShearModulus(self):
        ''' Return the shear modulus of the soil.'''
        return self.E / (2 * (1 + self.nu))
