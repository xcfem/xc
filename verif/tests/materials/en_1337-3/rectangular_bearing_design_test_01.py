# -*- coding: utf-8 -*-
''' Test of reinforced rectangular elastomeric bearing design according
    to EN 1337-3:2005.

    Data obtained from a calcuation sample from Mageba.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math

class EN1337RectangularLaminatedBearing:
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
        ''' Return the shape factor of the elastomer considering inner layer
            according to clause  5.3.3.1 of EN 1337-3:2005 (expression (3)).'''
        aMinus2C= self.getEffectiveLength()
        bMinus2C= self.getEffectiveWidth()
        factor= 0.5*(aMinus2C*bMinus2C)/(aMinus2C+bMinus2C)
        return factor/self.ti
        
    def getShapeFactorS2(self):
        ''' Return the shape factor of the elastomer considering outer layer
            according to clause  5.3.3.1 of EN 1337-3:2005 (expression (3)).'''
        aMinus2C= self.getEffectiveLength()
        bMinus2C= self.getEffectiveWidth()
        factor= 0.5*(aMinus2C*bMinus2C)/(aMinus2C+bMinus2C)
        return factor/(1.4*self.ted)

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
        print(retval*1e3)
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
       

        
    
# Maximum displacements.
vxd= 141.8e-3 # m
vyd= 33.5e-3 # m
# Rotations.
alpha_ad= 0.0030 # rad
alpha_bd= 0.0 # rad
# Loads.
Fzd= 2746.8e3 # design vertical load.

bearing= EN1337RectangularLaminatedBearing(a= 0.55, b= 0.5, tb= 0.172, ti= 0.011, ts= 2e-3, Te= 0.146, tso= 0.0, Tb= 0.172, n= 12, C= 5e-3, ted= 7e-3)

# Compute effective area.
effectiveArea= bearing.getEffectiveArea()
ratio1= abs(effectiveArea-264600.0e-6)/264600.0e-6
# Compute reduced effective area.
reducedArea= bearing.getReducedEffectiveArea(vxd= vxd, vyd= vyd)
ratio2= abs(reducedArea-177027e-6)/177027e-6
# Shape factor.
S1= bearing.getShapeFactorS1()
ratio3= abs(S1-11.676963812886143)/11.676963812886143
S2= bearing.getShapeFactorS2()
ratio4= abs(S2-13.106796116504853)/13.106796116504853
# Compute compressive strain
eps_cd= bearing.getCompressiveStrain(vxd= vxd, vyd= vyd, Fzd= Fzd)
ratio5= abs(eps_cd-2.2146438317891426)/2.2146438317891426
# Compute shear strain.
eps_qd= bearing.getShearStrain(vxd= vxd, vyd= vyd)
ratio6= abs(eps_qd-0.997968710058754)/0.997968710058754
# Compute strain due to angular rotation.
eps_alphad= bearing.getAngularRotationStrain(alpha_ad= alpha_ad, alpha_bd= alpha_bd)
ratio7= abs(eps_alphad-0.24803719008264463)/0.24803719008264463
# Compute total strain.
eps_td= bearing.getTotalStrain(vxd= vxd, vyd= vyd, Fzd= Fzd, alpha_ad= alpha_ad, alpha_bd= alpha_bd)
ratio8= abs(eps_td-5.433696054797232)/5.433696054797232

# Compute minimum thickness of steel laminate.
thk= bearing.getStrictReinforcedPlateThickness(vxd= vxd, vyd= vyd, Fzd= Fzd, withHoles= False)
minThk= bearing.getRequiredReinforcedPlateThickness(vxd= vxd, vyd= vyd, Fzd= Fzd, withHoles= False)
ratio9= abs(thk-1.8883532788898882e-3)/1.8883532788898882e-3


print('Effective area: Ae= '+'{:.3f}'.format(effectiveArea)+' m2')
print(ratio1)
print('Reduced effective area: Ar= '+'{:.3f}'.format(reducedArea)+' m2')
print(ratio2)
print('Shape factor considering inner layer: S1= '+'{:.2f}'.format(S1))
print(ratio3)
print('Shape factor considering outer layer: S2= '+'{:.2f}'.format(S2))
print(ratio4)
print('Compressive strain (cl. no. 5.3.3.1 & 5.3.3.2): eps_cd= '+'{:.3f}'.format(eps_cd))
print(ratio5)
print('Shear strain (cl. no. 5.3.3.3): eps_qd= '+'{:.3f}'.format(eps_qd))
print(ratio6)
print('Strain due to angular rotation(cl. no. 5.3.3.4): eps_alphad= '+'{:.3f}'.format(eps_alphad))
print(ratio7)
print('Total design strain (cl. no. 5.3.3a): eps_td= '+'{:.2f}'.format(eps_td))
print(ratio8)
print('Strict reinforcing plate thickness (cl. no. 5.3.3.5): t_s= '+'{:.2f}'.format(thk*1e3))
print(ratio9)
print('Minimum reinforcing plate thickness (cl. no. 5.3.3.5): t_s= '+'{:.2f}'.format(minThk*1e3))
