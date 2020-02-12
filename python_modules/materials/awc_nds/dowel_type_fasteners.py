# -*- coding: utf-8 -*-
''' Dowel type fasteners design according to chapter 12
    of "NATIONAL DESIGN SPECIFICATION FOR WOOD CONSTRUCTION 2018"
    of the American Wood Council.'''

import math
from materials.awc_nds import AWCNDS_materials as mat

class Screw(object):
    ''' Screw as defined in NDS-2018.'''
    def __init__(self, diameter, length, tip, bendingYieldStrength= 45e3*mat.psi2Pa):
        ''' Constructor.
 
        :param diameter: fastener diameter.
        :param length: fastener length.
        :param bendingYieldStrength: fastener bending yield strength (see
                                     table I1 NDS-2018).
        '''
        self.D= diameter
        self.L= length
        self.tip= tip
        self.Fyb= bendingYieldStrength

class LagScrew(Screw):
    ''' Lag screw as defined in NDS-2018.'''
    def __init__(self, diameter, length, tip, bendingYieldStrength= 45e3*mat.psi2Pa):
        ''' Constructor.

        :param tip: fastener tapered tip length.
        '''
        super(LagScrew,self).__init__(diameter, length, tip, bendingYieldStrength)
    def getReferenceWithdrawal(self, G):
        ''' Return reference withdrawal design value according
            to equation 12.2-1 of NDS-2018.
 
        :param G: specific gravity of wood.
        '''
        return 1800.0*mat.pound2N/mat.in2meter*math.pow(G,1.5)*pow((self.D/mat.in2meter),3.0/4.0)
    def getScrewPenetration(self, sideMemberThickness):
        ''' Return the screw penetration in the main member.

        :param sideMemberThickness: side member thickness.
        '''
        return self.L-sideMemberThickness-self.tip
    def getDesignWithdrawal(self,G, sideMemberThickness, endGrainFactor= 0.75):
        ''' Return the withdrawal design value based on main
            member penetration.

        :param G: specific gravity of wood.
        :param sideMemberThickness: side member thickness.
        :param endGrainFactor: applicable where lag screws are loaded 
                              in withdrawal from end grain (see section
                              12.2 of NDS-2018.'''
        W= self.getReferenceWithdrawal(G)
        pt= self.getScrewPenetration(sideMemberThickness)
        return self.getReferenceWithdrawal(G)*endGrainFactor*self.getScrewPenetration(sideMemberThickness)
        
class WoodScrew(Screw):
    ''' Wood screw as defined in NDS-2018.'''
    def __init__(self, diameter, length, headDiameter, rootDiameter, threadLength= None, bendingYieldStrength= 45e3*mat.psi2Pa):
        ''' Constructor.

        :param tip: fastener tapered tip length.
        :param headDiameter: head diameter.
        :param rootDiameter: root diameter.
        :param threadLength: thread length.
        '''
        super(WoodScrew,self).__init__(diameter, length, 2*diameter, bendingYieldStrength)
        self.headDiameter= headDiameter
        self.rootDiameter= rootDiameter
        if(threadLength):
            self.threadLength= threadLength
        else:
            self.threadLength= None
    def getMinPenetration(self):
        ''' Return the inimum length of wood screw pentration, p_min,
            including the length of the tapered tip where part of the 
            penetration into the main member for single shear connections
            and the side members for double shear connection according
            to clause 12.1.5.6 of NDS-2018.'''
        return 6.0*self.D


