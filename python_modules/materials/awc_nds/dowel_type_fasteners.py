# -*- coding: utf-8 -*-
''' Dowel type fasteners design according to chapter 12
    of "NATIONAL DESIGN SPECIFICATION FOR WOOD CONSTRUCTION 2018"
    of the American Wood Council.'''

from __future__ import print_function
from __future__ import division

import math
from materials.awc_nds import AWCNDS_materials as mat
from misc_utils import log_messages as lmsg
from misc_utils import units_utils

class DowelFastener(object):
    ''' Dowel-Type fastener as defined in chapter 12 of NDS-2018.

    :ivar diameter: fastener diameter.
    :ivar length: fastener length.
    :ivar tip: fastener tapered tip length.
    :ivar bendingYieldStrength: fastener bending yield strength (see
                                 table I1 NDS-2018).
    '''
    def __init__(self, diameter, length, tip, bendingYieldStrength):
        ''' Constructor.
 
        :param diameter: fastener diameter.
        :param length: fastener length.
        :param tip: fastener tapered tip length.
        :param bendingYieldStrength: fastener bending yield strength (see
                                     table I1 NDS-2018).
        '''
        self.D= diameter
        self.L= length
        self.tip= tip
        self.Fyb= bendingYieldStrength
        
    def getPenetration(self, sideMemberThickness):
        ''' Return the dowel penetration in the main member.

        :param sideMemberThickness: side member thickness.
        '''
        return self.L-sideMemberThickness-self.tip
        
    def getReductionTerm(self, theta, yieldMode):
        ''' Return the reduction term Rd according to table
            12.3.1B of NDS-2018.

        :param theta: maximum angle between the direction of load
                      and the direction of grain (0<=theta<=PI/2) for any
                      member in a connection.
        :param yieldMode: yield mode.
        '''
        if(self.D<0.25*units_utils.inchToMeter):
            if(self.D<0.17*units_utils.inchToMeter):
                return 2.2
            else:
                return 10.0*self.D/units_utils.inchToMeter+0.5
        else:
            k_theta= 1+0.25*(theta/(math.pi/2.0))
            if(yieldMode in ['Im','Is']):
                return 4.0*k_theta
            elif(yieldMode=='II'):
                return 3.6*k_theta
            elif(yieldMode in ['IIIm','IIIs', 'IV']):
                return 3.2*k_theta
            else:
                lmsg.error('Unknown yield mode: \''+yieldMode+'\'')
                
    def getDiameterForYield(self):
        ''' Return the diamterer to use in the k1, k2 and k3
            expressions accorcing to clause 12.3.7 of NDS-2018.'''
        return self.D

    def getK1(self,mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s, endGrain= False):
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
        :param endGrain: true for dowel-type fasteners with D > 1/4" that are 
                         inserted into the end grain of the main member, 
                         with the fastener axis parallel to the wood fibers.
                         See clause 12.3.3.4 of NDS.
        '''
        Fem= mainMemberWood.getDowelBearingStrength(self.D, theta_m, endGrain)
        Fes= sideMemberWood.getDowelBearingStrength(self.D, theta_s)
        Re= Fem/Fes
        Rt= lm/ls
        return (math.sqrt(Re+2.0*Re**2*(1+Rt+Rt**2)+Rt**2*Re**3)-Re*(1+Rt))/(1+Re)
    def getK2(self,mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s, endGrain= False):
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
        :param endGrain: true for dowel-type fasteners with D > 1/4" that are 
                         inserted into the end grain of the main member, 
                         with the fastener axis parallel to the wood fibers.
                         See clause 12.3.3.4 of NDS.
        '''
        Fem= mainMemberWood.getDowelBearingStrength(self.D, theta_m, endGrain)
        Fes= sideMemberWood.getDowelBearingStrength(self.D, theta_s)
        Re= Fem/Fes
        Rt= lm/ls
        D= self.getDiameterForYield()
        return -1+math.sqrt(2.0*(1+Re)+(2.0*self.Fyb*(2.0+Re)*D**2)/(3.0*Fem*lm**2))
    def getK3(self,mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s, endGrain= False):
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
        :param endGrain: true for dowel-type fasteners with D > 1/4" that are 
                         inserted into the end grain of the main member, 
                         with the fastener axis parallel to the wood fibers.
                         See clause 12.3.3.4 of NDS.
        '''
        Fem= mainMemberWood.getDowelBearingStrength(self.D, theta_m, endGrain)
        Fes= sideMemberWood.getDowelBearingStrength(self.D, theta_s)
        Re= Fem/Fes
        D= self.getDiameterForYield()
        return -1+math.sqrt(2.0*(1+Re)/Re+(2.0*self.Fyb*(2.0+Re)*D**2)/(3.0*Fem*ls**2))
    def getYieldLimit(self, mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s, doubleShear= False, endGrain= False):
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
        :param endGrain: true for dowel-type fasteners with D > 1/4" that are 
                         inserted into the end grain of the main member, 
                         with the fastener axis parallel to the wood fibers.
                         See clause 12.3.3.4 of NDS.
        '''
        if(lm<0.0):
            lmsg.error('negative main member dowel bearing length: lm= '+str(lm))
        if(ls<0.0):
            lmsg.error('negative side member dowel bearing length: ls= '+str(ls))
        D= self.getDiameterForYield()
        Fem= mainMemberWood.getDowelBearingStrength(D, theta_m, endGrain)
        Rd_Im= self.getReductionTerm(theta= 0.0, yieldMode= 'Im')
        Z_Im= D*lm*Fem/Rd_Im # Eq. (12.3-1 or 12.3-7)
        retval= Z_Im
        Rd_Is= self.getReductionTerm(theta= 0.0, yieldMode= 'Is')
        Fes= sideMemberWood.getDowelBearingStrength(D, theta_s)
        Z_Is= D*ls*Fes/Rd_Is # Eq. (12.3-2)
        if(doubleShear):
            Z_Is*=2.0 # Eq. (12.3-8)
        retval= min(retval,Z_Is)
        Rd_II= self.getReductionTerm(theta= 0.0, yieldMode= 'II')
        k1= self.getK1(mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s, endGrain)
        Z_II= k1*D*ls*Fes/Rd_II # Eq. (12.3-3)
        retval= min(retval,Z_II)
        Rd_IIIm= self.getReductionTerm(theta= 0.0, yieldMode= 'IIIm')
        k2= self.getK2(mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s, endGrain)
        Re= Fem/Fes
        Z_IIIm= k2*D*lm*Fem/(1+2.0*Re)/Rd_IIIm # Eq. (12.3-4)
        retval= min(retval,Z_IIIm)
        Rd_IIIs= self.getReductionTerm(theta= 0.0, yieldMode= 'IIIs')
        k3= self.getK3(mainMemberWood, sideMemberWood, lm, ls, theta_m, theta_s, endGrain)
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

    
class Nail(DowelFastener):
    ''' Nail as defined in NDS-2018.

    '''
    def __init__(self, diameter, length, hardenedSteel= False):
        ''' Constructor.
 
        :param diameter: fastener diameter.
        :param length: fastener length.
        :param hardenedSteel: if the fastener is made of hardened steel
                              according to ASTM F 1667 set this variable
                              to True.
        '''
        Fyb= 100e3*mat.psi2Pa
        if(not hardenedSteel):
            if(diameter<2.5146e-3):
                lmsg.error('Nail diameter too small.')
            elif(diameter<=3.6068e-3):
                Fyb*=1.0
            elif(diameter<=4.4958e-3):
                Fyb*=0.9
            elif(diameter<=5.9944e-3):
                Fyb*=0.8
            elif(diameter<=6.9342e-3):
                Fyb*=0.7
            elif(diameter<=8.7376e-3):
                Fyb*= 0.6
            elif(diameter<=9.525e-3):
                Fyb*= 0.45
            else:
                lmsg.error('Nail diameter too big.')
                Fyb*= 0.01
        else: # Hardened steel
            if(diameter<3.048e-3):
                lmsg.error('Nail diameter too small.')
            elif(diameter<=3.6068e-3):
                Fyb*=1.3
            elif(diameter<=4.8768e-3):
                Fyb*=1.15
            elif(diameter<=5.2578e-3):
                Fyb*=1.0
            else:
                lmsg.error('Nail diameter too big.')
                Fyb*= 0.01
        super(Nail,self).__init__(diameter, length, tip= 2.0*diameter,  bendingYieldStrength= Fyb)
        
    def getReferenceWithdrawal(self, G):
        ''' Return reference withdrawal design value according
            to equation 12.2-3 of NDS-2018.
 
        :param G: specific gravity of wood.
        '''
        return 1380.0*mat.pound2N/units_utils.inchToMeter*math.pow(G,5/2.0)*self.D/units_utils.inchToMeter

    def getDesignWithdrawal(self,G, sideMemberThickness):
        ''' Return the withdrawal design value based on main
            member penetration.

        :param G: specific gravity of wood.
        :param sideMemberThickness: side member thickness.
        '''
        W= self.getReferenceWithdrawal(G)
        pt= self.L - sideMemberThickness
        return self.getReferenceWithdrawal(G)*pt
    
    def getMinPenetration(self):
        ''' Return the minimum length of nail pentration, p_min,
            including the length of the tapered tip where part of the 
            penetration into the main member for single shear connections
            and the side members for double shear connection according
            to clause 12.1.6.4 of NDS-2018.'''
        return 6.0*self.D

