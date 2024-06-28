# -*- coding: utf-8 -*-
''' Reinforced rectangular elastomeric bearing design according
    to EN 1337-3:2005.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
from misc_utils import log_messages as lmsg

def get_shape_factor(bearing, effectiveThickness):
    ''' Return the shape factor of the elastomer according to expression (3)
        of EN 1337-3:2005 (expression (3)).

    :param effectiveThickness: effective thickness of an individual elastomer layer in compression.
    '''
    aMinus2C= bearing.getEffectiveLength()
    bMinus2C= bearing.getEffectiveWidth()
    loadedArea= aMinus2C*bMinus2C
    forceFreePerimeter= 2*(aMinus2C+bMinus2C)
    return loadedArea/(forceFreePerimeter*effectiveThickness)
    

class RectangularLaminatedBearing:
    ''' Reinforced rectangular elastomeric bearing according to EN 1337-3:2005.

    :ivar a: bearing length (see figure 2 of EN 1337-3:2005).
    :ivar b: bearing width (see figure 2 of EN 1337-3:2005).
    :ivar tb: total height (see figure 2 of EN 1337-3:2005).
    :ivar ti: thickness of individual elastomer layer (see figure 2 of EN 1337-3:2005).
    :ivar ts: thickness of steel reinforcing plate (see figure 2 of EN 1337-3:2005).
    :ivar tso: thickness of outer steel reinforcing plate.
    :ivar Te: total elastomer thickness.
    :ivar Tb: total nominal thickness of bearing.
    :ivar n: number of elastomer layers.
    :ivar C: side cover.
    :ivar ted: edge cover.
    :ivar G: nominal value of conventional shear modulus of elastomeric bearing.
    :ivar gammaM: partial safety factor which value may be chosen in the 
                  National Annex. The recommended value is γm = 1.00.
    :ivar fy: yield stress of the reinforcing steel.
    '''
    def __init__(self, a, b, tb, ti, ts, tso, Te, Tb, n, C, ted, G= 0.9e6, gammaM= 1.0, fy= 235e6):
        ''' Constructor.

        :param a: bearing length (see figure 2 of EN 1337-3:2005).
        :param b: bearing width (see figure 2 of EN 1337-3:2005).
        :param tb: total height (see figure 2 of EN 1337-3:2005).
        :param ti: thickness of individual elastomer layer (see figure 2 of EN 1337-3:2005).
        :param ts: thickness of steel reinforcing plate (see figure 2 of EN 1337-3:2005).
        :param tso: thickness of outer steel reinforcing plate.
        :param Te: total elastomer thickness.
        :param Tb: total nominal thickness of bearing.
        :param n: number of elastomer layers.
        :param C: side cover.
        :param ted: edge cover.
        :param G: nominal value of conventional shear modulus of elastomeric 
                  bearing (seel table 1 of EN 1337-3:2005).
        :param gammaM: partial safety factor which value may be chosen in the 
                       National Annex. The recommended value is γm = 1.00.
        :param fy: yield stress of the reinforcing steel.
        '''
        self.a= a # bearing length (see figure 2 of EN 1337-3:2005).
        self.b= b # bearing width (see figure 2 of EN 1337-3:2005).
        self.tb= tb # total height (see figure 2 of EN 1337-3:2005).
        self.ti= ti # thickness of individual elastomer layer (see figure 2 of EN 1337-3:2005).
        self.ts= ts # thickness of steel reinforcing plate (see figure 2 of EN 1337-3:2005).
        self.tso= tso # thickness of outer steel reinforcing plate.
        self.Te= Te # total elastomer thickness.
        self.Tb= Tb # total nominal thickness of bearing.
        self.n= n # number of elastomer layers.
        self.C= C # side cover
        self.ted= ted # edge cover.
        self.G= G # nominal value of conventional shear modulus of elastomeric 
                  # bearing (seel table 1 of EN 1337-3:2005).
        self.gammaM= gammaM
        self.fy= fy

    def getNumberOfSteelLaminate(self):
        ''' Return the number of steel laminates.'''
        return n+1

    def getEffectiveLength(self):
        ''' Return the length of the steel plates.'''
        return self.a-2*self.C
    
    def getEffectiveWidth(self):
        ''' Return the width of the steel plates.'''
        return self.b-2*self.C
    
    def getEffectiveArea(self):
        ''' Return the effective area of the bearing (area of the steel 
            plates).
        '''
        return self.getEffectiveLength()*self.getEffectiveWidth()

    def getReducedEffectiveArea(self, vxd, vyd):
        ''' Return reduced effective plan area due to the loading effect
            according to expression (9) of clause 5.3.3.2 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        '''
        retval= self.getEffectiveArea()
        retval*= (1-vxd/self.getEffectiveLength()-vyd/self.getEffectiveWidth())
        return retval

    def getShapeFactorS1(self):
        ''' Return the shape factor of the elastomer considering inner layers
            according to clause  5.3.3.1 of EN 1337-3:2005 (expression (3)).
        '''
        return get_shape_factor(bearing= self, effectiveThickness= self.ti)
        
    def getShapeFactorS2(self):
        ''' Return the shape factor of the elastomer considering outer layer
            according to clause  5.3.3.1 of EN 1337-3:2005 (expression (3)).'''
        return get_shape_factor(bearing= self, effectiveThickness= 1.4*self.ted)

    def getShapeFactor(self, innerLayer= True):
        ''' Return the shape factor of the elastomer according to expression (3)
            of clause  5.3.3.1 of EN 1337-3:2005.

        :param innerLayer: if true compute the shape factor considering inner 
                           layer otherwise consider outer layer.
        '''
        if(innerLayer):
            retval= self.getShapeFactorS1()
        else:
            retval= self.getShapeFactorS2()
        return retval

    def getCompressiveStrain(self, vxd, vyd, Fzd, innerLayer= True):
        ''' Return the value of the compressive strain due to the given vertical
            design force according to expression (8) of clause 5.3.3.2 of 
            EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd: vertical design force.
        :param innerLayer: if true compute the shape factor considering inner 
                           layer otherwise consider outer layer.
        '''
        retval= 1.5*Fzd
        Ar= self.getReducedEffectiveArea(vxd= vxd, vyd= vyd)
        retval/= (self.G*Ar*self.getShapeFactor(innerLayer= innerLayer))
        return retval
    
    def getCompressiveStress(self, vxd, vyd, Fzd):
        ''' Return the value of the compressive stress due to the given vertical
            design force according to expression (15) of clause 5.3.3.6 of 
            EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd: vertical design force.
        '''
        Ar= self.getReducedEffectiveArea(vxd= vxd, vyd= vyd)
        return Fzd/Ar

    def getShearStrain(self, vxd, vyd):
        ''' Return the value of the shear strain due to the given horizontal
            displacements according to expression (10) of clause 5.3.3.3 of 
            EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        '''
        vxyd= math.sqrt(vxd**2+vyd**2)
        return vxyd/self.Te

    def getAllowableShearStrain(self):
        ''' Return the allowable shear strain according to the clause 5.3.3.3 of
            EN 1337-3:2005 (see NOTE).
        '''
        return 1.0

    def getAngularRotationStrain(self, alpha_ad, alpha_bd):
        ''' Return the design strain due to angular rotation according to 
            expression (11) of clause 5.3.3.4 of EN 1337-3:2005.

        :param alpha_ad: angle of rotation across the width, a, of the bearing.
        :param alpha_bd: angle of rotation (if any) across the length, b, of the bearing.
        '''
        retval= self.getEffectiveWidth()**2*alpha_ad+self.getEffectiveLength()**2*alpha_bd
        retval*= self.ti # thickness of individual elastomer layer
        retval/= self.n*2*self.ti**3
        return retval

    def getTotalStrain(self, vxd, vyd, Fzd, alpha_ad, alpha_bd):
        ''' Return the value of the total strain according to expression (1)
            of clause 5.3.3 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd: vertical design force.
        :param alpha_ad: angle of rotation across the width, a, of the bearing.
        :param alpha_bd: angle of rotation (if any) across the length, b, of the bearing.
        '''
        # Compute compressive strain.
        retval= self.getCompressiveStrain(vxd= vxd, vyd= vyd, Fzd= Fzd, innerLayer= True)
        retval+= self.getCompressiveStrain(vxd= vxd, vyd= vyd, Fzd= Fzd, innerLayer= False)
        # Compute shear strain.
        retval+= self.getShearStrain(vxd= vxd, vyd= vyd)
        # Compute strain due to angular rotation.
        retval+= self.getAngularRotationStrain(alpha_ad= alpha_ad, alpha_bd= alpha_bd)
        return retval
    
    def getAllowableTotalStrain(self):
        ''' Return the allowable strain according to expression (2)
            cl. 5.3.3 of EN 1337-3:2005.
        '''
        return 7.0/self.gammaM

    def getTotalStrainEfficiency(self, vxd, vyd, Fzd, alpha_ad, alpha_bd):
        ''' Return the value of the total strain according to expression (1)
            of clause 5.3.3 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd: vertical design force.
        :param alpha_ad: angle of rotation across the width, a, of the bearing.
        :param alpha_bd: angle of rotation (if any) across the length, b, of the bearing.
        '''
        totalStrain= self.getTotalStrain(vxd= vxd, vyd= vyd, Fzd= Fzd, alpha_ad= alpha_ad, alpha_bd= alpha_bd)
        return totalStrain/self.getAllowableTotalStrain()

    def getStrictReinforcedPlateThickness(self, vxd, vyd, Fzd, withHoles= False):
        ''' Return the value of the minimum plate thickness according to
            expression (12) of clause 5.3.3.5 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd: vertical design force.
        :param withHoles: true if the reinforcing plate has holes in it.
        '''
        Kp= 1.3 # stress correction factor.
        Kh= 1
        if(withHoles):
            Kh= 2
        retval= Kp*Fzd*2*self.ti*Kh*self.gammaM
        retval/= self.getReducedEffectiveArea(vxd= vxd, vyd= vyd)
        retval/= self.fy # yield stress of the reinforcing steel.
        return retval
    
    def getRequiredReinforcedPlateThickness(self, vxd, vyd, Fzd, withHoles= False):
        ''' Return the value of the minimum plate thickness according to
            expression (12) of clause 5.3.3.5 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd: vertical design force.
        :param withHoles: true if the reinforcing plate has holes in it.
        '''
        retval= self.getStrictReinforcedPlateThickness(vxd= vxd, vyd= vyd, Fzd= Fzd, withHoles= withHoles) 
        return max(retval, 2e-3)

    def getPlateThicknessEfficiency(self, vxd, vyd, Fzd, withHoles= False):
        ''' Return the efficiency of the plate thickness according to
            expression (12) of clause 5.3.3.5 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd: vertical design force.
        :param withHoles: true if the reinforcing plate has holes in it.
        '''
        reqThk= self.getRequiredReinforcedPlateThickness(vxd= vxd, vyd= vyd, Fzd= Fzd, withHoles= withHoles)
        return reqThk/self.ts

    def getBucklingLimitStress(self):
        ''' Return the limit stress to avoid buckling instability according to 
            expression (15) of clause 5.3.3.6 of EN 1337-3:2005.
        '''
        S1= self.getShapeFactorS1()
        retval= 2*self.getEffectiveLength()*self.G*S1
        retval/= (3*self.Te)
        return retval

    def getBucklingEfficiency(self, vxd, vyd, Fzd):
        ''' Return the value of the compressive stress due to the given vertical
            design force according to expression (15) of clause 5.3.3.6 of 
            EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd: vertical design force.
        '''
        sigma_z= self.getCompressiveStress(vxd= vxd, vyd= vyd, Fzd= Fzd)
        return sigma_z/self.getBucklingLimitStress()

    def getTotalVerticalDeflection(self, vxd, vyd, Fzd):
        ''' Return the total vertical deflection of a laminated bearing 
            according to expression (20) of clause 5.3.3.7 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd: vertical design force.
        '''
        Eb= 2000e6
        sigma= self.getCompressiveStress(vxd= vxd, vyd= vyd, Fzd= Fzd)
        S= self.getShapeFactorS1()
        retval= 1/(5*self.G*S**2)
        retval+= 1/Eb
        retval*= self.n*sigma*self.ti
        return retval

    def getRotationalDeflection(self, alpha_ad, alpha_bd):
        ''' Return the rotational deflection due to angular rotation according
            to  expression (13) of clause 5.3.3.6 of EN 1337-3:2005.

        :param alpha_ad: angle of rotation across the width, a, of the bearing.
        :param alpha_bd: angle of rotation (if any) across the length, b, of the bearing.
        '''
        Krd= 3
        return (self.getEffectiveWidth()*alpha_ad+self.getEffectiveLength()*alpha_bd)/Krd

    def getRotationalLimitEfficiency(self, vxd, vyd, Fzd, alpha_ad, alpha_bd):
        ''' Return the efficiency with respect the rotational deflection limit
            according to  expression (13) of clause 5.3.3.6 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd: vertical design force.
         :param alpha_ad: angle of rotation across the width, a, of the bearing.
        :param alpha_bd: angle of rotation (if any) across the length, b, of the bearing.
        '''
        rotDef= self.getRotationalDeflection(alpha_ad= alpha_ad, alpha_bd= alpha_bd)
        Vzd= self.getTotalVerticalDeflection(vxd= vxd, vyd= vyd, Fzd= Fzd)
        return rotDef/Vzd
    
    def getPermCompressiveStressEfficiency(self, vxd, vyd, Fzd_min_perm):
        ''' Return the efficiency with respect to the limit compressive stress
            according to expression (16) of clause 5.3.3.6 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd_min_perm: minimum vertical design force under permanent loads.
        '''
        sgLim= 3.0
        retval= self.getCompressiveStress(vxd= vxd, vyd= vyd, Fzd= Fzd_min_perm)/1e6
        if(retval>sgLim): # sigma > sgLim MPa => OK
            retval= sgLim/retval
        else: # sigma > sgLim MPa => OK
            retval= retval/sgLim
        return retval

    def getFrictionCoefficient(self, vxd, vyd, Fzd_min, concreteBedding= True):
        ''' Return the friction coefficient according to clause 5.3.3.6 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd_min: vertical design force.
        :param concreteBedding: true if the bedding material is concrete, false otherwise.
        '''
        Kf= 0.6
        if(not concreteBedding):
            Kf= 0.2
        sigma= self.getCompressiveStress(vxd= vxd, vyd= vyd, Fzd= Fzd_min)/1e6
        return 0.1+ 1.5*Kf/sigma
        
    def getFrictionForce(self, vxd, vyd, Fzd_min, concreteBedding= True):
        ''' Return the friction force according to clause 5.3.3.6 of EN 1337-3:2005.

        :param vxd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "a" (length) of 
                    the bearing due to all design load effects.
        :param vyd: maximum horizontal relative displacement of parts of the
                    bearing in the direction of dimension "b" (width) of the 
                    bearing due to all design load effects.
        :param Fzd_min: minimum vertical design force coexisting with Fxd and Fyd.
        :param concreteBedding: true if the bedding material is concrete, false otherwise.
        '''
        frictionCoeff= self.getFrictionCoefficient(vxd= vxd, vyd= vyd, Fzd_min= Fzd_min, concreteBedding= concreteBedding)
        return frictionCoeff*Fzd_min
        
    def getSlidingConditionEfficiency(self, vxd, vyd, Fxd, Fyd, Fzd_min, concreteBedding= True):
        ''' Return the friction coefficient according to clause 5.3.3.6 of EN 1337-3:2005.

        :param Fxd, Fyd: resultant of all the horizontal forces.
        :param Fzd_min: minimum vertical design force coexisting with Fxyd.
        :param concreteBedding: true if the bedding material is concrete, false otherwise.
        '''
        frictionForce= self.getFrictionForce(vxd= vxd, vyd= vyd, Fzd_min= Fzd_min, concreteBedding= concreteBedding)
        return math.sqrt(Fxd**2+Fyd**2)/frictionForce
