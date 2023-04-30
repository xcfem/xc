# -*- coding: utf-8 -*-
'''frictional_soil.py: frictional (or cohesionless) soil model.'''

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
    def __init__(self, phi:float, rho= 2100.0, rhoSat= None, gammaMPhi= 1.0, E= 1e8, nu= 0.3):
        '''Constructor.

        :param phi: (float) internal friction angle of the soil
        :param rho: (float) soil density (mass per unit volume)
        :param rhoSat: saturated density of the soil (mass per unit volume)
        :param gammaMPhi: (float) partial reduction factor for internal 
                          friction angle of the soil.
        :param E: Young's modulus (defaults to 1e8 Pa).
        :param nu: Poisson's ratio (defaults to 0.3).
        '''
        self.phi= phi
        self.gammaMPhi= gammaMPhi
        self.rho= rho
        self.rhoSat= rhoSat
        self.E= E
        self.nu= nu

    def K0Jaky(self):
        '''Returns Jaky's coefficient (earth pressure at rest).'''
        return 1.0-math.sin(self.phi)

    def Ka(self):
        '''Active earth pressure coefficient.'''
        sinPhi= math.sin(self.phi)
        return (1-sinPhi)/(1+sinPhi)
      
    def Kp(self):
        '''Passive earth pressure coefficient.'''
        sinPhi= math.sin(self.phi)
        return (1+sinPhi)/(1-sinPhi)
      
    def Ka_coulomb(self, a, b, d= 0.0):
        '''
        Return the active earth pressure coefficient according to Coulomb's theory (see Recomendaciones para obras marítimas ROM 0.5-05 figure 3.7.12.).

        :param a:  angle of the back of the retaining wall (radians, 0 if vertical).
        :param b:  slope of the backfill (radians, 0 if horizontal).
        :param d:  friction angle between soil and the back of retaining wall (radians).
        '''
        phi= self.phi
        num= 1.0/math.cos(a)*math.cos(phi-a)
        r1=math.sqrt(math.cos(a+d))
        if(b>phi):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; the angle of the backfill: '+str(math.degrees(b))+' is greater than the friction angle: '+str(math.degrees(phi)))
        r2=math.sqrt(math.sin(phi+d)*math.sin(phi-b)/math.cos(b-a))
        return (math.pow((num/(r1+r2)),2))

    def Kah_coulomb(self,a,b,d):
        '''
        Return the horizontal component of the active earth pressure coefficient
        according to Coulomb's theory.

        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param d:  friction angle between soil an back of retaining wall (radians).
        '''
        return (self.Ka_coulomb(a,b,d)*math.cos(a+d))
      
    def Kav_coulomb(self,a,b,d):
        '''
        Return the vertical component of the active earth pressure coefficient
        according to Coulomb's theory.

        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param phi: internal friction angle of the soil (radians).
        :param d:  friction angle between soil an back of retaining wall (radians).
        '''
        return (self.Ka_coulomb(a,b,d)*math.sin(a+d))

    def Kp_coulomb(self, a, b, d= 0.0):
        '''
        Return the passive earth pressure coefficient according to Coulomb's theory.

        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param d:  friction angle between soil an back of retaining wall (radians).
        '''
        phi= self.phi
        if(b>phi):
            lmsg.error('The angle of the backfill: '+str(math.degrees(b))+' is greater than the friction angle: '+str(math.degrees(phi)))
        p1= 1.0/(math.cos(a)**2)
        num= math.cos(phi+a)
        r1=math.sqrt(math.sin(phi-d)*math.sin(phi+b)/(math.cos(a+d)*math.cos(b-a)))
        denom= 1.0-r1
        retval= p1*((num/denom)**2)/math.cos(a+d)
        return retval

    def eq_coulomb(self, a, b, d, p):
        '''
        eq_coulomb(a,b,d,p):
        Return the lateral earth pressure caused by a uniform load q
        action over the backfill surface according to Coulomb's theory.

        :param a: angle of the back of the retaining wall (radians).
        :param b: slope of the backfill (radians).
        :param d: friction angle between soil an back of retaining wall (radians).
        :param p: Uniform load.
        '''
        return(self.Ka_coulomb(a,b,d)*p*math.cos(a)/float(math.cos(b-a)))
      
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
        return self.phi/self.gammaMPhi
      
    def getDesignC(self):
        '''Return the design value of the soil cohesion.'''
        return 0.0
      
    def getMononobeOkabeDryOverpressure(self,H,kv,kh,psi= math.radians(90),delta_ad= 0.0,beta= 0.0,Kas= None,g= 9.81):
        ''' Overpressure due to seismic action according to Mononobe-Okabe

            Args:
            :H: height of the structure.
            :kv: seismic coefficient of vertical acceleration.
            :kh: seismic coefficient of horizontal acceleration.
            :psi: back face inclination of the structure (< PI/2)
            :beta: slope inclination of backfill.
            :delta_ad: angle of friction soil - structure.
            :Kas: static earth pressure coefficient 
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
