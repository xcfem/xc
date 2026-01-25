# -*- coding: utf-8 -*-
'''frictional_cohesive_soil.py: soil with friction and cohesion.

References: 

[1] Chapter 4-3 of Foundation Analysis and Design, Ed. 5 by Joseph E. Bowles.
[2] Brinch Hansen. A general formula for bearing capacity. The Danish Geotechnical Institute. Bulletin 11. Copenhagen 1961.
[3] Guía de cimentaciones en obras de carretera. Ministerio de Fomento (spain). 2002 (https://books.google.ch/books?id=a0eoygAACAAJ).
[4] Eurocódigo 7. Proyecto geotécnico. Parte 1: reglas generales.
'''
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import math
from geotechnics import frictional_soil as fs
from misc_utils import log_messages as lmsg
from scipy.optimize import fminbound
from materials import typical_materials

#   Schema for Brinch-Hansen formulas
#
#                           B
#              Beff
#         +-----------+-----------------------+
#         |             |                     |  
#         |     ^HloadL |                     |
#       L |     |       |                     |  
#       e | <-- +       |                     |
#       f | HloadB      |                     |  
#       f |             |                     |
#         |             |                     |  
#         +-------------+---------------------+
#         |             |                     |
#         |             |                     |  
#         |             |                     |
#   L     |             |                     |  
#         |             |                     |
#         |             |                     |  
#         |             |                     |
#         |             |                     |  
#         |             |                     |
#         |             |                     |  
#         |             |                     |
#         |             |                     |  
#         +-----------+-----------------------+
#

