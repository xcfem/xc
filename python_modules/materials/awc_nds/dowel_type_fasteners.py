# -*- coding: utf-8 -*-
''' Dowel type fasteners design according to chapter 12
    of "NATIONAL DESIGN SPECIFICATION FOR WOOD CONSTRUCTION 2018"
    of the American Wood Council.'''

import math
from materials.awc_nds import AWCNDS_materials as mat

class Screw(object):
    ''' Screw as defined in NDS-2018.'''
    def __init__(self, diameter, length):
        ''' Constructor.
 
        :param diameter: fastener diameter.
        :param length: fastener length.
        '''
        self.D= diameter
        self.L= length

class LagScrew(Screw):
    ''' Lag screw as defined in NDS-2018.'''
    def __init__(self, diameter, length, tip):
        ''' Constructor.

        :param tip: fastener tapered tip length.
        '''
        super(LagScrew,self).__init__(diameter, length)
        self.tip= tip
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
        
