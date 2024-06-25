# -*- coding: utf-8 -*-
''' Test of reinforced rectangular elastomeric bearing design accordint
    to EN 1337-3:2005.

    Data obtained from an analysis from Mageba.
'''

from __future__ import division
from __future__ import print_function


__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

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
    '''
    def __init__(self, a, b, tb, ti, ts, tso, Te, Tb, n, C, ted):
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
        print('factor= ', factor)
        print('ti= ', self.ted)
        print('retval= ', factor/1.4/self.ted)
        return factor/(1.4*self.ted)
        
    
# Maximum displacements.
vxd= 141.8e-3
vyd= 33.5e-3

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

print('Effective area: Ae= '+'{:.3f}'.format(effectiveArea)+' m2')
print(ratio1)
print('Reduced effective area: Ar= '+'{:.3f}'.format(reducedArea)+' m2')
print(ratio2)
print('Shape factor considering inner layer: S1= '+'{:.2f}'.format(S1))
print(ratio3)
print('Shape factor considering outer layer: S2= '+'{:.2f}'.format(S2))
print(ratio4)
