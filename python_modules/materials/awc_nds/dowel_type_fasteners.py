# -*- coding: utf-8 -*-
''' Dowel type fasteners design according to chapter 12
    of "NATIONAL DESIGN SPECIFICATION FOR WOOD CONSTRUCTION 2018"
    of the American Wood Council.'''

from __future__ import print_function
from __future__ import division

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
    def getReductionTerm(self, theta, yieldMode):
        ''' Return the reduction term Rd according to table
            12.3.1B of NDS-2018.

        :param theta: maximum angle between the direction of load
                      and the direction of grain (0<=theta<=PI/2) for any
                      member in a connection.
        :param yieldMode: yield mode.
        '''
        if(self.D<0.25*mat.in2meter):
            if(self.D<0.17*mat.in2meter):
                return 2.2
            else:
                return 10.0*self.D/mat.in2meter+0.5
        else:
            k_theta= 1+0.25*(theta/(math.pi/2.0))
            if(yieldMode in ['Im','Is']):
                return 4.0*k_theta
            elif(yieldMode=='II'):
                return 3.6*k_theta
            elif(yieldMode in ['IIIm','IIIs, IV']):
                return 3.2*k_theta
            else:
                lmsg.error('Unknown yield mode: '+yieldMode)
    def getDiameterForYield(self):
        ''' Return the diamterer to use in the k1, k2 and k3
            expressions accorcing to clause 12.3.7 of NDS-2018.'''
        return self.D
    def getK1(self,mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s):
        ''' Return the k1 factor according to table
            12.3.1B of NDS-2018.

        :param mainMemberWood: main member Wood object.
        :param mainMemberWood: side member Wood object.
        :param lm: main member dowel bearing length.
        :param ls: side member dowel bearing length.
        :param theta_m: angle between the direction of load and the
                        direction of grain of the main member.
        :param theta_s: angle between the direction of load and the
                        direction of grain of the side member.
        '''
        Fem= mainMemberWood.getDowelBearingStrenght(self.D, theta_m)
        Fes= sideMemberWood.getDowelBearingStrenght(self.D, theta_s)
        Re= Fem/Fes
        Rt= lm/ls
        return (math.sqrt(Re+2.0*Re**2*(1+Rt+Rt**2)+Rt**2*Re**3)-Re*(1+Rt))/(1+Re)
    def getK2(self,mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s):
        ''' Return the k2 factor according to table
            12.3.1B of NDS-2018.

        :param mainMemberWood: main member Wood object.
        :param mainMemberWood: side member Wood object.
        :param lm: main member dowel bearing length.
        :param ls: side member dowel bearing length.
        :param theta_m: angle between the direction of load and the
                        direction of grain of the main member.
        :param theta_s: angle between the direction of load and the
                        direction of grain of the side member.
        '''
        Fem= mainMemberWood.getDowelBearingStrenght(self.D, theta_m)
        Fes= sideMemberWood.getDowelBearingStrenght(self.D, theta_s)
        Re= Fem/Fes
        Rt= lm/ls
        D= self.getDiameterForYield()
        return -1+math.sqrt(2.0*(1+Re)+(2.0*self.Fyb*(2.0+Re)*D**2)/(3.0*Fem*lm**2))
    def getK3(self,mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s):
        ''' Return the k3 factor according to table
            12.3.1B of NDS-2018.

        :param mainMemberWood: main member Wood object.
        :param mainMemberWood: side member Wood object.
        :param lm: main member dowel bearing length.
        :param ls: side member dowel bearing length.
        :param theta_m: angle between the direction of load and the
                        direction of grain of the main member.
        :param theta_s: angle between the direction of load and the
                        direction of grain of the side member.
        '''
        Fem= mainMemberWood.getDowelBearingStrenght(self.D, theta_m)
        Fes= sideMemberWood.getDowelBearingStrenght(self.D, theta_s)
        Re= Fem/Fes
        D= self.getDiameterForYield()
        return -1+math.sqrt(2.0*(1+Re)/Re+(2.0*self.Fyb*(2.0+Re)*D**2)/(3.0*Fem*ls**2))
    def getYieldLimit(self, mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s, doubleShear= False):
        ''' Return the yield limit according to table
            12.3.1B of NDS-2018.

        :param mainMemberWood: main member Wood object.
        :param mainMemberWood: side member Wood object.
        :param lm: main member dowel bearing length.
        :param ls: side member dowel bearing length.
        :param theta_m: angle between the direction of load and the
                        direction of grain of the main member.
        :param theta_s: angle between the direction of load and the
                        direction of grain of the side member.
        :param doubleShear: double shear plane in connection.
        '''
        D= self.getDiameterForYield()
        Fem= mainMemberWood.getDowelBearingStrenght(D, theta_m)
        Rd_Im= self.getReductionTerm(theta= 0.0, yieldMode= 'Im')
        Z_Im= D*lm*Fem/Rd_Im # Eq. (12.3-1 or 12.3-7)
        retval= Z_Im
        Rd_Is= self.getReductionTerm(theta= 0.0, yieldMode= 'Is')
        Fes= sideMemberWood.getDowelBearingStrenght(D, theta_s)
        Z_Is= D*ls*Fes/Rd_Is # Eq. (12.3-2)
        if(doubleShear):
            Z_Is*=2.0 # Eq. (12.3-8)
        retval= min(retval,Z_Is)
        Rd_II= self.getReductionTerm(theta= 0.0, yieldMode= 'II')
        k1= self.getK1(mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s)
        Z_II= k1*D*ls*Fes/Rd_II # Eq. (12.3-3)
        retval= min(retval,Z_II)
        Rd_IIIm= self.getReductionTerm(theta= 0.0, yieldMode= 'IIIm')
        k2= self.getK2(mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s)
        Re= Fem/Fes
        Z_IIIm= k2*D*lm*Fem/(1+2.0*Re)/Rd_IIIm # Eq. (12.3-4)
        retval= min(retval,Z_IIIm)
        Rd_IIIs= self.getReductionTerm(theta= 0.0, yieldMode= 'IIIs')
        k3= self.getK3(mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s)
        Z_IIIs= k3*D*ls*Fem/(2+Re)/Rd_IIIs # Eq. (12.3-5)
        if(doubleShear):
            Z_IIIs*=2.0 # Eq. (12.3-9)
        retval= min(retval,Z_IIIs)
        Rd_IV= self.getReductionTerm(theta= 0.0, yieldMode= 'IV')
        Z_IV= D**2/Rd_IV*math.sqrt(2.0*Fem*self.Fyb/(3.0*(1+Re)))# Eq. (12.3-6)
        if(doubleShear):
            Z_IV*=2.0 # Eq. (12.3-10)
        retval= min(retval,Z_IV)
        return retval


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
    def getDiameterForYield(self):
        ''' Return the diamterer to use in the k1, k2 and k3
            expressions accorcing to clause 12.3.7 of NDS-2018.'''
        return self.rootDiameter