#Nail_2D= Nail(diameter= 0.072*units_utils.inchToMeter, length= 1*units_utils.inchToMeter)
#Nail_2D= Nail(diameter= 0.083*units_utils.inchToMeter, length= 1*units_utils.inchToMeter)
#Nail_3D= Nail(diameter= 0.083*units_utils.inchToMeter, length= 1.25*units_utils.inchToMeter)
Nail_4D= Nail(diameter= 0.109*units_utils.inchToMeter, length= 1.5*units_utils.inchToMeter)
Nail_5D= Nail(diameter= 0.109*units_utils.inchToMeter, length= 1.75*units_utils.inchToMeter)
Nail_6D= Nail(diameter= 0.12*units_utils.inchToMeter, length= 2*units_utils.inchToMeter)
Nail_8D= Nail(diameter= 0.134*units_utils.inchToMeter, length= 2.5*units_utils.inchToMeter)
Nail_10D= Nail(diameter= 0.148*units_utils.inchToMeter, length= 3*units_utils.inchToMeter)
Nail_12D= Nail(diameter= 0.148*units_utils.inchToMeter, length= 3.25*units_utils.inchToMeter)
Nail_16D= Nail(diameter= 0.165*units_utils.inchToMeter, length= 3.5*units_utils.inchToMeter)
Nail_20D= Nail(diameter= 0.203*units_utils.inchToMeter, length= 4*units_utils.inchToMeter)
Nail_30D= Nail(diameter= 0.22*units_utils.inchToMeter, length= 4.5*units_utils.inchToMeter)
Nail_40D= Nail(diameter= 0.238*units_utils.inchToMeter, length= 5*units_utils.inchToMeter)
Nail_60D= Nail(diameter= 0.238*units_utils.inchToMeter, length= 6*units_utils.inchToMeter)
    