class FrictionalCohesiveSoil(fs.FrictionalSoil):
    '''Soil with friction and cohesion

    :ivar c:    soil cohesion
    '''
    def __init__(self, phi, c, rho= 2100.0, rhoSat= None, phi_cv= None, gammaMPhi= 1.0, gammaMc= 1.0, E= 1e8, nu= 0.3):
        '''Constructor.

        :param phi: (float) internal friction angle of the soil
        :param c: (float) soil cohesion.
        :param rho: (float) soil density (mass per unit volume)
        :param rhoSat: saturated density of the soil (mass per unit volume).
        :param phi_cv: critical state (constant volume) angle of shearing resistance of the soil. See clause 6.5.3 (10) of Eurocode 7 part 1. 
        :param gammaMPhi: (float) partial reduction factor for internal 
                          friction angle of the soil.
        :param gammaMc: (float) partial reduction factor for soil cohesion.
        :param E: Young's modulus (defaults to 1e8 Pa).
        :param nu: Poisson's ratio (defaults to 0.3).
        '''
        super(FrictionalCohesiveSoil,self).__init__(phi= phi, rho= rho, rhoSat= rhoSat, phi_cv= phi_cv, gammaMPhi= gammaMPhi, E= E, nu= nu)
        self.c= c
        self.gammaMc= gammaMc

    def getDesignC(self):
        '''Return the design value of the soil cohesion.'''
        return self.c/self.gammaMc

    def Ka_bell(self, sg_v, a, b, d= 0.0, designValue= False):
        ''' Return the active earth pressure coefficient according to Bell's
        relationship. See: https://en.wikipedia.org/wiki/Lateral_earth_pressure#Rankine's_earth_pressure_coefficients_and_Bell's_extension_for_cohesive_soils

        :param sg_v:  vertical stress.
        :param a: angle of the back of the retaining wall (radians).
        :param b: slope of the backfill (radians).
        :param d: friction angle between soil an back of retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction and the cohesion.
        '''
        retval= 0.0
        if(sg_v>0.0):
            ka= super(FrictionalCohesiveSoil,self).Ka_coulomb(a,b,d, designValue= designValue)
            a1= ka*sg_v
            if(designValue):
                c= self.getDesignC()
            else:
                c= self.c
            a2= 2.0*c*math.sqrt(ka)
            retval= max(a1-a2,0.0)/sg_v
        return retval
    
    def Kah_bell(self, sg_v, a, b, d= 0.0, designValue= False):
        ''' Return horizontal component of the the active earth pressure 
            coefficient according to Bell's relationship. See: https://en.wikipedia.org/wiki/Lateral_earth_pressure#Rankine's_earth_pressure_coefficients_and_Bell's_extension_for_cohesive_soils

        :param sg_v: vertical stress.
        :param a: angle of the back of the retaining wall (radians).
        :param b: slope of the backfill (radians).
        :param d: friction angle between soil an back of retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction and the cohesion.
        '''
        retval= 0.0
        if(sg_v>0.0):
            kah= super(FrictionalCohesiveSoil,self).Kah_coulomb(a,b,d, designValue= designValue)
            a1= kah*sg_v
            if(designValue):
                c= self.getDesignC()
            else:
                c= self.c
            a2= 2.0*c*math.sqrt(kah)
            retval= max(a1-a2,0.0)/sg_v
        return retval

    # def Kp_bell(self, sg_v, a, b, d= 0.0, designValue= False):
    #     ''' Return the passive earth pressure coefficient according to Bell's
    #     relationship.

    #     :param sg_v:  vertical stress.
    #     :param a: angle of the back of the retaining wall (radians).
    #     :param b: slope of the backfill (radians).
    #     :param d: friction angle between soil an back of retaining 
    #               wall (radians).
    #     :param designValue: if true use the design value of the internal
    #                         friction and the cohesion.
    #     '''
    #     retval= 0.0
    #     if(sg_v>0.0):
    #         kp= super(FrictionalCohesiveSoil,self).Kp_coulomb(a,b,d, designValue= designValue)
    #         a1= kp*sg_v
    #         if(designValue):
    #             c= self.getDesignC()
    #         else:
    #             c= self.c
    #         a2= 2.0*c*math.sqrt(kp)
    #         retval= max(a1+a2,0.0)/sg_v
    #     return retval

    def getCoulombTensionCrackDepth(self, sg_v, a, b, d= 0.0, designValue= False):
        ''' Return the depth of the tension crack (the depth at which
            active lateral earth pressure is cero due to soil cohesion).

        :param sg_v: vertical stress.
        :param a: angle of the back of the retaining wall (radians).
        :param b: slope of the backfill (radians).
        :param d: friction angle between the soil and the back surface of the
                  retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction and the cohesion.
        '''
        ka= super(FrictionalCohesiveSoil,self).Ka_coulomb(a,b,d, designValue= designValue)
        if(designValue):
            c= self.getDesignC()
        else:
            c= self.c
        return max((2.0*c/math.sqrt(ka)-sg_v)/self.gamma(), 0.0)
    
    def ea_coulomb(self, sg_v, a, b, d= 0.0, designValue= False):
        ''' Return the lateral earth active pressure.

        :param sg_v:  vertical stress.
        :param a:  angle of the back of the retaining wall (radians).
        :param b:  slope of the backfill (radians).
        :param d:  friction angle between soil and the back surface of the 
                   retaining wall (radians). See Jiménez Salas, Geotecnia y 
                   Cimientos page 682 and Bell's relationship.
        :param designValue: if true use the design value of the internal
                            friction and the cohesion.
        '''
        retval= 0.0
        if(sg_v>0.0):
            ka= super(FrictionalCohesiveSoil,self).Ka_coulomb(a,b,d, designValue= designValue)
            a1= ka*sg_v
            if(designValue):
                c= self.getDesignC()
            else:
                c= self.c
            a2= 2.0*c*math.sqrt(ka)
            retval= max(a1-a2,0.0)
        return retval

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
                            friction and the cohesion.
        '''
        retval= 0.0
        kp= super(FrictionalCohesiveSoil,self).Kp_coulomb(a,b,d, designValue= designValue)
        a1= max(kp*sg_v, 0.0) # No tension.
        if(designValue):
            c= self.getDesignC()
        else:
            c= self.c
        a2= 2.0*c*math.sqrt(kp)
        retval= max(a1+a2,0.0) # No tension.
        return retval

    def eq_bell(self, q, a, b, d, designValue= False):
        ''' eq_bell(q,a,b,d):
        Return the lateral earth pressure caused by a uniform load q
        action over the backfill surface according to Coulomb's theory.

        :param q: Uniform load.
        :param a: angle of the back of the retaining wall (radians).
        :param b: slope of the backfill (radians).
        :param d:  friction angle between soil and the back surface of the 
                   retaining wall (radians).
        :param designValue: if true use the design value of the internal
                            friction and the cohesion.
        '''
        return(self.Ka_bell(sg_v= q, a= a, b= b, d= d, designValue= designValue)*q*math.cos(a)/float(math.cos(b-a)))
    
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
        return self.eq_bell(q= q, a= a, b= b, d= d, designValue= designValue)


    def sc(self, Beff, Leff):
        '''Factor that introduces the effect of foundation shape on
           the cohesion component.

        :param Beff: Width of the effective foundation area
                    (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Leff: Length of the effective foundation area
                    (see figure 12 in page 44 (8 in the PDF) of reference 2).
        '''
        return self.sq(Beff,Leff)

    def ic(self, Vload, HloadB, HloadL, Beff, Leff):
        '''Factor that introduces the effect of load inclination on
           the cohesion component.

        :param Vload: Vertical load. 
        :param HloadB: Horizontal load on Beff direction. 
        :param HloadL: Horizontal load on Leff direction. 
        :param Beff: Width of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Leff: Length of the effective foundation area
                    (see figure 12 in page 44 (8 in the PDF) of reference 2).
        '''
        if(self.getDesignPhi()!=0.0):
            iq= self.iq(Vload= Vload, HloadB= HloadB, HloadL= HloadL)
            return (iq*self.Nq()-1.0)/(self.Nq()-1.0)
        else: #See expresion (15) in reference [2]
            Hload= math.sqrt(HloadB**2+HloadL**2)
            resist= Beff*Leff*self.getDesignC()
            if(Hload<=resist):
                twoAlpha= math.acos(Hload/resist)
                return 0.5+(twoAlpha+math.sin(twoAlpha))/(math.pi+2.0)
            else:
                lmsg.warning('Load (H= '+str(Hload)+') greater than soil strength R='+str(resist)+' returns 0.0')
                return 0.0

    def dc(self, D, Beff):
        '''Depth factor for cohesion.

           :param D: foundation depth.
           :param Beff: Width of the effective foundation area
                        (see figure 12 in page 44 (8 in the PDF) of reference 2).
        '''
        k= min(D,2.0*Beff)/Beff
        return 1+2*self.Nq()/self.Nc()*(1-math.sin(self.getDesignPhi()))**2*math.atan(k)

    def tc(self, psi= 0.0):
        '''Factor that introduces the effect of the proximity of an slope.

           :param psi: angle of the line on which the q load acts 
                       (see figure 4.7 in page 102 of reference [3])
                       must be determined by iterations.
        '''
        if(self.getDesignPhi()!=0.0):
            return (self.tq(psi)*self.Nq()-1.0)/(self.Nq()-1.0)
        else:
            return 1-0.4*psi

    def rc(self,eta= 0.0):
        '''Factor that introduces the effect of sloped footing.

           :param eta: angle between the foundation plane with and the 
                       (see figure 4.8 in page 104 of reference [3])
                       favourable effect when eta<0.0.
        '''
        if(self.getDesignPhi()!=0.0):
            return (self.rq(eta)*self.Nq()-1.0)/(self.Nq()-1.0)
        else:
            return 1-0.4*eta

    def quCohesion(self, D, Beff, Leff, Vload, HloadB, HloadL, psi= 0.0, eta= 0.0):
        '''Cohesion "component" of the ultimate bearing capacity pressure of the soil.

        :param D: foundation depth.
        :param Beff: Width of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Leff: Length of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param Vload: Vertical load. 
        :param HloadB: Horizontal load on Beff direction. 
        :param HloadL: Horizontal load on Leff direction. 
        :param NgammaCoef: 1.5 in reference [1], 1.8 in reference 2 
                           and 2 in reference 3
        :param psi: angle of the line on which the q load acts 
                    (see figure 4.7 in page 102 of reference [3])
                    must be determined by iterations.
        '''
        return self.getDesignC()*self.Nc()*self.dc(D,Beff)*self.ic(Vload= Vload, HloadB= HloadB, HloadL= HloadL, Beff= Beff, Leff= Leff)*self.sc(Beff,Leff)*self.tc(psi)*self.rc(eta)

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
             lmsg.warning('Negative vertical load (V= '+str(Vload)+') means pulling upwards.')
        # Body mass component.
        gammaComp= self.quGamma(D,Beff,Leff,Vload,HloadB,HloadL,NgammaCoef,psi,eta)
        cComp= self.quCohesion(D,Beff,Leff,Vload,HloadB,HloadL,psi,eta)
        # Overburden component.
        qComp= self.quQ(q= q, D= D, Beff= Beff, Leff= Leff, Vload= Vload, HloadB= HloadB, HloadL= HloadL, psi= psi, eta= eta)
        retval= gammaComp+cComp+qComp
        return retval

    def getElasticIsotropicMaterialPlaneStrain(self, preprocessor, name):
        ''' Defines returns a Drucker-Prager material.

        :param preprocessor: pre-processor or the finite element problem.
        :param name: material identifier.
        '''
        return typical_materials.defElasticIsotropicPlaneStrain(preprocessor, name= name, E= self.E , nu= self.nu, rho= self.rho)

    def getDruckerPragerYieldStress(self):
        ''' Convert Mohr-Coulomb cohesion and friction angle to Drucker-Prager yield stress (sigma_y).'''
        # convert Mohr-Coulomb phi and c to Drucker-Prager
        return (6 * self.c * math.cos(self.phi)) / (math.sqrt(3) * (3 - math.sin(self.phi)) )
    
    def getDruckerPragerRho(self):
        ''' Convert Mohr-Coulomb cohesion and friction angle to Drucker-Prager rho.'''
        # convert Mohr-Coulomb phi and c to Drucker-Prager failure surface and associativity volumetric term (rho).
        return (2 * math.sqrt(2) * math.sin(self.phi)) / (math.sqrt(3) * (3 - math.sin(self.phi)) )

    def getDruckerPragerMaterial3d(self, preprocessor, name, rhoBar= None, Kinf= 0.0, Ko= 0.0, delta1= 0.0, H= 0.0, theta= 0.0, delta2= 0.0, elastFlag= 2, pAtm= 101.325e3):
        ''' Defines returns a Drucker-Prager material.

        :param preprocessor: pre-processor or the finite element problem.
        :param name: material identifier.
        :param rhoBar: related to dilation? controls evolution of plastic volume change, 0 ≤ rhoBar ≤ rho (defaults to 0).
        :param Kinf: isotropic hardening Kinf ≥ 0 (defaults to 0).
        :param Ko: nonlinear isotropic strain hardening parameter, Ko ≥ 0 (defaults to 0).
        :param delta1: nonlinear isotropic strain hardening parameter, delta1 ≥ 0 (defaults to 0).
        :param H: linear strain hardening parameter, H ≥ 0 (defaults to 0)
        :param theta: controls relative proportions of isotropic and kinematic hardening, 0 ≤ theta ≤ 1 (defaults to 0).
        :param delta2: tension softening parameter, delta2 ≥ 0 (defaults to 0).
        :param elastFlag: Flag to determine elastic behavior 0 = elastic+no param update; 1 = elastic+param update; 2 = elastoplastic+no param update (default)
        :param pAtm: reference pressure (defaults to one atmosphere).
        '''
        #---bulk modulus
        k= self.getBulkModulus()
        #---shear modulus
        G= self.getShearModulus()
        #---yield stress
        sigY= self.getDruckerPragerYieldStress()
        #---failure surface and associativity
        rho= self.getDruckerPragerRho()
        if(rhoBar is None):
            rhoB= rho
        else:
            rhoB= rhoBar # related to dilation? controls evolution of plastic volume change, 0 ≤ rhoBar ≤ rho
        #---isotropic hardening
        Kinf= Kinf 
        Ko= Ko
        delta1= delta1
        #---kinematic hardening
        H= 0.0 # linear strain hardening parameter
        theta= theta # controls relative proportions of isotropic and kinematic hardening, 0 ≤ theta ≤ 1
        #---tension softening
        delta2= delta2 # tension softening parameter, delta2 ≥ 0
        #---mass density
        mDen= self.rho
        if(self.rhoSat):
            mDen= self.rhoSat

        return typical_materials.defDruckerPrager3d(preprocessor, name= name,k= k, G= G, sigY= sigY, mRho= rho, mRhoBar= rhoB, Kinf= Kinf, Ko= Ko, delta1= delta1, H= H, theta= theta, delta2= delta2, mDen= mDen, elastFlag= elastFlag, pAtm= pAtm)
    
    def getDruckerPragerMaterialPlaneStrain(self, preprocessor, name, rhoBar= 0.0, Kinf= 0.0, Ko= 0.0, delta1= 0.0, H= 0.0, theta= 0.0, delta2= 0.0, elastFlag= 2, pAtm= 101.325e3):
        ''' Defines returns a Drucker-Prager material.

        :param preprocessor: pre-processor or the finite element problem.
        :param name: material identifier.
        :param rhoBar: related to dilation? controls evolution of plastic volume change, 0 ≤ rhoBar ≤ rho (defaults to 0).
        :param Kinf: isotropic hardening Kinf ≥ 0 (defaults to 0).
        :param Ko: nonlinear isotropic strain hardening parameter, Ko ≥ 0 (defaults to 0).
        :param delta1: nonlinear isotropic strain hardening parameter, delta1 ≥ 0 (defaults to 0).
        :param H: linear strain hardening parameter, H ≥ 0 (defaults to 0)
        :param theta: controls relative proportions of isotropic and kinematic hardening, 0 ≤ theta ≤ 1 (defaults to 0).
        :param delta2: tension softening parameter, delta2 ≥ 0 (defaults to 0).
        :param elastFlag: Flag to determine elastic behavior 0 = elastic+no param update; 1 = elastic+param update; 2 = elastoplastic+no param update (default)
        :param pAtm: reference pressure (defaults to one atmosphere).
        '''
        #---bulk modulus
        k= self.getBulkModulus()
        #---shear modulus
        G= self.getShearModulus()
        #---yield stress
        sigY= self.getDruckerPragerYieldStress()
        #---failure surface and associativity
        rho= self.getDruckerPragerRho()
        if(rhoBar is None):
            rhoB= rho
        else:
            rhoB= rhoBar # related to dilation? controls evolution of plastic volume change, 0 ≤ rhoBar ≤ rho
        #---isotropic hardening
        Kinf= Kinf 
        Ko= Ko
        delta1= delta1
        #---kinematic hardening
        H= 0.0 # linear strain hardening parameter
        theta= theta # controls relative proportions of isotropic and kinematic hardening, 0 ≤ theta ≤ 1
        #---tension softening
        delta2= delta2 # tension softening parameter, delta2 ≥ 0
        #---mass density
        mDen= self.rho
        if(self.rhoSat):
            mDen= self.rhoSat

        return typical_materials.defDruckerPragerPlaneStrain(preprocessor, name= name, k= k, G= G, sigY= sigY, mRho= rho, mRhoBar= rhoB, Kinf= Kinf, Ko= Ko, delta1= delta1, H= H, theta= theta, delta2= delta2, mDen= mDen, elastFlag= elastFlag, pAtm= pAtm)

class StratifiedSoil(object):
    '''Soil with layers of different properties.

    Soil with layers of different properties as described in
    4.5.5.7 "Guía de cimentaciones en obras de carreteras"
    (https://books.google.ch/books?id=a0eoygAACAAJ)
    2009

    :ivar hi: (float list) layer thicknesses.
    :ivar rhoi: (float list) layer densities.
    :ivar phii: (float list) layer internal friction angle.
    :ivar ci: (float list) layer cohesions.
    '''
    def __init__(self,hi,rhoi,phii,ci):
        '''Constructor.

        :param hi: (float list) layer thicknesses.
        :param rhoi: (float list) layer densities.
        :param phii: (float list) layer internal friction angle.
        :param ci: (float list) layer cohesions.
        '''
        self.hi= hi
        self.rhoi= rhoi
        self.phii= phii
        self.ci= ci

    def getAffectedHeights(self, affectedDepth):
        '''Returns the layers at depths less than affectedDepth.

        :param affectedDepth: depth affected by pressures on foundation soil.
        '''
        currentDepth= 0.0
        retval= list()
        for h in self.hi:
            currentDepth+= h
            if(currentDepth<affectedDepth):
                retval.append(h)
            else:
                retval.append(affectedDepth-(currentDepth-h)) # Previous depth.
                currentDepth= affectedDepth
                break
        if(currentDepth<affectedDepth):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; soil layers are not deep enough '+str(currentDepth) + ' < '+str(affectedDepth))
        return retval
    
    def getEquivalentRho(self,affectedDepth):
        '''Return equivalent density.'''
        affectedHeights= self.getAffectedHeights(affectedDepth)
        sz= len(affectedHeights)
        retval= 0.0
        for i in range(0,sz):
            h= affectedHeights[i]
            rho= self.rhoi[i]
            retval+= h*rho
        retval/= affectedDepth
        return retval
    
    def getEquivalentC(self,affectedDepth):
        '''Return equivalent cohesion.'''
        affectedHeights= self.getAffectedHeights(affectedDepth)
        sz= len(affectedHeights)
        retval= 0.0
        for i in range(0,sz):
            h= affectedHeights[i]
            c= self.ci[i]
            retval+= h*c
        retval/= affectedDepth
        return retval
    
    def getEquivalentPhi(self,affectedDepth):
        '''Return equivalent internal friction angle.'''
        affectedHeights= self.getAffectedHeights(affectedDepth)
        sz= len(affectedHeights)
        retval= 0.0
        for i in range(0,sz):
            h= affectedHeights[i]
            phi= self.phii[i]
            retval+= h*math.log(math.tan(phi))
        retval/= affectedDepth
        retval= math.exp(retval)
        retval= math.atan(retval)
        return retval

    def computeAffectedDepth(self, Beff):
        '''Computes affected depth.

        :Beff: (float) width of the effective foundation area
               (see figure 12 in page 44 (8 in the PDF) of reference 2).
        '''
        # define the acceptable range for Beff
        Bmin= Beff
        Bmax= 3.0*Beff
        
        # # define the starting guess 
        # start_guess= (Bmin+Bmax)/2.0

        # minimize 
        return fminbound(self.getEquivalentPhi, Bmin, Bmax)

    def getEquivalentSoil(self,Beff,gMPhi,gMc):
        '''Computes affected depth.

        :param Beff: (float) width of the effective foundation area
                     (see figure 12 in page 44 (8 in the PDF) of reference 2).
        :param gammaMPhi: (float) partial reduction factor for internal 
                          friction angle of the soil.
        :param gammaMc: (float) partial reduction factor for soil cohesion.
        '''
        H= self.computeAffectedDepth(Beff)
        equivalentRho= self.getEquivalentRho(H)
        equivalentC= self.getEquivalentC(H)
        equivalentPhi= self.getEquivalentPhi(H)
        return FrictionalCohesiveSoil(phi= equivalentPhi,c=equivalentC,rho= equivalentRho,gammaMPhi= gMPhi,gammaMc= gMc)
    