class Screw(DowelFastener):
    ''' Screw as defined in NDS-2018.
 
    :ivar rootDiameter: root diameter.
    '''
    def __init__(self, diameter, length, tip, rootDiameter, bendingYieldStrength= 45e3*mat.psi2Pa):
        ''' Constructor.
 
        :param diameter: fastener diameter.
        :param length: fastener length.
        :param tip: fastener tapered tip length.
        :param rootDiameter: root diameter.
        :param bendingYieldStrength: fastener bending yield strength (see
                                     table I1 NDS-2018).
        '''
        super(Screw,self).__init__(diameter, length, tip, bendingYieldStrength)
        self.rootDiameter= rootDiameter

class LagScrew(Screw):
    ''' Lag screw as defined in NDS-2018.'''
    def __init__(self, diameter, length, tip, rootDiameter, bendingYieldStrength= 45e3*mat.psi2Pa):
        ''' Constructor. 
 
        :param diameter: fastener diameter.
        :param length: fastener length.
        :param tip: fastener tapered tip length.
        :param rootDiameter: root diameter.
        :param bendingYieldStrength: fastener bending yield strength (see
                                     table I1 NDS-2018).
        '''
        super(LagScrew,self).__init__(diameter, length, tip, rootDiameter, bendingYieldStrength)
    def getReferenceWithdrawal(self, G):
        ''' Return reference withdrawal design value according
            to equation 12.2-1 of NDS-2018.
 
        :param G: specific gravity of wood.
        '''
        return 1800.0*mat.pound2N/units_utils.inchToMeter*math.pow(G,1.5)*pow((self.D/units_utils.inchToMeter),3.0/4.0)
    
    def getDesignWithdrawal(self,G, sideMemberThickness, endGrainFactor= 0.75):
        ''' Return the withdrawal design value based on main
            member penetration.

        :param G: specific gravity of wood.
        :param sideMemberThickness: side member thickness.
        :param endGrainFactor: applicable where lag screws are loaded 
                              in withdrawal from end grain (see section
                              12.2 of NDS-2018.'''
        W= self.getReferenceWithdrawal(G)
        pt= self.getPenetration(sideMemberThickness)
        return self.getReferenceWithdrawal(G)*endGrainFactor*pt
    
    def getMinPenetration(self):
        ''' Return the minimum length of lag screw pentration, p_min,
            including the length of the tapered tip where part of the 
            penetration into the main member for single shear connections
            and the side members for double shear connection according
            to clause 12.1.4.6 of NDS-2018.'''
        return 4.0*self.D
        
class WoodScrew(Screw):
    ''' Wood screw as defined in NDS-2018.

    :ivar headDiameter: head diameter.
    :ivar threadLength: thread length.
    '''
    def __init__(self, diameter, length, headDiameter, rootDiameter, threadLength= None, bendingYieldStrength= 45e3*mat.psi2Pa):
        ''' Constructor.

        :param tip: fastener tapered tip length.
        :param headDiameter: head diameter.
        :param rootDiameter: root diameter.
        :param threadLength: thread length.
        :param bendingYieldStrength: fastener bending yield strength (see
                                     table I1 NDS-2018).
        '''
        super(WoodScrew,self).__init__(diameter= diameter, length= length, tip= 2*diameter, rootDiameter= rootDiameter, bendingYieldStrength= bendingYieldStrength)
        self.headDiameter= headDiameter
        if(threadLength):
            self.threadLength= threadLength
        else:
            self.threadLength= None
    def getMinPenetration(self):
        ''' Return the minimum length of wood screw pentration, p_min,
            including the length of the tapered tip where part of the 
            penetration into the main member for single shear connections
            and the side members for double shear connection according
            to clause 12.1.5.6 of NDS-2018.'''
        return 6.0*self.D
    
    def getDiameterForYield(self):
        ''' Return the diameter to use in the k1, k2 and k3
            expressions accorcing to clause 12.3.7 of NDS-2018.'''
        return self.rootDiameter


