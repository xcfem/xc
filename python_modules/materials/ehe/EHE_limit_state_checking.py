# -*- coding: utf-8 -*-
''' Limit state checking according to structural concrete spanish standard EHE-08.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import functools
import sys
import json
import geom
import xc
from materials.ehe import EHE_materials
from materials.sections.fiber_section import fiber_sets
from materials import limit_state_checking_base as lscb
from postprocess import limit_state_data as lsd
from postprocess import control_vars as cv
from postprocess.config import file_names as fn
from misc_utils import log_messages as lmsg
import scipy.interpolate
from scipy import optimize
from materials import concrete_base
# from scipy import interpolate
from materials.sections import rebar_family as rf
from postprocess.reports import common_formats as fmt
from collections import defaultdict
from materials.sections import internal_forces
from model.mesh import element_vectors

class RebarController(lscb.EURebarController):
    '''Control of some parameters as development length 
       minimum reinforcement and so on.

       :ivar pos: reinforcement position according to clause 69.5.1.1
                  of EHE-08 (I: good adherence, II: poor adherence).
    '''    
    def __init__(self, concreteCover= 35e-3, spacing= 150e-3, pos= 'II', compression= True, alpha_1= 1.0, alpha_3= 1.0, alpha_4= 1.0, alpha_5= 1.0):
        '''Constructor.

        :param concreteCover: the distance from center of a bar or wire to 
                             nearest concrete surface.
        :param spacing: center-to-center spacing of bars or wires being 
                       developed, in.
        :param pos: reinforcement position according to clause 69.5.1.1
                    of EHE-08 (I: good adherence, II: poor adherence).
        :param compression: true if reinforcement is compressed.
        :param alpha_1: effect of the form of the bars assuming adequate cover.
        :param alpha_3: effect of confinement by transverse reinforcement.
        :param alpha_4: influence of one or more welded transverse bars along 
                        the design anchorage length.
        :param alpha_5: effect of the pressure transverse to the plane of 
                        splitting along the design anchorage length.
        '''
        super(RebarController,self).__init__(concreteCover= concreteCover, spacing= spacing, compression= compression, alpha_1= alpha_1, alpha_3= alpha_3, alpha_4= alpha_4, alpha_5= alpha_5)
        self.pos= pos

    def getM(self, concrete, steel):
        ''' Return the "m" coefficient according to table 69.5.1.2.a of
            EHE-08

        :param concrete: concrete material.
        :param steel: reinforcing steel.
        '''
        return concrete.getM(steel)

    def getBasicAnchorageLength(self, concrete, rebarDiameter, steel, dynamicEffects= False):
        '''Returns basic anchorage length in tension according to clause
           69.5.1.2 of EHE.

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of bar, wire, or prestressing strand.
        :param steel: reinforcement steel.
        :param dynamicEffects: true if the anchorage is subjected to
                               dynamic effects.
        '''
        return steel.getBasicAnchorageLength(concrete, rebarDiameter, self.pos, dynamicEffects)
    
    def getConcreteMinimumCoverEffect(self, rebarDiameter, barShape= 'bent', lateralConcreteCover= None):
        ''' Return the value of the alpha_2 factor that introduces the effect
            of concrete minimum cover according to figure 69.5.1.2.a and 
            table 69.5.1.2.c of EHE-08.

        :param rebarDiameter: nominal diameter of the bar.
        :param barShape: 'straight' or 'bent' or 'looped'.
        :param lateralConcreteCover: lateral concrete cover (c1 in figure 8.3
                                     of EC2:2004). If None make it equal to
                                     the regular concrete cover.
        '''
        return super(RebarController, self).getConcreteMinimumCoverEffect(rebarDiameter= rebarDiameter, barShape= barShape, lateralConcreteCover= lateralConcreteCover)

    def getBeta(self, rebarDiameter, barShape= 'bent', lateralConcreteCover= None):
        ''' Return the value of beta according to the commentary to clause
            69.5.1.2 of EHE-08 and table 69.5.1.2.c.

        :param rebarDiameter: nominal diameter of the bar.
        :param barShape: 'straight' or 'bent' or 'looped'.
        :param lateralConcreteCover: lateral concrete cover (c1 in figure 8.3
                                     of EC2:2004). If None make it equal to
                                     the regular concrete cover.
        '''
        alpha_2= self.getConcreteMinimumCoverEffect(rebarDiameter= rebarDiameter, barShape= barShape, lateralConcreteCover= lateralConcreteCover)
        return self.alpha_1*alpha_2*self.alpha_3*self.alpha_4*self.alpha_5

    def getNetAnchorageLength(self ,concrete, rebarDiameter, steel, steelEfficiency= 1.0, barShape= 'bent', lateralConcreteCover= None, dynamicEffects= False):
        '''Returns net anchorage length in tension according to clause
           69.5.1.2 of EHE.

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of bar, wire, or prestressing strand.
        :param steel: reinforcement steel.
        :param steelEfficiency: working stress of the reinforcement that it is 
                           intended to anchor, on the most unfavourable 
                           load hypothesis, in the section from which 
                           the anchorage length will be determined divided
                           by the steel design yield strength.
        :param barShape: 'straight' or 'bent' or 'looped'.
        :param dynamicEffects: true if the anchorage is subjected to
                               dynamic effects.
        '''
        beta= self.getBeta(rebarDiameter= rebarDiameter, barShape= barShape, lateralConcreteCover= lateralConcreteCover)
        tensionedBars= not self.compression
        return steel.getNetAnchorageLength(concrete, rebarDiameter, self.pos, beta= beta, efficiency= steelEfficiency, tensionedBars= tensionedBars, dynamicEffects= dynamicEffects)

    def getDesignAnchorageLength(self ,concrete, rebarDiameter, steel, steelEfficiency= 1.0, barShape= 'bent', lateralConcreteCover= None, dynamicEffects= False):
        '''Added for EC2 compatibility: returns net anchorage length in tension according to clause
           69.5.1.2 of EHE.

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of bar, wire, or prestressing strand.
        :param steel: reinforcement steel.
        :param steelEfficiency: working stress of the reinforcement that it is 
                           intended to anchor, on the most unfavourable 
                           load hypothesis, in the section from which 
                           the anchorage length will be determined divided
                           by the steel design yield strength.
        :param barShape: 'straight' or 'bent' or 'looped'.
        :param dynamicEffects: true if the anchorage is subjected to
                               dynamic effects.
        '''
        return  self.getNetAnchorageLength(concrete, rebarDiameter= rebarDiameter, steel= steel, steelEfficiency= steelEfficiency, barShape= barShape, lateralConcreteCover= lateralConcreteCover, dynamicEffects= dynamicEffects)

    def getLapLength(self ,concrete, rebarDiameter, steel, distBetweenNearestSplices, steelEfficiency= 1.0, ratioOfOverlapedTensionBars= 1.0, lateralConcreteCover= None, dynamicEffects= False):
        '''Returns net lap length in tension according to clause
           69.5.2 of EHE08.

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of bar, wire, or prestressing strand.
        :param steel: reinforcement steel.
        :param distBetweenNearestSplices: distance between the nearest splices
                                          according to figure 69.5.2.2.a.
        :param beta: reduction factor defined in Table 69.5.1.2.b.
        :param steelEfficiency: working stress of the reinforcement that it is 
                           intended to anchor, on the most unfavourable 
                           load hypothesis, in the section from which 
                           the anchorage length will be determined divided
                           by the steel design yield strength.
        :param ratioOfOverlapedTensionBars: ratio of overlapped tension bars 
                                            in relation to the total steel
                                            section.
        :param dynamicEffects: true if the anchorage is subjected to
                               dynamic effects.
        '''
        beta= self.getBeta(rebarDiameter= rebarDiameter, barShape= 'straight', lateralConcreteCover= lateralConcreteCover)
        tensionedBars= not self.compression      
        return steel.getLapLength(concrete, rebarDiameter, self.pos, distBetweenNearestSplices, beta= beta, efficiency= steelEfficiency, ratioOfOverlapedTensionBars= ratioOfOverlapedTensionBars, tensionedBars= tensionedBars, dynamicEffects= dynamicEffects)
    

class StrandController(RebarController):
    '''Control of some parameters as the length of transmission.

       :ivar pos: reinforcement position according to clause 69.5.1
                  of EHE-08 (I: good adherence, II: poor adherence).
       :ivar reinfType: prestressed reinforcement type: 'wire' or 'strand'
    '''
    
    def __init__(self, reinfType= 'strand', pos= 'II'):
        '''Constructor.

        :param pos: reinforcement position according to clause 69.5.1
                   of EHE-08 (I: good adherence, II: poor adherence).
        '''
        super(StrandController,self).__init__(pos)
        self.reinfType= reinfType
    
    def getDesignAdherenceStress(self, concrete, steel, t= 28):
        ''' Return the design value of the adherence stress according
            to the commentaries to the article 70.2.3 of EHE.

        :param concrete: concrete material.
        :param steel: prestressing steel material.
        :param t: concrete age at themoment of the prestress transmission
                  expressed in days.
        '''
        return steel.getDesignAdherenceStress(concrete,self.pos,t)

    def getTransmissionLength(self, rebarDiameter, concrete, steel, sg_pi, suddenRelease= True, ELU= True, t= 28):
        ''' Return the length of transmission for the strand according
            to the commentaries to the article 70.2.3 of EHE.

        :param rebarDiameter: nominal diameter of the wire, or prestressing strand.
        :param concrete: concrete material.
        :param steel: prestressing steel material.
        :param sg_pi: steel stress just after release.
        :param suddenRelease: if true, prestressing is transfered to concrete
                              in a very short time.
        :param ELU: true if ultimate limit state checking.
        :param t: concrete age at themoment of the prestress transmission
                  expressed in days.
        '''
        return steel.getTransmissionLength(rebarDiameter, concrete, steel, sg_pi, suddenRelease, ELU, t)


    def lbpd(self, rebarDiameter, concrete, sg_pi, suddenRelease= True, ELU= True, t= 28):
        ''' Return the design anchorage length for the strand according
            to the commentaries to the article 70.2.3 of EHE.

        :param rebarDiameter: nominal diameter of the wire, or prestressing strand.
        :param concrete: concrete material.
        :param sg_pi: tendon stress just after release.
        :param sg_pd: tendon stress under design load.
        :param sg_pcs: tendon stress due to prestress after all losses.
        :param suddenRelease: if true, prestressing is transfered to concrete
                              in a very short time.
        :param ELU: true if ultimate limit state checking.
        :param t: concrete age at themoment of the prestress transmission
                  expressed in days.
        '''
        lbpt= self.lbtp(rebarDiameter, concrete, sg_pi, suddenRelease, ELU, t)
        return lbpt
    
# Reinforced concrete section shear checking.
def getFcvEH91(fcd):
    '''
    Return fcv (concrete virtual shear strength)
     according to EH-91.

    :param fcd: design compressive strength of concrete.
    '''
    fcdKpcm2= -fcd*9.81/1e6
    return 0.5*math.sqrt(fcdKpcm2)/9.81*1e6


def getVu1(fcd, Nd, Ac, b0, d, alpha, theta):
    '''
    Return value of Vu1 (shear strength at failure due to diagonal compression
    in the web) according to clause 44.2.3.1 of EHE.

    :param fcd: Design compressive strength of concrete.
    :param Nd: axial force design value (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth.
    :param alpha: angle between the shear rebars and the member 
                  axis (figure 44.2.3.1.a EHE).
    :param theta: angle between the concrete compressed struts and the member 
                  axis (figure 44.2.3.1.a EHE).
    '''
    f1cd= 0.6*fcd
    sgpcd= (Nd/Ac)
    K= min(5/3*(1+sgpcd/fcd),1)
    ctgTheta= min(max(math.cotg(theta),0.5),2.0)
    return K*f1cd*b0*d*(ctgTheta+math.cotg(alpha))/(1+ctgTheta^2)

def getFcv(fact, fck, Nd, Ac, b0, d, AsPas, fyd, AsAct, fpd):
    '''
    Return the value of fcv (concrete virtual shear strength)
    for members WITH or WITHOUT shear reinforcement, according to clauses 
    44.2.3.2.1 y  44.2.3.2.2 of EHE.

    :param fact: factor equal to 0.12 for members WITHOUT shear reinforcement 
                 (0.18/gammac)
     and 0.10 for members WITH shear reinforcement  (0.15/gammac).
    :param fck: concrete characteristic compressive strength.
    :param Nd: axial force design value (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param fyd: reinforcing steel design yield strength (clause 38.3 EHE).
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param fpd: design value of prestressing steel strength (clause 38.6 EHE). 
    '''
    sgpcd= Nd/Ac
    chi= 1+math.sqrt(200/(d*1000)) # d MUST BE EXPRESSED IN METERS.
    rol= min((AsPas+AsAct*fpd/fyd)/(b0*d),0.02)
    return fact*chi*(rol*fck/1e4)^(1/3)*1e6-0.15*sgpcd


def getVu2SIN(fck, Nd, Ac, b0, d, AsPas, fyd, AsAct, fpd):
    '''
    Return the value of Vu2 (shear strength at failure due to tensile force in the web)
     for members WITHOUT shear reinforcement, according to 
     clause 44.2.3.2.1 of EHE (1998).

    :param fck: concrete characteristic compressive strength.
    :param Nd: axial force design value (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param fyd: reinforcing steel design yield strength (clause 38.3 EHE).
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
                  at a distance greater than the effective depth of the section.
    :param fpd: design value of prestressing steel strength (clause 38.6 EHE).

    '''
    return getFcv(0.12,fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd)*b0*d

def getVcu(fck, Nd, Ac, b0, d, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd):
    '''
    Return the value of Vcu (contribution of the concrete to shear strength) 
    for members WITH shear reinforcement, according to clause 44.2.3.2.2 of 
    EHE (1998).

    :param fck: concrete characteristic compressive strength.
    :param Nd: axial force design value (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param theta: angle between the concrete compressed struts and the member 
             axis (figure 44.2.3.1.a EHE 1998).
    :param AsPas: area of tensioned longitudinal steel rebars anchored
                  at a distance greater than the effective depth of the section.
    :param fyd: reinforcing steel design yield strength (clause 38.3 EHE).
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
                  at a distance greater than the effective depth of the section.
    :param fpd: design value of prestressing steel strength (clause 38.6 EHE).
    :param sgxd: design value of normal stress at the centre of gravity of the
                 section parallel to the main axis of the member. Calculated 
                 assuming NON CRACKED concrete (clause 44.2.3.2).
    :param sgyd: design value of normal stress at the centre of gravity of the
                 section parallel to shear force Vd. Calculated 
                 assuming NON CRACKED concrete (clause 44.2.3.2).
    '''
    fcv= getFcv(0.10,fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd)
    fctm= fctMedEHE08(fck) 
    ctgThetaE= min(max(0.5,math.sqrt(fctm-sgxd/fctm-sgyd)),2.0)
    ctgTheta= min(max(math.cotg(theta),0.5),2.0)
    if(ctgTheta<ctgThetaE):
        beta= (2*ctgTheta-1)/(2*ctgThetaE-1)
    else:
        beta= (ctgTheta-2)/(ctgThetaE-2)
    return fcv*b0*d*beta

def getVsu(z, alpha, theta, AsTrsv, fyd):
    '''
    Return the value of Vsu (contribution of the web’s transverse reinforcement 
    to shear strength) for members WITH shear reinforcement, according to 
    clause 44.2.3.2.2 of EHE.

    :param z: Mechanic lever arm.
    :param alpha: angle of the shear reinforcement with the member axis
                  (see figure 44.2.3.1.a EHE).
    :param theta: angle between the concrete compressed struts and the member 
                  axis (figure 44.2.3.1.a EHE).
    :param AsTrsv: transverse reinforcement area.
    :param fyd: design yield strength of the transverse reinforcement.

    '''
    ctgTheta= min(max(math.cotg(theta),0.5),2.0)
    fyalphad= min(fyd,400e6) # comment to clause 40.2 EHE
    return z*math.sin(alpha)*(math.cotg(alpha)+ctgTheta)*AsTrsv*fyalphad


def getVu2(fck, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv):
    '''
    Return the value of Vu2 (ultimate shear force failure due to tensile force 
    in the web) for members WITH or WITHOUT shear reinforcement, according to 
    clause 44.2.3.2.2 of EHE (1998).

    :param fck: concrete characteristic compressive strength.
    :param Nd: axial force design value (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param z: mechanic lever arm.
    :param alpha: angle of the shear reinforcement with the member 
                  axis (see figure 44.2.3.1.a EHE).
    :param theta: angle between the concrete compressed struts and the 
                  member axis (figure 44.2.3.1.a EHE).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
                  at a distance greater than the effective depth of the section.
    :param fyd: reinforcing steel design yield strength (clause 38.3 of EHE).
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param fpd: design value of prestressing steel strength
                (clause 38.6 of EHE).
    :param sgxd: design value of normal stress at the centre of gravity of the
     section parallel to the main axis of the member. Calculated assuming NON 
     CRACKED concrete (clause 44.2.3.2).
    :param sgyd: design value of normal stress at the centre of gravity of the
     section parallel to shear force Vd. Calculated assuming NON CRACKED 
     concrete (clause 44.2.3.2).
    :param AsTrsv: transverse reinforcement area.
    :param fydTrsv: design yield strength of the transverse reinforcement.
    '''
    if(AsTrsv>0.0):
        vcu= getVcu(fck,Nd,Ac,b0,d,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd)
        vsu= getVsu(z,alpha,theta,AsTrsv,fydTrsv)
    else:
        vcu= getVu2SIN(fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd)
        vsu= 0.0
    return vcu+vsu

def getVu(fck, fcd, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv):
    '''
    Return the value of Vu= max(Vu1,Vu2) for members WITH or WITHOUT shear 
     reinforcement, according to clause 44.2.3.2.2 of EHE (1998).

    :param fck: concrete characteristic compressive strength.
    :param Nd: axial force design value (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param z: mechanic lever arm.
    :param alpha: angle of the shear reinforcement with the member 
                  axis (figure 44.2.3.1.a EHE).
    :param theta: angle between the concrete compressed struts and the 
                  member axis (figure 44.2.3.1.a EHE).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
                  at a distance greater than the effective depth of the section.
    :param fyd: reinforcing steel design yield strength (clause 38.3 EHE).
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param fpd: design value of prestressing steel strength (clause 38.6 EHE).
    :param sgxd: design value of normal stress at the centre of gravity of the
     section parallel to the main axis of the member. Calculated assuming NON 
     CRACKED concrete (clause 44.2.3.2).
    :param sgyd: design value of normal stress at the centre of gravity of the
     section parallel to shear force Vd. Calculated assuming NON CRACKED 
     concrete (clause 44.2.3.2).
    :param AsTrsv: transverse reinforcement area.
    :param fydTrsv: design yield strength of the transverse reinforcement.
    '''
    vu1= getVu1(fcd,Nd,Ac,b0,d,alpha,theta)
    vu2= getVu2(fck,Nd,Ac,b0,d,z,alpha,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd,AsTrsv,fydTrsv)
    return min(vu1,vu2)

def shearOK(fck, fcd, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv, Vrd):
    '''Check shear.'''
    vu= getVu(fck,fcd,Nd,Ac,b0,d,z,alpha,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd,AsTrsv,fydTrsv)
    if(Vrd<=vu):
        return True
    else:
        return False

def shearExploitationRatioe(fck, fcd, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv, Vrd):
    '''Shear exploitation ratio.'''
    vu= getVu(fck,fcd,Nd,Ac,b0,d,z,alpha,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd,AsTrsv,fydTrsv)
    return Vrd/vu

# class ShearDesignParameters(object):
#     '''Defines shear design parameters.'''
#     def __init__(self):
#       self.concreteArea= 0.0 # Concrete section total area.
#       self.widthMin= 0.0 # net width of the element according to clause 40.3.5.
#       self.effectiveDepth= 0.0 # effective depth (meters).
#       self.mechanicLeverArm= 0.0 # mechanic lever arm (meters).
#       self.tensionedRebarsArea= 0.0 # Area of tensioned longitudinal steel rebars anchored at a distance greater than the effective depth of the section.
#       self.tensionedStrandsArea= 0.0 # Area of tensioned longitudinal prestressed steel anchored at a distance greater than the effective depth of the section.
#       self.areaShReinfBranchsTrsv= 0.0 # transverse reinforcement area.
#       self.sigmaXD= 0.0 # design value of normal stress at the centre of gravity of the section parallel to the main axis of the member. Calculated assuming NON CRACKED concrete (clause 44.2.3.2).
#       self.sigmaYD= 0.0 # design value of normal stress at the centre of gravity of the section parallel to shear force Vd. Calculated assuming NON CRACKED concrete (clause 44.2.3.2).
#       self.angAlpha= math.pi/2 # angle of the shear reinforcement with the member axis (figure 44.2.3.1.a EHE).
#       self.angTheta= math.pi/6. # Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
#       self.ultimateShearStrength= 0.0

#     def computeUltimateShearStrength(self, concreteFibersSet, rebarFibersSet, tensionedRebarsFiberSet, fck, fcd, fyd, fpd, fydTrsv):
#         '''Compute section shear strength.'''
#         self.concreteArea= concreteFibersSet.getArea()
#         self.widthMin= concreteFibersSet.getAnchoMecanico() # Enhance (not valid with non-convex sections).
#         self.effectiveDepth= concreteFibersSet.getEffectiveDepth()
#         self.mechanicLeverArm= concreteFibersSet.getMechanicLeverArm()
#         self.tensionedRebarsArea= tensionedRebarsFiberSet.getArea
#         # self.tensionedStrandsArea= 

#         self.sigmaXD= N/area+Mz/Iz*self.centerOfMassY+My/Iy*self.centerOfMassZ
#         self.ultimateShearStrength= getVu(fck= fck, fcd= fcd, Nd= N, Ac= self.concreteArea, b0= self.widthMin, d= self.effectiveDepth, z= self.mechanicLeverArm, alpha= self.angAlpha, theta= self.angTheta, AsPas= self.tensionedRebarsArea, fyd= fyd, AsAct= self.tensionedStrandsArea, fpd= fpd, sgxd= self.sigmaXD, sgyd= self.sigmaYD, AsTrsv= self.areaShReinfBranchsTrsv, fydTrsv= fydTrsv)

#     def printParams(self, os= sys.stdout):
#         '''print shear checking values.'''
#         os.write("area of tensioned rebars; As= "+str(self.tensionedRebarsArea*1e4)+" cm2")
#         os.write("transverse reinforcement area; AsTrsv= "+str(self.areaShReinfBranchsTrsv*1e4)+" cm2")
#         os.write("design value of normal stress; sigmaXD= "+str(self.sigmaXD/1e6)+"MPa")
#         os.write("effective depth; d= "+str(self.effectiveDepth)+" m")
#         os.write("minimal width; b0= "+str(self.widthMin)+" m")
#         os.write("mechanic lever arm; z= "+str(self.mechanicLeverArm)+" m")
#         os.write("shear strength; Vu= "+str(self.ultimateShearStrength/1e3)+" kN")

def getF1cdEHE08(fck,fcd):
    '''getF1cdEHE08(fck,fcd). Returns the value of f1cd (design value of the concrete strut strength) according to clause 44.2.3.1 of EHE-08.

    :param fck: concrete characteristic compressive strength (Pa).
    :param fcd: design value of concrete compressive strength (N/m2).
    '''
    retval=0.6
    if(fck>60e6):
        retval=max(0.9-fck/200.e6,0.5)
    retval= retval*fcd
    return retval



def getKEHE08(sgpcd,fcd):
    '''getKEHE08(sgpcd,fcd). Return the value of K (coefficent that depends of the axial force) according to clause 44.2.3.1 de la EHE-08

    :param sgpcd: effective normal stress in concrete Ncd/Ac.
    :param fcd: design value of concrete compressive strength (N/m2).  
    '''
    s=-sgpcd/fcd #Positive when compressed
    if s>1:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+"; too much compression in concrete: ("+str(sgpcd)+"<"+str(-fcd)+")\n")
    if s<=0:
        retval=1.0
    elif s<=0.25:
        retval=1+s
    elif s<=0.5:
        retval=1.25
    else:
        retval=2.5*(1-s)
    return retval

def getVu1EHE08(fck, fcd, Ncd, Ac, b0, d, alpha, theta):
    '''getVu1EHE08(fck,fcd,Ncd,Ac,b0,d,alpha,theta) [units: N, m, rad]. Return
       the value of Vu1 (shear strength at failure due to diagonal compression in the web) 
       according to clause 44.2.3.1 of EHE-08.

    :param fck: concrete characteristic compressive strength.
    :param fcd: design value of concrete compressive strength (N/m2).
    :param Ncd: design value of axial force in concrete only (positive if in 
                tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth.
    :param alpha: angle of the shear reinforcement with the member axis
                  (figure 44.2.3.1 EHE-08).
    :param theta: angle between the concrete compressed struts and the 
                  member axis (figure 44.2.3.1.a EHE).
    :param As: Total area of compressed reinforcement. In combined compression, 
               it may be assumed that the entire reinforcement is subject 
               to the tension fyd.
    :param fyd: Design strength of reinforcement As.
    '''
    f1cd=getF1cdEHE08(fck,fcd)
    K= 1
    if(Ncd<0): # compressed section.
        sgpcd= Ncd/Ac
        K= getKEHE08(sgpcd,fcd)
    ctgTheta=min(max(1/math.tan(theta),0.5),2.0)
    return K*f1cd*b0*d*(ctgTheta+1/math.tan(alpha))/(1+ctgTheta**2)

def getVu2EHE08NoAtNoFis(fctd,I,S,b0,alphal,Ncd,Ac):
    '''getVu2EHE08NoAtNoFis(fctd,I,S,b0,alphal,Ncd,Ac) [units: N, m, rad].
       Return the value of Vu2 (shear strength at failure due to tensile 
       force in the web) according to clause 44.2.3.2.1.1 of EHE-08.

    :param fctd: design tensile strength of the concrete.
    :param I: Moment of inertia of the section with respect of its centroid.
    :param S: First moment of the section above the center of gravity.
    :param b0: net width of the element according to clause 40.3.5.
    :param alphal: coeficiente que, en su caso, introduce el efecto de 
                   la transferencia.
    :param Ncd: design value of axial force in concrete 
                (positive if in tension).
    :param Ac: concrete section total area.   
    '''
    tmp= fctd
    if alphal!=1.0:
      sgpcd=min(Ncd/Ac,0)
      tmp=math.sqrt(fctd**2-alphal*sgpcd*fctd)
    return I*b0/S*tmp

def getFcvEHE08(fact,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct):
    '''getFcvEHE08(fact,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct) [units: N, m, rad]
     Return the value of fcv (concrete virtual shear strength)
     for members WITH or WITHOUT shear reinforcement in cracked regions, according
     to clauses 44.2.3.2.1.2 and 44.2.3.2.2 of EHE-08.

    :param fact: Factor with a value of 0.18 for members WITHOUT shear 
                 reinforcement and 0.15 for members WITH shear reinforcement.
    :param fcv: effective concrete shear strength. For members without 
                shear reinforcement fcv= min(fck,60MPa). For members 
                 with shear reinforcement fcv= min(fck,100MPa).
     In both cases, if concrete quality control is not direct fcv= 15 MPa.
    :param gammaC: Partial safety factor for concrete.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param chi: coefficient that takes into account the aggregate effect
     inside the effective depth.
    :param sgpcd: average axial stress in the web (positive if in compression).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    '''
    rol=min((AsPas+AsAct)/(b0*d),0.02)
    return fact/gammaC*chi*(rol*fcv/1e4)**(1/3)*1e6-0.15*sgpcd


def getFcvMinEHE08(fcv,gammaC,d,chi,sgpcd):
    '''getFcvMinEHE08(fcv,gammaC,d,chi,sgpcd)
     Return the minimum value of fcv (concrete virtual shear strength)
     for members WITHOUT shear reinforcement in cracked regions, according to
     clause 44.2.3.2.1.2 of EHE-08.

    :param fcv: effective concrete shear strength. For members without 
                shear reinforcement fcv= min(fck,60MPa). For members 
                with shear reinforcement fcv= min(fck,100MPa).
     In both cases, if concrete quality control is not direct fcv= 15 MPa.
    :param gammaC: Partial safety factor for concrete.
    :param d: effective depth (meters).
    :param chi: coefficient that takes into account the aggregate effect
     inside the effective depth.
    :param sgpcd: average axial stress in the web (positive if in compression).
    '''
    return 0.075/gammaC*math.pow(chi,1.5)*math.sqrt(fcv)*1e3-0.15*sgpcd


def getVu2EHE08NoAtSiFis(fcv,fcd,gammaC,Ncd,Ac,b0,d,AsPas,AsAct):
    '''getVu2EHE08NoAtSiFis(fcv,fcd,gammaC,Ncd,Ac,b0,d,AsPas,AsAct) [units: N, m]
     Return the value of Vu2 (shear strength at failure due to tensile force in the web)
     for members WITHOUT shear reinforcement in cracked regions, according to 
     clause 44.2.3.2.1.2 of EHE-08.

    :param fcv: effective concrete shear strength. For members without 
                shear reinforcement fcv= min(fck,60MPa). For members with
                shear reinforcement fcv= min(fck,100MPa). In both cases, if 
                concrete quality control is not direct fcv= 15 MPa.
    :param fcd: design value of concrete compressive strength).
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete
                (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
                  at a distance greater than the effective depth of the section.
    '''
    chi=min(2,1+math.sqrt(200/(d*1000.))) #HA DE EXPRESARSE EN METROS.
    sgpcd=max(max(Ncd/Ac,-0.3*fcd),-12e6)
    fcvTmp=getFcvEHE08(0.18,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct)
    fcvMinTmp=getFcvMinEHE08(fcv,gammaC,d,chi,sgpcd)
    return max(fcvTmp,fcvMinTmp)*b0*d

def getVu2EHE08NoAt(M,Mfis,fcv,fck,gammaC,I,S,alphaL,Ncd,Ac,b0,d,AsPas,AsAct):
    '''getVu2EHE08NoAt(M,Mfis,fcv,fck,gammaC,I,S,alphaL,Ncd,Ac,b0,d,AsPas,AsAct)       [units: N, m, rad]. Return the value of Vu2
       (shear strength at failure due to tensile force in the web)
       for members WITHOUT shear reinforcement, according to clauses 
       44.2.3.2.1.1 y 44.2.3.2.1.2 of EHE-08.

    :param M: Bending moment in the section.
    :param Mfis: Cracking moment of the section calculated using 
                 fct,d= fct,k/gammaC in the same plane that M.
    :param fcv: effective concrete shear strength. For members without 
                shear reinforcement fcv= min(fck,60MPa). For members 
                with shear reinforcement fcv= min(fck,100MPa). In both cases,
                if concrete quality control is not direct fcv= 15 MPa.
    :param fck: concrete characteristic compressive strength.
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete
                (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    '''
    concrTmp= EHE_materials.EHEConcrete("HA",fck,gammaC)
    fctdTmp=concrTmp.fctkEHE08()/gammaC
    fcdTmp=fck/gammaC
    if M<=Mfis:
        retval=getVu2EHE08NoAtNoFis(fctdTmp,I,S,b0,alphaL,Ncd,Ac)
    else:
        retval=getVu2EHE08NoAtSiFis(fck,fcdTmp,1.5,Ncd,Ac,b0,d,AsPas,AsAct)
    return retval

def getVsuEHE08(z,alpha,theta,AsTrsv,fyd, circular):
    '''getVsuEHE08(z,alpha,theta,AsTrsv,fyd)  [units: N, m, rad]
    Return the value of Vsu (contribution of the web’s transverse 
    reinforcement to shear strength) for members WITH shear reinforcement,
    according to clause 44.2.3.2.2 of EHE-08.

    :param z: mechanic lever arm.
    :param alpha: angle of the transverse reinforcement with the member axis.
    :param theta: angle between the concrete compressed struts and the 
     member axis (figure 44.2.3.1.a EHE).
    :param AsTrsv: transverse reinforcement area which contribution will 
                   be computed.
    :param fyd: design yield strength of the transverse reinforcement.
    :param circular: if true we reduce the efectiveness of the shear 
                     reinforcement due to the transverse inclination of its
                     elements.
    '''
    fyalphad= min(fyd,400e6) # comment to clause 40.2 EHE
    retval= z*math.sin(alpha)*(1/math.tan(alpha)+1/math.tan(theta))*AsTrsv*fyalphad
    if(circular):
        retval*= 0.85
    return retval

def getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue):
    '''getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue) [units: N, m, rad]
    Return the longitudinal strain in the web according to
    expression in commments to the clause 44.2.3.2.2 of EHE-08.

    :param Nd: Design value of axial force (here positive if in tension)
    :param Md: Absolute value of design value of bending moment.
    :param Vd: Absolute value of effective design shear (clause 42.2.2).
    :param Td: design value of torsional moment.
    :param z: Lever arm.
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param Es: reinforcing steel elastic modulus.
    :param Ep: prestressing steel elastic modulus.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Area enclosed by the middle line of the effective hollow section.
    :param ue: Perimeter of the middle line of the effective hollow section.
    '''
    denomEpsilonX=2*(Es*AsPas+Ep*AsAct) 
    Md= max(Md,Vd*z)
    assert z>0
    assert Ae>0
    if denomEpsilonX>0:
        retvalEpsilonX=(Md/z+math.sqrt(Vd**2+(ue*Td/2.0/Ae)**2)+Nd/2.+Fp)/denomEpsilonX  
    else:
        retvalEpsilonX=0
    retvalEpsilonX= max(0,retvalEpsilonX)
    return retvalEpsilonX
  
def getCrackAngleEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue):
    '''getCrackAngleEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue) [units: N, m, rad]
     Return the reference angle of inclination of cracks (in radians) from
     the longitudinal strain in the web. See general method in clause 
     44.2.3.2.2 of EHE-08.

    :param Nd: Design value of axial force (here positive if in tension)
    :param Md: Absolute value of design value of bending moment.
    :param Vd: Absolute value of effective design shear (clause 42.2.2).
    :param Td: design value of torsional moment.
    :param z: Lever arm.
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param Es: reinforcing steel elastic modulus.
    :param Ep: prestressing steel elastic modulus.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Area enclosed by the middle line of the effective hollow section.
    :param ue: Perimeter of the middle line of the effective hollow section.

    '''
    return math.radians(29+7000*getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)) 

def getBetaVcuEHE08(theta, thetaE):
    '''getBetaVcuEHE08(theta,thetaE) [units: N, m, rad]
     Return the value of «beta» for the expression
     of Vcu according to clause 44.2.3.2.2 of EHE-08.

    :param thetaE: Reference angle of inclination of cracks.
    :param theta: angle between the concrete compressed struts and the 
                  member axis (figure 44.2.3.1.a EHE).    
    '''
    cotgTheta= 1/math.tan(theta)
    cotgThetaE= 1/math.tan(thetaE)
    retval=0.0
    if cotgTheta<0.5:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; theta angle (theta= '+str(math.degrees(theta))+') is too small.')
    else:
        if(cotgTheta<cotgThetaE):
            retval= (2*cotgTheta-1)/(2*cotgThetaE-1)
        else:
            if cotgTheta<=2.0:
                retval= (cotgTheta-2)/(cotgThetaE-2)
            else:
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(methodName+'; theta angle (theta= '+str(math.degrees(theta))+') is too big.')
    return retval
  
def getVcuEHE08CrackAngle(fcv, fcd, gammaC, Ncd, Ac, b0, d, AsPas, AsAct, theta, thetaEVcu):
    '''getVcuEHE08(fcv, fcd, gammaC, Ncd, Ac, b0, d, AsPas,AsAct, theta, thetaEVcu) 
     [units: N, m, rad]
     Return the value of Vcu (contribution of the concrete to shear strength)
     for members WITH shear reinforcement, according to clause 
     44.2.3.2.2 of EHE-08.

    :param fcv: effective concrete shear strength. For members without shear 
                reinforcement fcv= min(fck,60MPa). For members with shear 
                reinforcement fcv= min(fck,100MPa). In both cases, if 
                concrete quality control is not direct fcv= 15 MPa.
    :param fcd: design value of concrete compressive strength).
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete
                (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param theta: angle between the concrete compressed struts and the member 
     axis (figure 44.2.3.1.a EHE)
    :param thetaEvCu: reference angle of inclination of cracks (in radians).
    '''
    chi=min(2,1+math.sqrt(200/(d*1000.)))  # must be expressed in meters.
    sgpcd=max(max(Ncd/Ac,-0.3*fcd),-12e6)
    FcvVcu=getFcvEHE08(0.15,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct)
    betaVcu= getBetaVcuEHE08(theta,thetaEVcu)
    return FcvVcu*betaVcu*b0*d

 
def getVcuEHE08(fcv,fcd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue):
    '''getVcuEHE08(fcv,fcd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue) 
     [units: N, m, rad]
     Return the value of Vcu (contribution of the concrete to shear strength)
     for members WITH shear reinforcement, according to clause 
     44.2.3.2.2 of EHE-08.

    :param fcv: effective concrete shear strength. For members without shear 
                reinforcement fcv= min(fck,60MPa). For members with shear 
                reinforcement fcv= min(fck,100MPa). In both cases, if 
                concrete quality control is not direct fcv= 15 MPa.
    :param fcd: design value of concrete compressive strength).
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete
                (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param z: Lever arm.
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param theta: angle between the concrete compressed struts and the member 
     axis (figure 44.2.3.1.a EHE)
    :param Nd: Design value of axial force (positive if in tension)
    :param Md: Absolute value of design value of bending moment.
    :param Vd: Absolute value of effective design shear (clause 42.2.2).
    :param Td: design value of torsional moment.
    :param Es: reinforcing steel elastic modulus.
    :param Ep: prestressing steel elastic modulus.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Area enclosed by the middle line of the effective hollow section.
    :param ue: Perimeter of the middle line of the effective hollow section.
    '''
    thetaEVcu= getCrackAngleEHE08(Nd= Nd, Md= Md, Vd= Vd, Td= Td, z= z, AsPas= AsPas, AsAct= AsAct, Es= Es, Ep= Ep, Fp= Fp, Ae= Ae, ue= ue)
    return getVcuEHE08CrackAngle(fcv= fcv, fcd= fcd, gammaC= gammaC, Ncd= Ncd, Ac= Ac, b0= b0, d= d, AsPas= AsPas, AsAct= AsAct, theta= theta, thetaEVcu= thetaEVcu)

  
def getVu2EHE08SiAt(fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue, circular= False):
    '''getVu2EHE08SiAt(fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue) [units: N, m, rad]. 
     Return the value of Vu2 (shear strength at failure due to tensile force in the web)
     for members WITH shear reinforcement, according to clause 
     44.2.3.2.2 of EHE-08.

    :param fcv: effective concrete shear strength. For members without shear 
                reinforcement fcv= min(fck,60MPa). For members with shear 
                reinforcement fcv= min(fck,100MPa). In both cases, if 
                concrete quality control is not direct fcv= 15 MPa.
    :param fcd: design value of concrete compressive strength.
    :param fyd: design yield strength of the transverse reinforcement.
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete
                (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param z: Lever arm.
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param AsTrsv: transverse reinforcement area.
    :param alpha: angle of the transverse reinforcement with the axis of the part.
    :param theta: angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    :param Nd: Design value of axial force (positive if in tension)
    :param Md: Absolute value of design value of bending moment.
    :param Vd: Absolute value of effective design shear (clause 42.2.2).
    :param Td: design value of torsional moment.
    :param Es: reinforcing steel elastic modulus.
    :param Ep: prestressing steel elastic modulus.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Area enclosed by the middle line of the effective hollow section.
    :param ue: Perimeter of the middle line of the effective hollow section.
    :param circular: if true we reduce the efectiveness of the shear 
                     reinforcement due to the transverse inclination of its
                     elements.
    '''
    return getVcuEHE08(fcv= fcv, fcd= fcd, gammaC= gammaC, Ncd= Ncd, Ac= Ac, b0= b0, d= d, z= z, AsPas= AsPas, AsAct= AsAct, theta= theta, Nd= Nd, Md= Md, Vd= Vd, Td= Td, Es= Es, Ep= Ep, Fp= Fp, Ae= Ae, ue= ue)+getVsuEHE08(z= z, alpha= alpha, theta= theta, AsTrsv= AsTrsv, fyd= fyd, circular= circular)
  
def getVuEHE08SiAt(fck,fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue):
    '''def getVuEHE08SiAt(fck,fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue) [units: N, m, rad]
    Return the value of Vu (section shear strength)
    for members WITH shear reinforcement, according to clause 44.2.3.2.2 
    of EHE-08.

    :param fck: characteristic value of concrete compressive strength.
    :param fcv: effective concrete shear strength. For members without 
           shear reinforcement fcv= min(fck,60MPa). For members with 
           shear reinforcement fcv= min(fck,100MPa).
     In both cases, if concrete quality control is not direct fcv= 15 MPa.
    :param fcd: design value of concrete compressive strength.
    :param fyd: design yield strength of the transverse reinforcement.
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete only (positive if 
                in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param z: Lever arm.
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param AsTrsv: transverse reinforcement area.
    :param alpha: angle of the transverse reinforcement with the axis of the 
                  part.
    :param theta: angle between the concrete compressed struts and the member 
                  axis (figure 44.2.3.1.a EHE).
    :param Nd: Design value of axial force (positive if in tension)
    :param Md: Absolute value of design value of bending moment.
    :param Vd: Absolute value of effective design shear (clause 42.2.2).
    :param Td: design value of torsional moment.
    :param Es: reinforcing steel elastic modulus.
    :param Ep: prestressing steel elastic modulus.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Area enclosed by the middle line of the effective hollow section.
    :param ue: Perimeter of the middle line of the effective hollow section.
    '''
    return  min(getVu2EHE08SiAt(fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv,alpha,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue),getVu1EHE08(fck,fcd,Ncd,Ac,b0,d,alpha,theta))



#       ___         _   _ _           
#      | _ )_  _ __| |_| (_)_ _  __ _ 
#      | _ \ || / _| / / | | ' \/ _` |
#      |___/\_,_\__|_\_\_|_|_||_\__, |
#                               |___/ 
# 
# Check buckling approximate method. Straight combined bending (clause 43.5.1 of EHE-08).

def get_buckling_e1_e2_eccentricities(Nd:float, MdMax:float, MdMin:float):
    ''' Compute the e1 and e2 eccentricities as defined in the clause 43.1.2 of
        EHE-08.

    :param Nd: design axial force.
    :param MdMax: maximum bending moment on the member to be checked.
    :param MdMin: minimum bending moment on the member to be checked.
    '''
    sign = functools.partial(math.copysign, 1)
    if(Nd>0.0):
        methodName= sys._getframe(0).f_code.co_name
        errMsg= methodName+"; member is in tension; no need to perform buckling check.\n"
        lmsg.error(errMsg)
    if(MdMax<MdMin): # Make sure that MdMax>MdMin
        MdMax, MdMin= MdMin, MdMax
    absMdMax= abs(MdMax)
    absMdMin= abs(MdMin)
    if(absMdMax>absMdMin):
        e2= -absMdMax/Nd # e2>0
        if(sign(MdMax)==sign(MdMin)):
            e1= -absMdMin/Nd  # e1>0
        else:
            e1= absMdMin/Nd # e1<0
    else:
        e2= -absMdMin/Nd # e2>0
        if(sign(MdMax)==sign(MdMin)):
            e1= -absMdMax/Nd  # e1>0
        else:
            e1= absMdMax/Nd # e1<0
    return e1, e2

def get_element_buckling_eccentricities(element):
    ''' Compute the e1 and e2 eccentricities as defined in the clause 43.1.2 of
        EHE-08 using the internal forces of the element.

    :param element: element to compute the buckling eccentricities for.
    '''
    nDOF= element.getNumDOF() # Number of degrees of freedom.
    N= element.getN() # Axial load.
    if(nDOF==6): # 2D element.
        M1= element.getM1 # Bending moments.
        M2= element.getM2
        if(M1>M2):
            ez1, ez2= get_buckling_e1_e2_eccentricities(Nd= N, MdMax= M1, MdMin= M2)
        else:
            ez1, ez2= get_buckling_e1_e2_eccentricities(Nd= N, MdMax= M2, MdMin= M1)
        retval= (ez1, ez2)
    elif(nDOF==12): # 3D element.
        Mz1= element.getMz1; Mz2= element.getMz2
        if(Mz1>Mz2):
            ez1, ez2= get_buckling_e1_e2_eccentricities(Nd= N, MdMax= Mz1, MdMin= Mz2)
        else:
            ez1, ez2= get_buckling_e1_e2_eccentricities(Nd= N, MdMax= Mz2, MdMin= Mz1)
        My1= element.getMy1; My2= element.getMy2
        if(My1>My2):
            ey1, ey2= get_buckling_e1_e2_eccentricities(Nd= N, MdMax= My1, MdMin= My2)
        else:
            ey1, ey2= get_buckling_e1_e2_eccentricities(Nd= N, MdMax= My2, MdMin= My1)
        retval= (ez1, ez2, ey1, ey2)
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        errMsg= className+'.'+methodName+"; not implemented for elements with. " + str(nDOF) + " degrees of freedom."
        lmsg.error(errMsg)
        
    return retval
        
def get_fictitious_eccentricity(sectionDepth:float, firstOrderEccentricity, reinforcementFactor:float, epsilon_y:float, radiusOfGyration:float, bucklingLength:float):
    ''' Return the fictitious eccentricity used to represent the second order 
        effects according to clause 43.5.1 of EHE-08.

    :param sectionDepth: total depth of the concrete section in the bending plane considered.
    :param firstOrderEccentricity: equivalent first order design eccentricity.
    :param reinforcementFactor: reinforcement factor computed as $\beta= \frac{(d-d')^2}{4*i_s^2}$ with $i_s$ being the radius of gyration of the reinforcements.
    :param epsilon_y: deformation in the steel for the design stress fyd.
    :param radiusOfGyration: radius of gyration of the concrete section in the direction concerned.
    :param bucklingLength: buckling length.
    '''
    return (1+0.12*reinforcementFactor)*(epsilon_y+0.0035)*(sectionDepth+20*firstOrderEccentricity)/(sectionDepth+10*firstOrderEccentricity)*bucklingLength**2/50.0/radiusOfGyration

def get_lower_slenderness_limit(C:float, nonDimensionalAxialForce:float, e1, e2, sectionDepth):
    ''' Return the lower slenderness limit according to clause 43.1.2 of EHE-08.

    :param C: Coefficient which depends on the configuration of reinforcements
              whose values are: 0.24 for symmetrical reinforcement on two 
              opposing sides in the bending plane, 0.20 for equal reinforcement on the four sides, 0.16 for symmetrical reinforcement on the lateral sides.
    :param nonDimensionalAxialForce: design value of the non-dimensional or 
                                     reduced axial force actuating in the 
                                     support.
    :param e1: First order eccentricity at the end of the support with the 
               lower moment which is positive if it has the same sign as e2.
    :param e2: First order eccentricity in the end of the support with the 
               larger moment, deemed to be positive.

    :param sectionDepth: total depth of the concrete section in the bending plane considered.
    '''
    if(e2<0.0):
        methodName= sys._getframe(0).f_code.co_name
        errMsg= methodName+"; e2 must be positive.\n"
        lmsg.error(errMsg)
    e2= max(sectionDepth/20.0, e2) # consider minimum eccentricity as sectionDepth/20.0
    if(abs(e1)>abs(e2)):
        methodName= sys._getframe(0).f_code.co_name
        errMsg= methodName+"; e2 must be greater or equal that e1.\n"
        lmsg.error(errMsg)

    retval= 1+0.24/(e2/sectionDepth)+3.4*(e1/e2-1)**2
    retval*= C/nonDimensionalAxialForce
    retval= min(35.0*math.sqrt(retval),100.0)
    return retval



def get_buckling_parameters(element, bucklingLoadFactors, rcSection, sectionDepthZ, Cz, reinforcementFactorZ, sectionDepthY= None, Cy= None, reinforcementFactorY= None, eigenvectorNormThreshold= 1e-3, alpha_cr_threshold= 10.0):
    ''' Return the effective length, mechanical slenderness and fictitious eccentricity for the given buckling load factors.

    :param element: element to compute the buckling parameters for.
    :param bucklingLoadFactors: list containing the buckling load factors obtained from the linear buckling analysis.
    :param rcSection: reinforced concrete section of the element.
    :param sectionDepthZ: depth of the reinforced concrete section measured along the z axis.
    :param Cz: Coefficient which depends on the configuration of reinforcements
              whose values are: 0.24 for symmetrical reinforcement on two 
              opposing sides in the bending plane, 0.20 for equal reinforcement on the four sides, 0.16 for symmetrical reinforcement on the lateral sides.
    :param reinforcementFactorZ: reinforcement factor computed as $\beta= \frac{(d-d')^2}{4*i_s^2}$ with $i_s$ being the radius of gyration of the reinforcements about the z axis.
    :param sectionDepthY: depth of the reinforced concrete section measured along the y axis.
    :param Cy: Coefficient which depends on the configuration of reinforcements
              whose values are: 0.24 for symmetrical reinforcement on two 
              opposing sides in the bending plane, 0.20 for equal reinforcement on the four sides, 0.16 for symmetrical reinforcement on the lateral sides.
    :param reinforcementFactorY: reinforcement factor computed as $\beta= \frac{(d-d')^2}{4*i_s^2}$ with $i_s$ being the radius of gyration of the reinforcements about the y axis.
    :param eigenvectorNormThreshold: if the node eigenvector has a norm smaller
                                     than this threshold it is considered null.
    :param alpha_cr_threshold: # Lower limit for the Ncr/N factor by which the design loading would have to be increased to cause elastic instability (see Eurocode 4:2004 cl.5.2.1(2)). For values of alpha_cr greater than alpha_cr_threshold the instability effect is ignored.
    '''
    maxMechLambda= 100.0 # According to clause 43.5 of EHE-08.
    N= element.getN() # Element axial load.
    Ncri= [x * N for x in bucklingLoadFactors]
    nonDimensionalAxialForce= rcSection.getNonDimensionalAxialForce(N) # Non-dimensional axial force.
    steel= rcSection.getReinfSteelType()
    nDOF= element.getNumDOF() # Number of degrees of freedom.
    section= element.physicalProperties.getVectorMaterials[0]
    Leffi= list() # Effective lengths for each mode.
    mechLambdai= list() # Mechanical slenderness for each mode.
    Efi= list() # Fictitious eccentricity for each mode.
    strongAxisBucklingPercent= list()
    alpha_cr_i= list() # Ncr/N factor by which the design loading would have to be increased to cause elastic instability (see Eurocode 4:2004 cl.5.2.1(2)).
    if(nDOF==6): # 2D element.
        e1, e2= get_element_buckling_eccentricities(element) # Compute eccentricities according to clause 43.1.2
        EI= section.sectionProperties.EI()
        iz= section.sectionProperties.i # Radius of gyration.
        # Lower slenderness limit.
        lowerSlendernessLimit= get_lower_slenderness_limit(C= Cz, nonDimensionalAxialForce= nonDimensionalAxialForce, e1= e1, e2= e2, sectionDepth= sectionDepthZ)
        minimumEccentricity= max(.02, sectionDepthZ/20)
        for mode, Ncr in enumerate(Ncri):
            Leff= math.pi * math.sqrt(EI/abs(Ncr)) # Effective length.
            if(Ncr>0):
                Leff= -Leff
            Leffi.append(Leff)
            mechLambda= Leff/iz # Compute mechanical slenderness
            mechLambdai.append(mechLambda)
            if(mechLambda>maxMechLambda):
                methodName= sys._getframe(0).f_code.co_name
                warningMsg= methodName+"; mechanical slenderness exceeded in mode: "+ str(mode+1) + "for this simplified method: (" + str(mechLambda) + ">"+str(maxMechLambda)+"), load factor: "+str(bucklingLoadFactors[mode])+"."
                lmsg.warning(warningMsg)
            if(mechLambda<lowerSlendernessLimit):
                ef= minimumEccentricity
            else:
                ef= max(minimumEccentricity, get_fictitious_eccentricity(sectionDepth= sectionDepthZ, firstOrderEccentricity= e2, reinforcementFactor= reinforcementFactorZ, epsilon_y= steel.eyd(), radiusOfGyration= iz, bucklingLength= Leff))
            Efi.append(ef)
            strongAxisBucklingPercent.append(0.0) # can't buckle around another axis.
            if(abs(N)>0):
                alpha_cr_i.append(Ncr/N)
            else:
                alpha_cr_i.append(1e3)
    elif(nDOF==12):
        ez1, ez2, ey1, ey2= get_element_buckling_eccentricities(element) # Compute eccentricities according to clause 43.1.2
        elementWeakAxis= element.getVDirWeakAxisGlobalCoord(True) # initialGeometry= True
        # Get the element nodes.
        nodes= element.nodes
        coordTransf= element.getCoordTransf
        EA= section.sectionProperties.EA()
        EIz= section.sectionProperties.EIz()
        EIy= section.sectionProperties.EIy()
        iz= section.sectionProperties.iz # Radius of gyration about z axis.
        iy= section.sectionProperties.iy # Radius of gyration about y axis.
        # Lower slenderness limit.
        lowerSlendernessLimitZ= get_lower_slenderness_limit(C= Cz, nonDimensionalAxialForce= nonDimensionalAxialForce, e1= ez1, e2= ez2, sectionDepth= sectionDepthZ)
        minimumEccentricityZ= max(.02, sectionDepthZ/20)
        lowerSlendernessLimitY= get_lower_slenderness_limit(C= Cy, nonDimensionalAxialForce= nonDimensionalAxialForce, e1= ey1, e2= ey2, sectionDepth= sectionDepthY)
        minimumEccentricityY= max(.02, sectionDepthY/20)
        for mode, Ncr in enumerate(Ncri):
            if(abs(N)>0):
                alpha_cr= Ncr/N
            else:
                alpha_cr= 1e3
            alpha_cr_i.append(alpha_cr)
            mode1= mode+1
            node0Eigenvector= nodes[0].getEigenvector(mode1)
            node0EigenvectorNorm= node0Eigenvector.Norm()
            node1Eigenvector= nodes[1].getEigenvector(mode1)
            node1EigenvectorNorm= node1Eigenvector.Norm()
            if(((node0EigenvectorNorm>eigenvectorNormThreshold) or (node1EigenvectorNorm>eigenvectorNormThreshold)) and (alpha_cr<alpha_cr_threshold)):
                # Compute the stiffness in the direction of buckling for this mode.
                ## Check if it is a translational buckling.
                globalEigenvector= (0.5*(node0Eigenvector+node1Eigenvector)).Normalized()
                # index_max= min(range(len(globalEigenvector)), key=globalEigenvector.__getitem__)
                globalEigenvectorDisp= xc.Vector(list(globalEigenvector)[0:3]) # Displacement components of the eigenvector.
                globalEigenvectorRot= xc.Vector(list(globalEigenvector)[3:6]) # Rotational components of the eigenvector.
                ## Get the direction of buckling.
                ### Convert to local coordinates.
                localEigenvectorDisp= coordTransf.getVectorLocalCoordFromGlobal(globalEigenvectorDisp)
                normDisp= localEigenvectorDisp.Norm()
                localEigenvectorRot= coordTransf.getVectorLocalCoordFromGlobal(globalEigenvectorRot)
                normRot= localEigenvectorRot.Norm()
                # Compute the dot product of the displacement eigenvector
                # and the element weak axis.
                dotProduct= abs(elementWeakAxis.dot(globalEigenvectorDisp))
                if(normDisp>normRot):
                    ### Remove axial (x) component and normalize.
                    localEigenvectorDispYZ= xc.Vector([localEigenvectorDisp[1], localEigenvectorDisp[2]])
                    # Normalize
                    localEigenvectorDispYZ= localEigenvectorDispYZ.Normalized()
                    ## Compute the projected stiffness
                    sectionStiffnessMatrix= xc.Matrix([[EIz, 0],[0, EIy]])
                    EI= localEigenvectorDispYZ.dot(sectionStiffnessMatrix*localEigenvectorDispYZ)
                    ## Compute the projected lower slenderness limit.
                    lowerSlendernessLimitVector= xc.Vector([lowerSlendernessLimitZ, lowerSlendernessLimitY])
                    lowerSlendernessLimit=  lowerSlendernessLimitVector.dot(localEigenvectorDispYZ)
                    # Compute the effective length.
                    absNcr= abs(Ncr)
                    Leff= math.pi*math.sqrt(EI/absNcr) # Effective length.
                    if(Ncr>0):
                        Leff= -Leff
                    Leffi.append(Leff)
                    if(dotProduct>eigenvectorNormThreshold):
                        sphericTensor= (abs(EIz-EIy)<1e-6)
                        if(sphericTensor): # both axes are strong (or weak)
                            dotProduct= 0.0
                    # Compute the mechanical slenderness
                    i_mode= math.sqrt(EI/EA) # radius of giration.
                    mechLambda= Leff/i_mode # Compute mechanical slenderness
                    if(mechLambda>maxMechLambda):
                        methodName= sys._getframe(0).f_code.co_name
                        warningMsg= methodName+"; mechanical slenderness exceeded in mode: "+ str(mode1) + "for this simplified method: (" + str(mechLambda) + ">"+str(maxMechLambda)+"), load factor: "+str(bucklingLoadFactors[mode])+"."
                        lmsg.warning(warningMsg)
                    mechLambdai.append(mechLambda)
                    ## Compute the projected minimum eccentricity.
                    minimumEccentricityVector= xc.Vector([minimumEccentricityZ, minimumEccentricityY])
                    minimumEccentricity=  minimumEccentricityVector.dot(localEigenvectorDispYZ)
                    ## Compute the projected section depth.
                    sectionDepthVector= xc.Vector([sectionDepthY, sectionDepthZ])
                    sectionDepth= abs(sectionDepthVector.dot(localEigenvectorDispYZ))
                    ## Compute the projected first order eccentricity
                    firstOrderEccentricityVector= xc.Vector([ez2, ey2])
                    firstOrderEccentricity=  firstOrderEccentricityVector.dot(localEigenvectorDispYZ)
                    ## Compute the projected reinforcementFactor.
                    reinforcementFactorVector= xc.Vector([reinforcementFactorZ, reinforcementFactorY])
                    reinforcementFactor=  abs(reinforcementFactorVector.dot(localEigenvectorDispYZ))
                    # Compute fictitious eccentricity.
                    if(mechLambda<lowerSlendernessLimit):
                        ef= minimumEccentricity
                    else:
                        ef= get_fictitious_eccentricity(sectionDepth= sectionDepth, firstOrderEccentricity= firstOrderEccentricity, reinforcementFactor= reinforcementFactor, epsilon_y= steel.eyd(), radiusOfGyration= i_mode, bucklingLength= Leff)
                        ef= max(minimumEccentricity, ef)
                    ef= ef*localEigenvectorDispYZ
                    efz= ef[0]
                    if(abs(efz/sectionDepth)<1e-3):
                        efz= 0.0
                    efy= ef[1]
                    if(abs(efy/sectionDepth)<1e-3):
                        efy= 0.0
                    Efi.append((abs(efz), abs(efy)))
                else: # torsional buckling
                    # Assume a low mechanical slenderness.
                    mechLambda= 0.1*min(lowerSlendernessLimitZ, lowerSlendernessLimitY)
                    mechLambdai.append(mechLambda)
                    # Compute effective length from this mechanical slenderness. 
                    iO= math.sqrt(iz**2+iy**2) # +yO**2+zO**2 # (y0 and z0 Coordinates of the shear center with respect to the centroid)
                    Leff= mechLambda*iO # Compute mechanical slenderness
                    Leffi.append(Leff)
                    # No torsional buckling for concrete sections so minimum eccentricities.
                    efz= minimumEccentricityZ
                    efy= minimumEccentricityY
                    Efi.append((efz, efy))
                strongAxisBucklingPercent.append(dotProduct)
            else: # Element not affected by this buckling mode.
                # Assume a low mechanical slenderness.
                mechLambda= 0.1*min(lowerSlendernessLimitZ, lowerSlendernessLimitY)
                mechLambdai.append(mechLambda)
                # Compute effective length from this mechanical slenderness. 
                iO= math.sqrt(iz**2+iy**2) # +yO**2+zO**2 # (y0 and z0 Coordinates of the shear center with respect to the centroid)
                Leff= mechLambda*iO # Compute mechanical slenderness
                Leffi.append(Leff)
                efz= minimumEccentricityZ
                efy= minimumEccentricityY
                Efi.append((efz, efy))
                strongAxisBucklingPercent.append(0.0)
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        errMsg= className+'.'+methodName+"; not implemented for elements with. " + str(nDOF) + " degrees of freedom."
        lmsg.error(errMsg)
    retval= {'Leffi':Leffi, 'mechLambdai': mechLambdai, 'Efi': Efi, 'strongAxisBucklingPercent': strongAxisBucklingPercent, 'alpha_cr_i':alpha_cr_i}
    return retval

class SectionBucklingProperties(object):
    ''' Properties that define the buckling behavior of the RC section
        according to clause 43.1 of EHE-08.

    :ivar reinforcementFactorZ: reinforcement factor according to table 43.5.1 of EHE-08 (z axis).
    :ivar sectionDepthZ: section depth (z axis).
    :ivar Cz: clause 43.1.2 of EHE-08 (z axis).
    :ivar reinforcementFactorY: reinforcement factor according to table 43.5.1 of EHE-08 (y axis).
    :ivar sectionDepthY: section depth (y axis).
    :ivar Cy: clause 43.1.2 of EHE-08 (y axis).
    :ivar sectionData:  object derived from RCSectionBase used to define the reinforced concrete section.
    '''
    def __init__(self, reinforcementFactorZ, sectionDepthZ, Cz,reinforcementFactorY, sectionDepthY, Cy, sectionObject):
        ''' Constructor.

        :param reinforcementFactorZ: reinforcement factor according to table 43.5.1 of EHE-08 (z axis).
        :param sectionDepthZ: section depth (z axis).
        :param Cz: clause 43.1.2 of EHE-08 (z axis).
        :param reinforcementFactorY: reinforcement factor according to table 43.5.1 of EHE-08 (y axis).
        :param sectionDepthY: section depth (y axis).
        :param Cy: clause 43.1.2 of EHE-08 (y axis).
        :param sectionObject:  object derived from RCSectionBase used to define the reinforced concrete section.
        '''
        self.reinforcementFactorZ= reinforcementFactorZ
        self.sectionDepthZ= sectionDepthZ
        self.Cz= Cz
        self.reinforcementFactorY= reinforcementFactorY
        self.sectionDepthY= sectionDepthY
        self.Cy= Cy
        self.sectionObject= sectionObject
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= {'reinforcementFactorZ': self.reinforcementFactorZ,
                 'sectionDepthZ': self.sectionDepthZ,
                 'reinforcementFactorY': self.reinforcementFactorY,
                 'sectionDepthY': self.sectionDepthY,
                 'Cz': self.Cz,
                 'Cy': self.Cy,
                 'sectionObject': self.sectionObject}
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        self.reinforcementFactorZ= dct['reinforcementFactorZ']
        self.sectionDepthZ= dct['sectionDepthZ']
        self.Cz= dct['Cz']
        self.reinforcementFactorY= dct['reinforcementFactorY']
        self.sectionDepthY= dct['sectionDepthY']
        self.Cy= dct['Cy']
        self.sectionObject= dct['sectionObject']
        
    @classmethod
    def newFromDict(cls, dct= None):
        ''' Builds a new object from the data in the given dictionary.

        :param cls: class of the object itself.
        :param dct: dictionary contaning the data.
        '''
        newObject = cls.__new__(cls) # just object.__new__
        if(dct):
            newObject.setFromDict(dct)
        return newObject
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= (self.reinforcementFactorZ == other.reinforcementFactorZ)
                if(retval):
                    retval= (self.sectionDepthZ == other.sectionDepthZ)
                if(retval):
                    retval= (self.Cz == other.Cz)
                if(retval):
                    retval= (self.reinforcementFactorY == other.reinforcementFactorY)
                if(retval):
                    retval= (self.sectionDepthY == other.sectionDepthY)
                if(retval):
                    retval= (self.Cy == other.Cy)
                if(retval):
                    retval= (self.sectionObject == other.sectionObject)
            else:
                retval= True
        else:
            retval= False
        return retval
    
class BucklingParametersLimitStateData(lsd.BucklingParametersLimitStateData):
    
    ''' Buckling parameters data for limit state checking.
    '''
    def __init__(self, numModes= 4, limitStateLabel= 'ULS_bucklingParametersComputation', outputDataBaseFileName= fn.bucklingVerificationResultsFile, designSituations= lsd.default_uls_design_situations, eigenvectorNormThreshold= 1e-3, alpha_cr_threshold= 10.0):
        '''Constructor

        :param numModes: number of buckling modes to compute.
        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, uls_earthquake, etc.
        :param eigenvectorNormThreshold: if the node eigenvector has a norm 
                                         smaller than this threshold it is 
                                         considered null.
        :param alpha_cr_threshold: # Lower limit for the Ncr/N factor by which the design loading would have to be increased to cause elastic instability (see Eurocode 4:2004 cl.5.2.1(2)). For values of alpha_cr greater than alpha_cr_threshold the instability effect is ignored.
        '''
        super(BucklingParametersLimitStateData, self).__init__(numModes= numModes, limitStateLabel= limitStateLabel, outputDataBaseFileName= outputDataBaseFileName, designSituations= designSituations)
        self.eigenvectorNormThreshold= eigenvectorNormThreshold
        self.alpha_cr_threshold= alpha_cr_threshold

    def getController(self, biaxialBuckling= True):
        ''' Return a controller corresponding to this limit state.

        :param biaxialBuckling: if True, consider bending aroung both 
                                cross-section axes.
        '''
        if(biaxialBuckling):
            return BiaxialBucklingController(limitStateLabel= self.label)
        else:
            return UniaxialBucklingController(limitStateLabel= self.label)
        
    def getEHEBucklingParametersDict(self, nmbComb, xcSet):
        '''Creates a dictionary with the buckling parameters of the given
           elements.

        :param nmbComb: combination name.
        :param xcSet: set containing the nodes to export the modes on.
        '''
        retval= dict()
        preprocessor= xcSet.getPreprocessor
        domain= preprocessor.getDomain
        eigenvalues= domain.getEigenvaluesList()
        # Normalize eigenvectors if needed.
        for mode in range(0, len(eigenvalues)):
            norm= domain.getMesh.getEigenvectorsMaxNormInf(mode+1)
            if(abs(norm-1.0)>1e-4):
                norm= domain.getMesh.normalizeEigenvectors(mode+1)
        retval['buckling_load_factors']= eigenvalues
        elementParametersDict= dict()
        retval['element_parameters']= elementParametersDict
        for e in xcSet.elements:
            elementBucklingParameters= {'Leffi':None, 'mechLambdai':None, 'Efi':None, 'strongAxisBucklingPercent':None}
            section= e.physicalProperties.getVectorMaterials[0]
            if(section.hasProp('sectionBucklingProperties')):
                sectionBucklingProperties= section.getProp('sectionBucklingProperties')
                reinforcementFactorZ= sectionBucklingProperties.reinforcementFactorZ
                sectionDepthZ= sectionBucklingProperties.sectionDepthZ
                reinforcementFactorY= sectionBucklingProperties.reinforcementFactorY
                sectionDepthY= sectionBucklingProperties.sectionDepthY
                Cz= sectionBucklingProperties.Cz
                Cy= sectionBucklingProperties.Cy
                rcSection= sectionBucklingProperties.sectionObject
                elementBucklingParameters= get_buckling_parameters(element= e, rcSection= rcSection, bucklingLoadFactors= eigenvalues, sectionDepthZ= sectionDepthZ, Cz= Cz, reinforcementFactorZ= reinforcementFactorZ, sectionDepthY= sectionDepthY, Cy= Cy, reinforcementFactorY= reinforcementFactorY, eigenvectorNormThreshold= self.eigenvectorNormThreshold, alpha_cr_threshold= self.alpha_cr_threshold)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                errMsg= className+'.'+methodName+"; reinforced concrete for element: "+str(e.tag)+" is not defined. Can't compute buckling parameters for this element."
                lmsg.error(errMsg)
                
            elementParametersDict[e.tag]= elementBucklingParameters
        return retval
        
    def prepareResultsDictionaries(self):
        ''' Prepare the dictionaries to store the results of the analysis.'''
        super().prepareResultsDictionaries()
        self.eheBucklingParametersDict= dict()
        
    def updateResults(self, combName, calcSet, constrainedNodes= None):
        ''' Append the results of the current analysis to the results 
            dictionaries.

        :param combName: load combination corresponding to the current analysis.
        :param setCalc: set of entities for which the verification is 
                          going to be performed.
        :param constrainedNodes: constrained nodes (defaults to None)
        '''
        super().updateResults(combName= combName, calcSet= calcSet, constrainedNodes= constrainedNodes)
        self.eheBucklingParametersDict.update(self.getEHEBucklingParametersDict(combName, xcSet= calcSet))
        
    def getResultsDict(self):
        ''' Build a dictionary containing all the analysis results.'''
        retval= super().getResultsDict()
        retval['ehe_buckling_parameters']= self.eheBucklingParametersDict
        return retval
    
    def getInternalForcesTuple(self, setCalc):
        ''' Read the element tags, load combinations identifiers and internal
            forces for the elements in the given set and return them in a 
            tuple: (eTags, loadCombinations, internalForces). The internal
            forces are preprocessing to increase the bending moments
            according to the results of the buckling analysis.

        :param setCalc: elements to read internal forces for.
        '''
        bucklingAnalysisResultsFileName= self.getBucklingAnalysisResultsFileName()
    
        intForcItems= read_buckling_analysis_results(bucklingAnalysisResultsFileName= bucklingAnalysisResultsFileName, setCalc= setCalc)
        # intForcItems: tuple containing the element tags, the identifiers
        # of the load combinations and the values of the
        # internal forces.
        elementTags= intForcItems[0]
        loadCombinations= intForcItems[1]
        internalForces= intForcItems[2]
        bucklingParameters= intForcItems[3]
        bucklingLoadFactors= intForcItems[4]
        idCombs= set() # load combinations x modes
        newInternalForces= defaultdict(list) # load combination results for each mode.
        if(len(bucklingParameters)>0):
            numModes= len(bucklingLoadFactors)
            modes= list(range(0, numModes))
            ## Compute the new internal forces.
            for loadComb in loadCombinations:
                for mode in modes: # for each mode under this combination.
                    loadCombMode= loadComb+'_mode_'+str(mode+1)
                    idCombs.add(loadCombMode) # load combination results for mode i.                
                    for eTag in elementTags: # for each element.
                        elementBucklingParameters= bucklingParameters[eTag]
                        # Get fictitious eccentricities
                        efi= elementBucklingParameters['Efi']
                        elementSectionsInternalForces= internalForces[eTag]
                        efz= efi[mode][0] # fictitious eccentricity Z axis.
                        efy= efi[mode][1] # fictitious eccentricity Y axis.
                        # newElementSectionsInternalForces= list()
                        for iForces in elementSectionsInternalForces:
                            if(iForces.idComb==loadComb): # if the internal forces correspond to this load combination
                                newForces= iForces.getCopy()
                                newForces.increaseEccentricities(ez= efz, ey= efy)
                                newForces.idComb= loadCombMode
                                newInternalForces[eTag].append(newForces)            
        return (elementTags, idCombs, newInternalForces)
    
            
#       _    _       _ _        _        _       
#      | |  (_)_ __ (_) |_   __| |_ __ _| |_ ___ 
#      | |__| | '  \| |  _| (_-<  _/ _` |  _/ -_)
#      |____|_|_|_|_|_|\__| /__/\__\__,_|\__\___|
#       __ ___ _ _| |_ _ _ ___| | |___ _ _ ___   
#      / _/ _ \ ' \  _| '_/ _ \ | / -_) '_(_-<   
#      \__\___/_||_\__|_| \___/_|_\___|_| /__/   
# Limit state controllers.

# Check normal stresses limit state.
class BiaxialBendingNormalStressController(lscb.BiaxialBendingNormalStressControllerBase):
    '''Object that controls normal stresses limit state.'''

    def __init__(self,limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel)

class UniaxialBendingNormalStressController(lscb.UniaxialBendingNormalStressControllerBase):
    '''Object that controls normal stresses limit state (uniaxial bending).'''

    def __init__(self,limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(UniaxialBendingNormalStressController,self).__init__(limitStateLabel)
        
class NormalStressesLimitStateData(lsd.NormalStressesRCLimitStateData):
    ''' Reinforced concrete normal stresses data for limit state checking.'''
    
    def getController(self, biaxialBending= True):
        ''' Return a controller corresponding to this limit state.

        :param biaxialBending: if True use a controller that checks bending
                               around both cross-section axes.
        '''
        retval= None
        if(biaxialBending):
            retval= BiaxialBendingNormalStressController(self.label)
        else:
            retval= UniaxialBendingNormalStressController(self.label)
        return retval
    
normalStressesResistance= NormalStressesLimitStateData()

# Buckling checking.
class BiaxialBucklingController(BiaxialBendingNormalStressController):
    '''Object that controls buckling limit state.'''

    ControlVars= cv.BiaxialBucklingControlVars
    
    def __init__(self,limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(BiaxialBucklingController, self).__init__(limitStateLabel)
        
class UniaxialBucklingController(UniaxialBendingNormalStressController):
    '''Object that controls buckling limit state (uniaxial bending).'''

    ControlVars= cv.UniaxialBucklingControlVars
    
    def __init__(self, limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(UniaxialBucklingController, self).__init__(limitStateLabel)
        
# Shear checking.

class ShearController(lscb.ShearControllerBase):
    '''Shear control according to EHE-08.'''

    ControlVars= cv.SIATypeRCShearControlVars
    
    def __init__(self, limitStateLabel, solutionProcedureType= lscb.defaultStaticNonLinearSolutionProcedure):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(ShearController,self).__init__(limitStateLabel= limitStateLabel, fakeSection= False, solutionProcedureType= solutionProcedureType)
        self.concreteFibersSetName= "concrete" #Name of the concrete fibers set.
        self.rebarFibersSetName= "reinforcement" #Name of the rebar fibers set.
        self.isBending= False # True if there is ar bending moment.
        self.fcvH= 0.0 # effective concrete shear strength.
        self.fckH= 0.0 # characteristic value of concrete compressive strength.
        self.fcdH= 0.0 # design value of concrete compressive strength.
        self.fctdH= 0.0 # design tensile strength of the concrete.
        self.gammaC= 0.0 # partial safety factor for concrete.
        self.fydS= 0.0 # design value of reinforcement steel strength.
        self.effectiveDepth= 0.0 # current effective depth
        self.mechanicLeverArm= 0.0 # current value of mechanic lever arm.
        self.strutWidth= 0.0 # compressed strut width «b0».
        self.I= 0.0 # moment of inertia of the section with respect to the neutral axis in elastic range.
        self.S= 0.0 # first moment of area of the section with respect to the neutral axis in elastic range.
        self.concreteArea= 0.0 #Concrete area of the section.
        self.tensionedRebars= lscb.TensionedRebarsBasicProperties()
        self.eps1= 0.0 #Maximum strain in concrete.
        self.concreteAxialForce= 0.0 # Axial force in concrete.
        self.E0= 0.0 #Concrete tangent stiffness.

        self.alphaL= 1.0 #Factor que depende de la transferencia de pretensado.
        self.AsTrsv= 0.0 #Shear reinforcement area.
        self.alpha= math.radians(90) # angle of the shear reinforcement with the member axis (figure 44.2.3.1 EHE-08).
        self.theta= math.radians(45) #Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
        self.thetaMin= math.atan(0.5) # Minimal value of the theta angle.
        self.thetaMax= math.atan(2) # Maximal value of the theta angle.

        self.thetaFisuras= 0.0 # angle of the cracks with the member axis.
        self.Vcu= 0.0 # contribution of the concrete to shear strength.
        self.Vsu= 0.0 # contribution of the web’s transverse reinforcement to shear strength.
        self.Vu1= 0.0 # Shear strength at failure due to diagonal compression in the web.
        self.Vu2= 0.0 # Shear strength at failure due to tensile force in the web
        self.Vu= 0.0 # Shear strength at failure.

    def calcVuEHE08NoAt(self, scc, rcSets):
        ''' Compute the shear strength at failure without shear reinforcement
         according to clause 44.2.3.2.1 of EHE-08.
         XXX Presstressing contribution not implemented yet.

         :param scc: fiber model of the section.
         :param rcSets: fiber sets in the reinforced concrete section.
        '''
        # concrFibers= rcSets.concrFibers.fSet
        self.concreteArea= rcSets.getConcreteArea(1)
        self.Vsu= 0.0 # No shear reinforcement.
        if(self.concreteArea<1e-6):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= className+'.'+methodName+"; concrete area too small; Ac= " + str(self.concreteArea) + " m2\n"
            lmsg.error(errMsg)
        else:
            tensionedReinforcement= rcSets.tensionFibers
            self.isBending= scc.isSubjectedToBending(0.1)
            self.tensionedRebars.number= rcSets.getNumTensionRebars()
            self.E0= rcSets.getConcreteInitialTangent()
            if(self.isBending):
                self.eps1= rcSets.getMaxConcreteStrain()
                self.concreteAxialForce= rcSets.getConcreteCompression()
                self.strutWidth= scc.getCompressedStrutWidth() # b0
                if((self.E0*self.eps1)<self.fctdH): # Non cracked section
                    self.I= scc.getHomogenizedI(self.E0)
                    self.S= scc.getSPosHomogenized(self.E0)
                    self.Vu2= getVu2EHE08NoAtNoFis(self.fctdH,self.I,self.S,self.strutWidth,self.alphaL,self.concreteAxialForce,self.concreteArea)
                else: # Cracked section
                  self.effectiveDepth= scc.getEffectiveDepth() # d
                  if(self.tensionedRebars.number>0):
                      self.tensionedRebarsArea= tensionedReinforcement.getArea(1)
                  else:
                      self.tensionedRebarsArea= 0.0
                  self.Vu2= getVu2EHE08NoAtSiFis(self.fckH,self.fcdH,self.gammaC,self.concreteAxialForce,self.concreteArea,self.strutWidth,self.effectiveDepth,self.tensionedRebarsArea,0.0)
                self.Vu1= -1.0
            else: # Uncracked section
                axes= scc.getInternalForcesAxes()
                self.I= scc.getFibers().getHomogenizedSectionIRelToLine(self.E0,axes)
                self.S= scc.getFibers().getSPosHomogenizedSection(self.E0,geom.HalfPlane2d(axes))
                self.strutWidth= scc.getCompressedStrutWidth() # b0
                self.Vu2= getVu2EHE08NoAtNoFis(self.fctdH,self.I,self.S,self.strutWidth,self.alphaL,self.concreteAxialForce,self.concreteArea)
            self.Vcu= self.Vu2
            self.Vu= self.Vu2

    def calcVuEHE08SiAt(self, scc, torsionParameters, Nd, Md, Vd, Td, rcSets, circular= False):
        ''' Compute the shear strength at failure WITH shear reinforcement.
         XXX Presstressing contribution not implemented yet.

         :param scc: fiber model of the section.
         :param torsionParameters: parameters that define torsional behaviour 
                                   of the section as in clause 45.1 of EHE-08.
         :param Nd: Design value of axial force (here positive if in tension)
         :param Md: Absolute value of design value of bending moment.
         :param Vd: Absolute value of effective design shear (clause 42.2.2).
         :param Td: design value of torsional moment.
         :param rcSets: fiber sets in the reinforced concrete section.
         :param circular: if true we reduce the efectiveness of the shear 
                          reinforcement due to the transverse inclination of
                          its elements.
        '''
        if(torsionParameters):
            self.VuAe= torsionParameters.Ae()
            self.Vuue= torsionParameters.ue()
        else: # XXX Ignore torsional deformation.
            self.VuAe= 1.0
            self.Vuue= 0.0

        concrFibers= rcSets.concrFibers.fSet
        reinfFibers= rcSets.reinfFibers.fSet
        tensionedReinforcement= rcSets.tensionFibers

        self.isBending= scc.isSubjectedToBending(0.1)
        self.tensionedRebars.number= rcSets.getNumTensionRebars()
        self.concreteArea= rcSets.getConcreteArea(1)
        if(self.concreteArea<1e-6):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= className+'.'+methodName+"concrete area too small; Ac= " + str(self.concreteArea) + "\n"
            lmsg.error(errMsg)
        else:
            self.E0= concrFibers[0].getMaterial().getInitialTangent()
            self.strutWidth= scc.getCompressedStrutWidth() # b0
            self.effectiveDepth= scc.getEffectiveDepth() # d
            self.concreteAxialForce= concrFibers.getCompressionResultant()
            self.Vu1= getVu1EHE08(self.fckH,self.fcdH,self.concreteAxialForce,self.concreteArea,self.strutWidth,self.effectiveDepth, alpha= self.alpha, theta= self.theta)
            if(self.isBending):
                self.eps1= concrFibers.getStrainMax()
                self.reinforcementElasticModulus= reinfFibers[0].getMaterial().getInitialTangent()
                self.mechanicLeverArm= scc.getMechanicLeverArm() # z
                if(self.tensionedRebars.number>0):
                    self.tensionedRebars.area= tensionedReinforcement.getArea(1)
                else:
                    self.tensionedRebars.area= 0.0
                self.thetaFisuras= getCrackAngleEHE08(Nd= Nd, Md= Md, Vd= Vd, Td= Td, z= self.mechanicLeverArm, AsPas= self.tensionedRebars.area, AsAct= 0.0, Es= self.reinforcementElasticModulus, Ep= 0.0, Fp= 0.0, Ae= self.VuAe, ue= self.Vuue)
                self.Vcu= getVcuEHE08CrackAngle(fcv= self.fckH, fcd= self.fcdH, gammaC= self.gammaC, Ncd= self.concreteAxialForce, Ac= self.concreteArea, b0= self.strutWidth, d= self.effectiveDepth, AsPas= self.tensionedRebars.area, AsAct= 0.0, theta= self.theta, thetaEVcu= self.thetaFisuras)
                self.Vsu= getVsuEHE08(self.mechanicLeverArm,self.alpha,self.theta,self.AsTrsv,self.fydS, circular)
                self.Vu2= self.Vcu+self.Vsu
            else: # Uncracked section
                # I (LCPT) don't find an expression for this case in EHE
                axes= scc.getInternalForcesAxes()
                self.strutWidth= scc.getCompressedStrutWidth() # b0
                self.thetaFisuras= math.radians(29) # compressed web so epsilon x= 0.0
                self.Vcu= getVcuEHE08CrackAngle(fcv= self.fckH, fcd= self.fcdH, gammaC= self.gammaC, Ncd= self.concreteAxialForce, Ac= self.concreteArea, b0= self.strutWidth, d= self.effectiveDepth, AsPas= self.tensionedRebars.area, AsAct= 0.0, theta= self.theta, thetaEVcu= self.thetaFisuras)
                self.mechanicLeverArm= scc.getMechanicLeverArm()
                self.Vsu= getVsuEHE08(self.mechanicLeverArm,self.alpha,self.theta,self.AsTrsv,self.fydS, circular)
                self.Vu2= self.Vcu+self.Vsu
                # self.I= scc.getFibers().getHomogenizedSectionIRelToLine(self.E0,axes)
                # self.S= scc.getFibers().getSPosHomogenizedSection(self.E0,geom.HalfPlane2d(axes))
                # self.Vu2= getVu2EHE08NoAtNoFis(self.fctdH,self.I,self.S,self.strutWidth,self.alphaL,self.concreteAxialForce,self.concreteArea)
            self.Vu= min(self.Vu1,self.Vu2)

    def calcVuEHE08(self, scc, torsionParameters, Nd, Md, Vd, Td, rcSets, circular= False):
        '''  Compute the shear strength at failure.
         XXX Presstressing contribution not implemented yet.

         :param scc: fiber model of the section.
         :param torsionParameters: parameters that define torsional behaviour 
                                   of the section as in clause 45.1 of EHE-08.
         :param Nd: Design value of axial force (positive if in tension)
         :param Md: Absolute value of design value of bending moment.
         :param Vd: Absolute value of effective design shear (clause 42.2.2).
         :param Td: design value of torsional moment.
         :param rcSets: fiber sets in the reinforced concrete section.
         :param circular: if true we reduce the efectiveness of the shear 
                          reinforcement due to the transverse inclination of
                          its elements.
        '''
        if(self.AsTrsv==0):
            self.calcVuEHE08NoAt(scc= scc, rcSets= rcSets)
        else:
            self.calcVuEHE08SiAt(scc= scc, torsionParameters= torsionParameters, Nd= Nd, Md= Md, Vd= Vd, Td= Td, rcSets= rcSets, circular= circular)

    def checkInternalForces(self, sct, torsionParameters, Nd, Md, Vd, Td):
        '''  Compute the shear strength at failure.
         XXX Presstressing contribution not implemented yet.

         :param sct: reinforced concrete section object to check shear on.
         :param torsionParameters: parameters that define torsional behaviour 
                                   of the section as in clause 45.1 of EHE-08.
         :param Nd: Design value of axial force (positive if in tension)
         :param Md: Absolute value of design value of bending moment.
         :param Vd: Absolute value of effective design shear (clause 42.2.2).
         :param Td: design value of torsional moment.
        '''
        section= sct.getProp('sectionData')
        concreteCode= section.getConcreteType()
        reinforcementCode= section.getReinfSteelType()
        shReinf= section.getShearReinfY()
        circular= section.isCircular()
        rcSets= self.extractFiberData(sct, concreteCode, reinforcementCode)
        self.AsTrsv= shReinf.getAs()
        self.alpha= shReinf.angAlphaShReinf
        
        def vu_theta(th:float):
            self.theta= th
            self.calcVuEHE08(scc= sct, torsionParameters= torsionParameters, Nd= Nd, Md= Md, Vd= Vd, Td= Td, rcSets= rcSets, circular= circular)
            return -self.Vu

        th= self.theta # math.pi/4.0
        Vu= -vu_theta(th)
        if(self.Vu<Vd):
            options= {'maxiter':10}#{'xatol':tol}
            result= optimize.minimize_scalar(vu_theta, bounds= [self.thetaMin, self.thetaMax], method= 'bounded', options= options)
            # self.iterations= result.nfev
        if(self.Vu!=0.0):
            FCtmp= Vd/self.Vu
        else:
            FCtmp= 1e99
        return FCtmp, self.Vu
            
    def checkSection(self, sct, elementDimension, torsionParameters= None):
        ''' Check shear on the section argument.

        :param sct: reinforced concrete section object to check shear on.
        :param elementDimension: dimension of the element (1, 2 or 3).
        :param torsionParameters: parameters that define torsional behaviour 
                                  of the section as in clause 45.1 of EHE-08.
        '''
        NTmp= sct.getStressResultantComponent("N")
        MyTmp= sct.getStressResultantComponent("My")
        MzTmp= sct.getStressResultantComponent("Mz")
        MTmp= math.sqrt((MyTmp)**2+(MzTmp)**2)
        VyTmp= sct.getStressResultantComponent("Vy")
        VzTmp= sct.getStressResultantComponent("Vz")
        VTmp= self.getShearForce(Vy= VyTmp, Vz= VzTmp, elementDimension= elementDimension)
        TTmp= sct.getStressResultantComponent("Mx")
        #Searching for the best theta angle (concrete strut inclination).
        #We calculate Vu for several values of theta and chose the highest Vu with its associated theta
        FCtmp, VuTmp= self.checkInternalForces(sct= sct, torsionParameters= torsionParameters, Nd= NTmp, Md= MTmp, Vd= VTmp, Td= TTmp)
        return FCtmp, VuTmp, NTmp, VyTmp, VzTmp, TTmp, MyTmp, MzTmp 

    def check(self, elements, combName):
        ''' For each element in the set 'elements' passed as first parameter
        and the resulting internal forces for the load combination 'combName'  
        passed as second parameter, this method calculates all the variables 
        involved in the shear-ULS checking and obtains the capacity factor.
        In the case that the calculated capacity factor is smaller than the 
        smallest obtained for the element in previous load combinations, this 
        value is saved in the element results record.

        Elements processed are those belonging to the phantom model, that is to 
        say, of type xc.ZeroLengthSection. As we have defined the variable 
        fakeSection as False, a reinfoced concrete fiber section is generated
        for each of these elements. 

        XXX Rebar orientation not taken into account yet.

        :param elements: phantom model elements.
        :param combName: name of the load combination being checked.
        '''
        if(self.verbose):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.log(className+'.'+methodName+"; postprocessing combination: "+combName)
        torsionParameters= None # XXX Ignore torsional deformation.
        for e in elements:
            # Call getResisting force to update the internal forces.
            R= e.getResistingForce()
            if(__debug__):
                if(not R):
                    AssertionError('Can\'t append the block.')                    
            scc= e.getSection()
            idSection= e.getProp("idSection")
            masterElementDimension= e.getProp("masterElementDimension")
            FCtmp, VuTmp, NTmp, VyTmp, VzTmp, TTmp, MyTmp, MzTmp= self.checkSection(sct= scc, elementDimension= masterElementDimension, torsionParameters= torsionParameters)
            Mu= 0.0 #Apparently EHE doesn't use Mu
            if(FCtmp>=e.getProp(self.limitStateLabel).CF): # new worst case.
                e.setProp(self.limitStateLabel, self.ControlVars(idSection= idSection, combName= combName, CF= FCtmp, N= NTmp, My= MyTmp, Mz= MzTmp, Mu= Mu, Vy= VyTmp, Vz= VzTmp, theta= self.theta, Vcu= self.Vcu, Vsu= self.Vsu, Vu= VuTmp)) # set worst case

class ShearResistanceLimitStateData(lsd.ShearResistanceRCLimitStateData):
    ''' Reinforced concrete normal stresses data for limit state checking.'''
    
    def getController(self, solutionProcedureType= None):
        ''' Return a controller corresponding to this limit state.

        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results
                                      (if None, use the default one).
        '''
        retval= None
        if(solutionProcedureType):
            retval= ShearController(limitStateLabel= self.label, solutionProcedureType= solutionProcedureType)
        else:
            retval= ShearController(limitStateLabel= self.label)
        return retval;
    
shearResistance= ShearResistanceLimitStateData()

class TorsionController(lscb.ShearControllerBase):
    '''Torsion strength control according to EHE-08.'''

     # ControlVars= cv.RCTorsionControlVars
    
    def __init__(self, limitStateLabel, solutionProcedureType= lscb.defaultStaticNonLinearSolutionProcedure):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(TorsionController,self).__init__(limitStateLabel= limitStateLabel, fakeSection= False, solutionProcedureType= solutionProcedureType)
        self.thetaMin= math.atan(0.5) # Minimal value of the theta angle.
        self.thetaMax= math.atan(2) # Maximal value of the theta angle.

        self.Tu1= 0.0 # Maximum torsional moment which the concrete’s compressed struts can resist.
        self.Tu2= 0.0 # Maximum torsional moment which transverse reinforcements can resist.
        self.Tu3= 0.0 # Maximum torsional moment which longitudinal reinforcements can resist.

    def calcTu1(self, rcSection, Ncd, Ae, he, alpha= 0.6):
        ''' Return the maximum torsional moment which the concrete’s 
            compressed struts can resist according to clause 45.2.2.1 of EHE-08.

        :param rcSection: reinforced concrete section.
        :param Ncd: design value of axial force in concrete only (positive if 
                    in tension).
        :param Ae: Area enclosed by the middle line of the design effective 
                   hollow section (figure 45.2.1).
        :param he: effective thickness of the wall of the design section.
        :param alpha: 0.60 if there are stirrups only along the external 
                      circumference of the member; 0.75 if closed stirrups are 
                      installed on both faces of the wall of the equivalent
                      hollow section or the actual hollow section.
        '''
        concrete= rcSection.getConcreteType()
        fck= concrete.fck
        fcd= concrete.fcd()
        K= 1
        if(Ncd<0): # compressed concrete.
            sgpcd= Ncd/Ac
            K= getKEHE08(sgpcd,fcd)
        f1cd= getF1cdEHE08(fck,fcd)
        cotgtheta= 1.0/math.tan(rcSection.torsionReinf.angThetaConcrStruts)
        retval= cotgtheta/(1+cotgtheta**2)
        retval*= 2*K*alpha*f1cd*Ae*he
        return -retval
        
    def calcTu2(self, rcSection, Ae:float):
        ''' Compute the torsional stress which transverse reinforcements can 
            resist according to clause 45.2.2.2 of EHE-08.

        :param rcSection: reinforced concrete section.
        :param Ae: Area enclosed by the middle line of the design effective 
                   hollow section (figure 45.2.1).
        '''
        # Do not multiply by 2 because getAs() already returns the double
        # of At in the formula.
        steel= rcSection.getReinfSteelType()
        fytd= min(steel.fyd(), 400e6)
        theta= rcSection.torsionReinf.angThetaConcrStruts
        return Ae*rcSection.torsionReinf.getAs()/rcSection.torsionReinf.shReinfSpacing*fytd/math.tan(theta)
    
    def calcTu3(self, rcSection, Ae:float, ue:float):
        ''' Compute the torsional stress which longitudinal reinforcements can 
            resist according to clause 45.2.2.3 of EHE-08.

        :param rcSection: reinforced concrete section.
        :param Ae: Area enclosed by the middle line of the design effective 
                   hollow section (figure 45.2.1).
        :param ue: perimeter of the middle line in the design effective 
                   hollow section Ae.
        '''
        steel= rcSection.getReinfSteelType()
        fy1d= min(steel.fyd(), 400e6)
        theta= rcSection.torsionReinf.angThetaConcrStruts
        return 2*Ae*rcSection.torsionReinf.A1/ue*fy1d*math.tan(theta)
        
    def calcTu(self, rcSection, Ncd, Ae, he, ue:float, alpha= 0.6):
        ''' Compute the torsional strength of the given section.
            according to clause 45.2.2 of EHE-08.

        :param rcSection: reinforced concrete section.
        :param Ncd: design value of axial force in concrete only (positive if 
                    in tension).
        :param Ae: Area enclosed by the middle line of the design effective 
                   hollow section (figure 45.2.1).
        :param he: effective thickness of the wall of the design section.
        :param ue: perimeter of the middle line in the design effective 
                   hollow section Ae.
        :param alpha: 0.60 if there are stirrups only along the external 
                      circumference of the member; 0.75 if closed stirrups are 
                      installed on both faces of the wall of the equivalent
                      hollow section or the actual hollow section.
        '''
        Tu1= self.calcTu1(rcSection= rcSection, Ncd= Ncd, Ae= Ae, he= he, alpha= alpha)
        Tu2= self.calcTu2(rcSection= rcSection, Ae= Ae)
        Tu3= self.calcTu3(rcSection= rcSection, Ae= Ae, ue= ue)
        return min(Tu1, Tu2, Tu3)

class TorsionResistanceLimitStateData(lsd.TorsionResistanceRCLimitStateData):
    ''' Reinforced concrete torsion strength limit state data.'''

    def getController(self):
        ''' Return a controller corresponding to this limit state.
        '''
        return TorsionController(self.label)
        
torsionResistance= TorsionResistanceLimitStateData()

class CrackController(lscb.LimitStateControllerBase):
    '''Object that verifies the cracking serviceability limit state according 
    to clause 49.2.4 of EHE-08.

    :ivar wk_lim: maximum allowable crack width. 
    :ivar beta: Coefficient which relates the mean crack opening to the 
                characteristic value and is equal to 1.3 in the case 
                of cracking caused by indirect actions only, and 1.7 
                in other cases.
    :ivar k2: coefficient of value 1.0 in the case of non-repeating temporary
              load and 0.5 in other cases.
    '''
    ControlVars= cv.RCCrackControlVars
    
    def __init__(self, limitStateLabel, wk_lim= 0.3e-3, beta= 1.7, k2= 1.0, solutionProcedureType= lscb.defaultStaticNonLinearSolutionProcedure):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param wk_lim: maximum allowable crack width. 
        :param beta: Coefficient which relates the mean crack opening to the 
                     characteristic value and is equal to 1.3 in the case 
                     of cracking caused by indirect actions only, and 1.7 
                     in other cases.
        :param k2: coefficient of value 1.0 in the case of non-repeating 
                   temporary load and 0.5 in other cases.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(CrackController,self).__init__(limitStateLabel= limitStateLabel, fakeSection= False, solutionProcedureType= solutionProcedureType)
        self.wk_lim= wk_lim
        self.beta= beta # if only indirect actions beta must be =1.3
        self.k2= k2

    def EHE_hceff(self,width,h,x):
        '''Return the maximum height to be considered in the calculation of 
           the concrete effective area in tension.

        :param width: section width.
        :param h: lever arm.
        :param x: depth of the neutral fiber.
        '''
        if width>2*h:   #slab, flat beam
            hceff=min(abs(h+x),h/4.)
        else:
            hceff=min(abs(h+x),h/2.)
        return hceff

    def EHE_k1(self, eps1, eps2):
        '''Return the coefficient k1 involved in the calculation of the mean 
        crack distance according to EHE. This coefficient represents the 
        effect of the tension diagram in the section.

        :param eps1: maximum deformation calculated in the section at the 
                     limits of the tension zone.
        :param eps2: minimum deformation calculated in the section at the 
                     limits of the tension zone.
        '''
        k1= (eps1+eps2)/(8.0*eps1)
        return k1

    def check(self,elements,loadCombinationName):
        ''' For each element in the set 'elememts' passed as first parameter 
            and the resulting internal forces for the load combination 
            'loadCombinationName' passed as second parameter, this method calculates 
            all the variables involved in the crack-SLS checking and obtains 
            the crack width. In the case that the calculated crack width is 
            greater than the biggest obtained for the element in previous
            load combinations, this value is saved in the element results 
            record. 

            Elements processed are those belonging to the phantom model, that 
            is to say, of type xc.ZeroLengthSection. As we have defined the 
            variable fakeSection as False, a reinfoced concrete fiber section
            is generated for each of these elements.

        :param elements: elements to check.
        :param loadCombinationName: name of the load combination.
        '''
        if(self.verbose):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.log(className+'.'+methodName+"; postprocessing combination: "+loadCombinationName)
        concreteMaxTensileStrain= 0.00015 # Used only as a threshold for very small tension strains.
        for e in elements:
            Aceff=0  #init. value
            R= e.getResistingForce()
            sct=e.getSection()
            sctCrkProp= lscb.FibSectLSProperties(sct)
            sctCrkProp.setupStrghCrackDist()
            if(sctCrkProp.eps1<=0): # No tensile strains.
                s_rmax= 0.0
                sigma_s= sctCrkProp.getMaxReinforcementTensileStress()
                wk= 0.0
            elif(sctCrkProp.eps1<concreteMaxTensileStrain):  # Very small tensile strain => no cracking
                s_rmax= 0.0
                sigma_s= sctCrkProp.getMaxReinforcementTensileStress()
                wk= 0.0
            else:
                hceff= self.EHE_hceff(sct.getAnchoMecanico(),sctCrkProp.h,sctCrkProp.x)
                Aceff=sct.getNetEffectiveConcreteArea(hceff,"tensSetFb",15.0)
                if(Aceff<=0):
                    s_rmax= 0.0
                    sigma_s= sctCrkProp.getMaxReinforcementTensileStress()
                    wk= 0.0
                else:
                    ro_s_eff=sctCrkProp.As/Aceff # effective ratio of reinforcement
                    k1= self.EHE_k1(sctCrkProp.eps1,sctCrkProp.eps2)
                    # median crack spacing.
                    sm= 2*sctCrkProp.cover+0.2*sctCrkProp.spacing+0.4*k1*sctCrkProp.fiEqu/ro_s_eff
                    sigma_sr= sctCrkProp.getAverageSigmaSR()
                    # Service stress of the passive reinforcement in the cracked section hypothesis.
                    sigma_s, eps_s= sctCrkProp.getAverageReinforcementTensileStrainAndStress()
                    factor= max(1.0-self.k2*(sigma_sr/sigma_s)**2,0.4)
                    eps_sm= factor*eps_s
                    s_rmax= self.beta*sm
                    wk= s_rmax*eps_sm
            if(wk>=e.getProp(self.limitStateLabel).wk):
                CF= wk/self.wk_lim # Capacity factor.
                sigma_c= sctCrkProp.sigma_c
                e.setProp(self.limitStateLabel, self.ControlVars(idSection=e.getProp('idSection'), combName=loadCombinationName, N=-R[0], My=-R[4], Mz=-R[5], s_rmax= s_rmax, sigma_s= sigma_s, sigma_c= sigma_c, wk= wk, CF= CF))

class CrackStraightController(CrackController):
    '''Object that verifies the cracking serviceability limit state according 
    to EHE-08 when considering a concrete stress-strain diagram that takes 
    account of the effects of tension  stiffening.
    '''

    ControlVars= cv.RCCrackStraightControlVars
    
    def __init__(self, limitStateLabel, beta= 1.7, solutionProcedureType= lscb.defaultStaticNonLinearSolutionProcedure):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param beta: Coefficient which relates the mean crack opening to the 
                     characteristic value and is equal to 1.3 in the case 
                     of cracking caused by indirect actions only, and 1.7 
                     in other cases.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(CrackStraightController,self).__init__(limitStateLabel= limitStateLabel, fakeSection= False, beta= beta, solutionProcedureType= solutionProcedureType)

    def expectsTensionStiffeningModel(self):
        ''' Return true if a tension-stiffening concrete fiber model must be
            used for this limit state.'''
        return True
    
    def check(self, elements, combName):
        ''' For each element in the set 'elememts' passed as first parameter 
            and the resulting internal forces for the load combination 
            'combName' passed as second parameter, this method calculates 
            all the variables involved in the crack-SLS checking and obtains 
            the crack width. In the case that the calculated crack width is 
            greater than the biggest obtained for the element in previous
            load combinations, this value is saved in the element results 
            record. 

            Elements processed are those belonging to the phantom model, that 
            is to say, of type xc.ZeroLengthSection. As we have defined the 
            variable fakeSection as False, a reinfoced concrete fiber section
            is generated for each of these elements. 

        :param elements: elements to check.
        :param loadCombinationName: name of the load combination.
        '''
        if(self.verbose):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.log(className+'.'+methodName+"; postprocessing combination: "+combName)
        for e in elements:
            Aceff=0  #init. value
            R=e.getResistingForce()
            sct=e.getSection()
            sctCrkProp= lscb.FibSectLSProperties(sct)
            sctCrkProp.setupStrghCrackDist()
            hceff=self.EHE_hceff(sct.getAnchoMecanico(),sctCrkProp.h,sctCrkProp.x)
            #Acgross=sct.getGrossEffectiveConcreteArea(hceff)
            Aceff=sct.getNetEffectiveConcreteArea(hceff,"tensSetFb",15.0)
            rfSteel= sctCrkProp.sctProp.getReinfSteelType()
            concrete= sctCrkProp.sctProp.getConcreteType()
            k1=self.EHE_k1(sctCrkProp.eps1,sctCrkProp.eps2)
            if Aceff<=0:
               s_rmax=0
            else:
                ro_s_eff=sctCrkProp.As/Aceff      #effective ratio of reinforcement
                sm=2*sctCrkProp.cover+0.2*sctCrkProp.spacing+0.4*k1*sctCrkProp.fiEqu/ro_s_eff
                s_rmax=self.beta*sm
                #Parameters for tension stiffening of concrete
                paramTS= concrete_base.paramTensStiffness(concrMat=concrete,reinfMat=rfSteel,reinfRatio=ro_s_eff,diagType='K')
                concrete.tensionStiffparam=paramTS #parameters for tension stiffening
                                                 #are assigned to concrete
                ftdiag=concrete.tensionStiffparam.pointOnsetCracking()['ft']      #stress at the adopted point for concrete onset cracking
                Etsdiag=abs(concrete.tensionStiffparam.regresLine()['slope'])
                fiber_sets.redefTensStiffConcr(setOfTenStffConcrFibSect=sctCrkProp.setsRC.concrFibers,ft=ftdiag,Ets=Etsdiag)
            e.setProp('ResF',R)   #vector resisting force
            e.setProp('s_rmax',s_rmax)  #maximum crack distance
        self.preprocessor.getDomain.revertToStart()
        self.solutionProcedure.solveComb(combName)
        for e in elements:
            sct=e.getSection()
            rfset=sct.getFiberSets()["reinfSetFb"]
            eps_sm=rfset.getStrainMax()
            srmax=e.getProp("s_rmax")
            wk=srmax*eps_sm
            if (wk>e.getProp(self.limitStateLabel).wk):
                R=e.getProp('ResF')
                e.setProp(self.limitStateLabel, self.ControlVars(idSection=e.getProp("idSection"),combName=combName,N=-R[0],My=-R[4],Mz=-R[5],s_rmax=srmax,eps_sm=eps_sm,wk=wk))
      
class CrackControl(lscb.CrackControlBaseParameters):
    '''
    Define the properties that will be needed for crack control checking
    as in clause 49.2.4 of EHE-08.'''

    def __init__(self, limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''        
        super(CrackControl,self).__init__(limitStateLabel)
        self.concreteArea= 0.0 # Concrete section area.
        self.fctmFis= 0.0 # Average tensile strength of the concrete.
        self.tensionedRebars= lscb.TensionedRebarsProperties()
        self.eps1= 0.0 # Maximum strain in concrete.
        self.eps2= 0.0 # Minimum strain in concrete.
        self.k1= 0.0 # Coefficient representing the effect of the tension diagram in the section.
        self.k2= 0.5 # Coefficient of value 1.0 in the case of non-repeating
                     # temporary load and 0.5 otherwise.
        self.depthMecanico= 0.0 # mechanic section depth.
        self.widthMecanico= 0.0 # mechanic section width.
        self.aspectRatio= 0.0 #aspect ratio width/depth.
        self.hEfMax= 0.0 # maximum depth of effective area.
        self.grossEffectiveArea= 0.0 # gross effective area.
        self.netEffectiveArea= 0.0 # net effective area.
        self.E0= 0.0 # Concrete tangent stiffness.
        self.beta= 1.7 # Coefficient which relates the mean crack opening to the
                       # characteristic value and is equal to 1.3 in the case of
                       # cracking caused by indirect actions only, and 1.7 otherwise.
        self.Wk= 0.0 # Characteristic crack opening.

    def printParams(self, os= sys.stdout):
        ''' Prints crack control parameters.'''
        self.tensionedRebars.printParams()    
        os.write("Gross effective area: "+str(self.grossEffectiveArea*1e4)+" cm2")
        os.write("Net effective area: "+str(self.netEffectiveArea*1e4)+" cm2")
        os.write("Maximum concrete strain in the cracked section inside the tensioned zone; eps1= "+str(self.eps1*1e3)+" per mil.")
        os.write("Minimum concrete strain in the cracked section inside the tensioned zone; eps2= "+str(self.eps2*1e3)+" per mil.")
        os.write("Effect of the tension diagram; k1= "+str(self.k1)+"")
        os.write("Mechanic depth; h= "+str(self.depthMecanico)+" m")
        os.write("Lever arm; z= "+str(self.mechanicLeverArm)+" m")
        os.write("Mechanical width; b= "+str(self.widthMecanico)+" m")
        os.write("Aspect ratio; r= "+str(self.aspectRatio)+"")
        os.write("Maximum depth for effective area; hEfMax= "+str(self.hEfMax)+" m")
        os.write("Average tensile strength of the concrete; fctm= "+str(self.fctmFis/1e6)+" MPa")
        os.write("Concrete tangent stiffness; E0= "+str(self.E0/1e9)+" GPa")
        os.write("Characteristic crack opening; Wk= "+str(self.Wk*1e3)+" mm")

    def computeWkOnBars(self, tensionedReinforcement):
        '''Compute the characteristic crack opening on each bar and return
           the maximum.

        :param tensionedReinforcement: 
        '''
        retval= 0.0
        for i, rebar in enumerate(tensionedReinforcement):
            rebarArea= rebar.getArea()
            rebarStress= rebar.getMaterial().getStress()
            rebarCover= tensionedReinforcement.getFiberCover(i)
            rebarDiameter= rebar.getEquivalentDiameter()
            rebarSigmaSR= tensionedReinforcement.getSigmaSRAtFiber(i,self.E0,self.tensionedRebars.E, self.fctmFis)
            self.tensionedRebars.averageStress+= rebarArea*rebarSigmaSR

            rebarEffConcArea= tensionedReinforcement.getFiberEffectiveConcreteArea(i)
            rebarSpacing= min(tensionedReinforcement.getFiberSpacing(i),15*rebarDiameter)
            rebarCrackMeanSep= 2*rebarCover+0.2*rebarSpacing+0.4*self.k1*rebarDiameter*rebarEffConcArea/rebarArea
            maxBarElongation= rebarStress/self.tensionedRebars.E
            averageBarElongation= max(1.0-self.k2*(rebarSigmaSR/rebarStress)**2,0.4)*maxBarElongation
            rebarWk=  self.beta*rebarCrackMeanSep*averageBarElongation
            retval= max(retval,rebarWk)
        self.tensionedRebars.averageStress/= self.tensionedRebars.area
        return retval

    def computeWk(self, scc, concreteMatTag, reinfSteelMaterialTag, fctm):
        '''Computes the characteristic value of the crack width.

         :param scc: section.
         :param reinfSteelMaterialTag: identifier of the concrete material.
         :param reinfSteelMaterialTag: identifier of the reinforcing 
                                       steel material.
         :param fctm: average tensile strength of the concrete.
        '''
        if(self.rcSets is None):
            self.rcSets= fiber_sets.fiberSectionSetupRC3Sets(scc,concreteMatTag,self.concreteFibersSetName,reinfSteelMaterialTag,self.rebarFibersSetName)
        concrFibers= self.rcSets.concrFibers.fSet
        tensionedReinforcement= self.rcSets.tensionFibers

        self.fctmFis= fctm
        self.claseEsfuerzo= scc.getStrClaseEsfuerzo(0.0)
        self.tensionedRebars.number= self.rcSets.getNumTensionRebars()
        self.Wk= 0.0
        if(self.tensionedRebars.number>0):
            scc.computeCovers(self.tensionedRebarsFiberSetName)
            scc.computeSpacement(self.tensionedRebarsFiberSetName)
            self.eps1= concrFibers.getStrainMax()
            self.eps2= max(concrFibers.getStrainMin(),0.0)
            self.k1= (self.eps1+self.eps2)/8/self.eps1
            self.E0= concrFibers[0].getMaterial().getInitialTangent()
            self.concreteArea= concrFibers.getArea(1.0)
            self.depthMecanico= scc.getLeverArm()
            self.mechanicLeverArm= scc.getMechanicLeverArm() # z
            self.widthMecanico= scc.getAnchoMecanico()
            self.aspectRatio= self.widthMecanico/self.depthMecanico
            if(self.aspectRatio>1):
                self.hEfMax= self.depthMecanico/4.0 # Pieza tipo losa
            else:
                self.hEfMax= self.depthMecanico/2.0
            self.netEffectiveArea= scc.computeFibersEffectiveConcreteArea(self.hEfMax,self.tensionedRebarsFiberSetName,15)

            self.tensionedRebars.setup(tensionedReinforcement)
            self.Wk= self.computeWkOnBars(tensionedReinforcement)
            
class CrackControlLimitStateData(lsd.CrackControlRCLimitStateData):
    ''' EHE crack control limit state data.'''
        
    def getController(self, wk_lim= 0.3e-3, beta= 1.7, k2= 1.0, solutionProcedureType= None):
        ''' Return a controller corresponding to this limit state.
        :param wk_lim: maximum allowable crack width. 
        :param beta: Coefficient which relates the mean crack opening to the 
                     characteristic value and is equal to 1.3 in the case 
                     of cracking caused by indirect actions only, and 1.7 
                     in other cases.
        :param k2: coefficient of value 1.0 in the case of non-repeating 
                   temporary load and 0.5 in other cases.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results
                                      (if None, use the default one).
        '''
        retval= None
        if(solutionProcedureType):
            retval= CrackController(limitStateLabel= self.label, wk_lim= wk_lim, beta= beta, k2= k2, solutionProcedureType= solutionProcedureType)
        else:
            retval= CrackController(limitStateLabel= self.label, wk_lim= wk_lim, beta= beta, k2= k2)
        return retval
        
class RareLoadsCrackControlLimitStateData(CrackControlLimitStateData):
    ''' Reinforced concrete crack control under rare loads limit state data.'''
    def __init__(self, designSituations= ['sls_rare']):
        '''Limit state data constructor 

        :param designSituations: design situations that will be checked; 
                                 i. e. sls_quasi-permanent, sls_frequent, 
                                 sls_rare, sls_earthquake, etc. 
        '''
        super(RareLoadsCrackControlLimitStateData,self).__init__(limitStateLabel= 'SLS_rareLoadsCrackControl', outputDataBaseFileName= fn.crackControlRareVerificationResultsFile, designSituations= designSituations)
        
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifCrackRareFile())

class FreqLoadsCrackControlLimitStateData(CrackControlLimitStateData):
    ''' Reinforced concrete crack control under frequent loads limit state data.'''
    def __init__(self, designSituations= ['sls_frequent']):
        '''Constructor. 

        :param designSituations: design situations that will be checked; 
                                 i. e. sls_quasi-permanent, sls_frequent, 
                                 sls_rare, sls_earthquake, etc. 
        '''
        super(FreqLoadsCrackControlLimitStateData,self).__init__(limitStateLabel= 'SLS_frequentLoadsCrackControl', outputDataBaseFileName= fn.crackControlFreqVerificationResultsFile, designSituations= designSituations)
        
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifCrackFreqFile())
            
class QPLoadsCrackControlLimitStateData(CrackControlLimitStateData):
    ''' Reinforced concrete crack control under quasi-permanent loads limit state data.'''
    def __init__(self, designSituations= ['sls_quasi-permanent']):
        '''Constructor. 

        :param designSituations: design situations that will be checked; 
                                 i. e. sls_quasi-permanent, sls_frequent, 
                                 sls_rare, sls_earthquake, etc. 
        '''
        super(QPLoadsCrackControlLimitStateData,self).__init__(limitStateLabel= 'SLS_quasiPermanentLoadsCrackControl', outputDataBaseFileName= fn.crackControlQpermVerificationResultsFile, designSituations= designSituations)
        
    def readControlVars(self, modelSpace):
        ''' Read the control vars associated with this limit state.

        :param modelSpace: PredefinedSpace object used to create the FE model
                           (see predefined_spaces.py).
        '''
        modelSpace.readControlVars(inputFileName= self.envConfig.projectDirTree.getVerifCrackQpermFile())

rareLoadsCrackControl= RareLoadsCrackControlLimitStateData()
freqLoadsCrackControl= FreqLoadsCrackControlLimitStateData()
quasiPermanentLoadsCrackControl= QPLoadsCrackControlLimitStateData()

class TorsionParameters(object):
    '''Methods for checking reinforced concrete section under torsion 
       according to clause 45.1 of EHE-08.

    :ivar h0: Actual thickness of the section wall in the case of hollow sections.
    :ivar c: Covering of longitudinal reinforcements.

    :ivar crossSectionContour: Cross section contour.
    :ivar midLine: Polygon defined by the midline of the effective hollow section.
    :ivar intLine: Polygon defined by the interior contour of the effective hollow section.
    :ivar effectiveHollowSection: Effective hollow section contour
    '''
    def __init__(self):
        self.h0= 0.0  # Actual thickness of the sectin wall in the case of hollow sections.
        self.c= 0.0  # Covering of longitudinal reinforcements.

        self.crossSectionContour= geom.Polygon2d()  # Cross section contour.
        self.midLine=  geom.Polygon2d() # Polygon defined by the midline of the effective hollow section.
        self.intLine=  geom.Polygon2d() # Polygon defined by the interior contour of the effective hollow section.
        self.effectiveHollowSection= geom.PolygonWithHoles2d() # Effective hollow section contour
        
    def A(self):
        '''Return the area of the transverse section inscribed in the 
           external circumference including inner void areas
        '''
        return self.crossSectionContour.getArea()
    
    def u(self):
        '''Return the external perimeter of the transverse section.
        '''
        return self.crossSectionContour.getPerimeter()
    
    def he(self):
        '''Return the effective thickness of the wall of the design section.
        '''
        return max(2*self.c,min(self.A()/self.u(),self.h0))
    
    def Ae(self):
        '''Return the area enclosed by the middle line of the design 
           effective hollow section (see figure 45.2.1 of EHE-08).
        '''
        return self.midLine.getArea()
    
    def ue(self):
        '''Return the perimeter of the middle line in the design effective 
           hollow section Ae.
        '''
        return self.midLine.getPerimeter()

def computeEffectiveHollowSectionParameters(sectionGeometry, h0, c):
    '''Computes the parameters for torsion analysis of an
     effective hollow section according to clause 45.2.1
     of EHE-08. Not valid if for non-convex sections.

    :param sectionGeometry: section geometry.
    :param h0: actual thickness of the wall for hollow sections.
    :param c: cover of longitudinal reinforcement.
    '''
    retval= TorsionParameters()
    retval.h0= h0
    retval.c= c
    retval.crossSectionContour= sectionGeometry.getRegionsContour()
    he= retval.he() # effective thickness.
    retval.midLine= retval.crossSectionContour.offset(-he/2) # mid-line
    retval.intLine= retval.crossSectionContour.offset(-he) # internal line
    retval.effectiveHollowSection.contour(retval.crossSectionContour)
    retval.effectiveHollowSection.addHole(retval.intLine)
    return retval

def computeEffectiveHollowSectionParametersRCSection(rcSection):
    '''Computes the parameters for torsion analysis of an
     effective hollow section according to clause 45.2.1
     of EHE-08.

    :param rcSection: reinforced concrete section
    '''
    h0= rcSection.getTorsionalThickness()
    return computeEffectiveHollowSectionParameters(rcSection.geomSection,h0,rcSection.minCover)

class ReinforcementRatios(object):
    ''' Maximum and minimum reinforcement ratios according to EHE-08.

    :ivar concrete: concrete material.
    :ivar reinforcingSteel: reinforcing steel material.
    :ivar Ac: concrete area.
    :ivar Ap: area of the bonded active reinforcement.
    :ivar P: pre-stressing force with instantaneous losses disregarded.
    :ivar prestressingSteel: pre-stressing steel material.
    '''
    def __init__(self, concrete, reinforcingSteel, Ac , Ap= 0.0, P= 0, prestressingSteel= None):
        '''Constructor.

        :param concrete: concrete material.
        :param reinforcingSteel: reinforcing steel material.
        :param Ac: gross concrete section area.
        :param Ap: area of the bonded active reinforcement.
        :param P: pre-stressing force with instantaneous losses disregarded.
        :param prestressingSteel: pre-stressing steel material.
        '''
        self.concrete= concrete
        self.reinforcingSteel= reinforcingSteel
        self.Ac= Ac
        self.Ap= Ap
        self.P= P
        self.prestressingSteel= prestressingSteel
    
class BeamReinforcementRatios(ReinforcementRatios):
    ''' Maximum and minimum reinforcement ratios for members subjected to
        pure or combined bending.

    :ivar fct_m_fl: average flexural strength of the concrete.
    :ivar W1: section modulus of the gross section relating to the fibre under greatest tension.
    :ivar z: Mechanical lever arm of the section. In the absence of more accurate calculations, this may be taken to be z = 0.8 h.
    :ivar fpd: design value of the tensile strength of bonded active reinforcement steel.
    '''
    def __init__(self, concrete, reinforcingSteel, Ac, W1, h, z= None, Ap= 0.0, P= 0.0, dp=0.0, ds=0.0, e= 0.0, prestressingSteel= None):
        '''Constructor.

        :param concrete: concrete material.
        :param reinforcingSteel: reinforcing steel material.
        :param Ac: concrete area.
        :param W1: section modulus of the gross section relating to the fibre under greatest tension.
        :param h: section depth.
        :param z: mechanical lever arm of the section. In the absence of more accurate calculations, this may be taken to be z = 0.8 h.
        :param Ap: area of the bonded active reinforcement.
        :param dp: depth of the active reinforcement from the most compressed fibre in the section.
        :param ds: depth of the passive reinforcement from the most compressed fibre in the section.
        :param e: eccentricity of the pre-stressing relative to the centre of gravity of the gross section.
        '''
        super().__init__(concrete= concrete, reinforcingSteel= reinforcingSteel, Ac= Ac, Ap= Ap, P= P, prestressingSteel= prestressingSteel)
        self.W1= W1
        self.h= h
        if(z):
            self.z= z
        else:
            self.z= 0.8*h
        self.dp= dp
        self.ds= ds
        self.e= e
    
    def getMinimumGeometricAmount(self):
        ''' Minimum geometric reinforcement amount for columns
            according to table 42.3.5 of EHE-08.'''
        fyd= self.reinforcingSteel.fyd()
        retval= self.Ac
        if(fyd>400e6):
            retval*= 0.0028
        else:
            retval*= 0.0033
        return retval
    
    def getMinimumMechanicalAmount(self):
        ''' Minimum mechanical reinforcement amount according to clause 
            42.3.2 of EHE-08.'''
        retval= (self.W1/self.z*self.concrete.getFlexuralTensileStrength(self.h))
        if(self.Ap!=0.0): # prestressing
            retval+= self.P/self.z*(self.W1/self.Ac+self.e)
            frac= 1.0
            if(self.ds!=0.0): # Passive and active reinforcement.
                frac= self.dp/self.ds
            retval-= self.Ap*self.prestressingSteel.fpd()*frac
        retval/= self.reinforcingSteel.fyd()
        return max(retval,0.0)

class ColumnReinforcementRatios(ReinforcementRatios):
    ''' Minimum and maximum reinforcement ratios for columns.

    '''
    def __init__(self, concrete, reinforcingSteel, Ac, Ap= 0.0, P= 0):
        '''Constructor.

        :param concrete: concrete material.
        :param reinforcingSteel: reinforcing steel material.
        :param Ac: concrete area.
        :param Ap: area of the bonded active reinforcement.
        :param P: pre-stressing force with instantaneous losses disregarded.
        '''
        super().__init__(concrete= concrete, reinforcingSteel= reinforcingSteel, Ac= Ac, Ap= Ap, P= P)
        
    def getMinimumGeometricAmount(self):
        ''' Minimum geometric reinforcement amount for columns
            according to table 42.3.5 of EHE-08.'''
        return 0.004*self.Ac 

    def getMinimumMechanicalAmount(self, Nd):
        ''' Minimum mechanical reinforcement amount according to
            clause 42.3.3 of EHE-08.

        :param Nd: Design value of the internal axial load. 
        '''
        if(Nd>0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            msg= className+'.'+methodName+'; column member in tension.'
            lmsg.warning(msg)
            retval= self.P+self.Ac*self.concrete.fctm()
            if(self.prestressingSteel):
                retval-= self.Ap*self.prestressingSteel.fpd()
            retval/= self.reinforcingSteel.fyd()
        else:
            fycd= min(self.reinforcingSteel.fyd(),400e6)
            retval= -0.1*Nd/fycd
        return retval

    def getMaximumReinforcementAmount(self):
        '''Return the maximal reinforcement amount acording to clause
           42.3.3 of EHE-08.'''
        fycd= min(self.reinforcingSteel.fyd(),400e6)
        return (-self.concrete.fcd()*self.Ac)/fycd

    def check(self, As, Nd):
        '''Checking of main reinforcement ratio in compression.'''
        ctMinGeom= self.getMinumumGeometricAmount()
        if(As<ctMinGeom):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            msg= className+'.'+methodName+"; reinforcement area: "+str(As*1e4)+\
                 " cm, gives a ratio below minimal geometric reinforcement amount: "+\
                 str(ctMinGeom*1e4)+" cm2\n"
            lmsg.warning(msg)
        ctMinMec= self.getMinimumMechanicalAmount(Nd)
        if(As<ctMinMec):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            msg= className+'.'+methodName+"; reinforcement area: "+str(As*1e4)+" cm, gives a ratio below minimal mechanical reinforcement amount: "+str(ctMinMec*1e4)+" cm2\n"
            lmsg.warning(msg)
        ctMax= self.getMaximumReinforcementAmount()
        if(As>ctMax):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            msg= className+'.'+methodName+"; reinforcement area: "+str(As*1e4)+" cm, gives a ratio above the maximum mechanical reinforcement ratio: "+str(ctMax*1e4)+" cm2\n"
            lmsg.warning(msg)
            
class TieReinforcementRatios(ReinforcementRatios):
    ''' Minimum and maximum reinforcement ratios for members that resist
        tension forces.

    '''
    def __init__(self, concrete, reinforcingSteel, Ac, Ap= 0.0, P= 0):
        '''Constructor.

        :param concrete: concrete material.
        :param reinforcingSteel: reinforcing steel material.
        :param Ac: concrete area.
        :param Ap: area of the bonded active reinforcement.
        :param P: pre-stressing force with instantaneous losses disregarded.
        '''
        super().__init__(concrete= concrete, reinforcingSteel= reinforcingSteel, Ac= Ac, Ap= Ap, P= P)
        
    def getMinimumGeometricAmount(self):
        ''' Minimum geometric reinforcement amount for columns
            according to table 42.3.5 of EHE-08 (considered as a
            rib).'''
        fyd= self.reinforcingSteel.fyd()
        retval= self.Ac
        if(fyd>400e6):
            retval*= 0.003
        else:
            retval*= 0.004
        return retval

    def getMinimumMechanicalAmount(self, Nd):
        ''' Minimum mechanical reinforcement amount according to
            clause 42.3.3 of EHE-08.

        :param Nd: Design value of the internal axial load. 
        '''
        if(Nd>0):
            retval= self.P+self.Ac*self.concrete.fctm()
            if(self.prestressingSteel):
                retval-= self.Ap*self.prestressingSteel.fpd()
            retval/= self.reinforcingSteel.fyd()
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            msg= className+'.'+methodName+'; tie member in compression.'
            lmsg.warning(msg)
            fycd= min(self.reinforcingSteel.fyd(),400e6)
            retval= -0.1*Nd/fycd
        return retval

    def check(self, As, Nd):
        '''Checking of main reinforcement ratio in compression.'''
        ctMinGeom= self.getMinumumGeometricAmount()
        if(As<ctMinGeom):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            msg= className+'.'+methodName+"; reinforcement area: "+str(As*1e4)+\
                 " cm, gives a ratio below minimal geometric reinforcement amount: "+\
                 str(ctMinGeom*1e4)+" cm2\n"
            lmsg.warning(msg)
        ctMinMec= self.getMinimumMechanicalAmount(Nd)
        if(As<ctMinMec):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            msg= className+'.'+methodName+"; reinforcement area: "+str(As*1e4)+" cm, gives a ratio below minimal mechanical reinforcement amount: "+str(ctMinMec*1e4)+" cm2\n"
            lmsg.warning(msg)

def get_min_and_max_reinforcement(memberType:str, rcSection, Nd, bendingAxis= 'z'):
    ''' Return the minimum and maximum reinforcement areas for the given 
        section.

    :param memberType: member type: 'beam', 'column', 'tie', ...
    :param rcSection: reinforced concrete section.
    :param Nd: design value of the internal axial load.
    :param bendingAxis: bending axis ('z' or 'y'). 
    '''
    if(memberType=='column'):
        columnReinfRatios= ColumnReinforcementRatios(Ac= rcSection.getAc(), concrete= rcSection.getConcreteType(), reinforcingSteel= rcSection.getReinfSteelType())
        asMinGeom= columnReinfRatios.getMinimumGeometricAmount()
        asMinMech= columnReinfRatios.getMinimumMechanicalAmount(Nd= Nd)
        asMax= columnReinfRatios.getMaximumReinforcementAmount()
    elif(memberType=='tie'):
        tieReinfRatios= TieReinforcementRatios(Ac= rcSection.getAc(), concrete= rcSection.getConcreteType(), reinforcingSteel= rcSection.getReinfSteelType())
        asMinGeom= tieReinfRatios.getMinimumGeometricAmount()
        asMinMech= tieReinfRatios.getMinimumMechanicalAmount(Nd= Nd)
        asMax= None
    elif(memberType=='beam'):
        Ac= rcSection.getAc()
        W1= rcSection.getW1(bendingAxis= bendingAxis)
        h= rcSection.getDepth()
        beamReinfRatios= BeamReinforcementRatios(Ac= Ac, concrete= rcSection.getConcreteType(), reinforcingSteel= rcSection.getReinfSteelType(), W1= W1, h= h)
        asMinGeom= beamReinfRatios.getMinimumGeometricAmount()
        asMinMech= beamReinfRatios.getMinimumMechanicalAmount()
        asMax= None
    else:
        methodName= sys._getframe(0).f_code.co_name
        msg= methodName+"; member type: '"+str(memberType)+"' unknonw."
        lmsg.warning(msg)
        asMinGeom= None
        asMinMech= None
        asMax= None

    return asMinGeom, asMinMech, asMax
            
class ConcreteCorbel(object):
    '''Concrete corbel as in EHE-08 design code.'''
    def __init__(self,concrete, steel, jointType):
        '''
        Constructor.

        :param concrete: concrete material for corbel.
        :param concrete: steel material for corbel reinforcement.
        :param jointType: corbel to column joint quality
          ("monolitica", "junta" o "junta_debil").
        '''
        self.concrete= concrete
        self.steel= steel
        self.jointType= jointType
        
    def getCotgStrutAngle(self):
        '''getCotgStrutAngle()
 
          Return the cotangent of the angle between the concrete
          compressed strut an the vertical according to 
          clause 64.1.2.1 of EHE-08.
        '''
        if self.jointType=="monolitica":
            retval=1.4
        elif self.jointType=="junta":
            retval=1.0
        elif self.jointType=="junta_debil":
            retval=0.6
        return retval

    def getMinimumEffectiveDepth(self, a):
        '''getMinimumEffectiveDepth(self, a) return the minimal effective 
        depth of the corbel according to clause 64.1.2.1 of EHE-08.

        :param a: Distance (m) between the axis of the applied load and
                  the corbel fixed section (see figure 64.1.2 of EHE-08).

        '''
        return a*self.getCotgStrutAngle()/0.85

    def getMainReinforcementTension(self, Fv,Fh):
        '''getMainReinforcementTension(self, Fv,Fh). Return the tension
         in the main reinforcement according to clause 64.1.2.1.1 of EHE-08.

        :param Fv: Vertical load on the corbel, positive downwards (N).
        :param Fh: Horizontal load on the corbel, positive outwards (N).        
        '''
        return Fv/self.getCotgStrutAngle()+Fh

    def getAreaNecMainReinforcement(self, Fv,Fh):
        '''getAreaNecMainReinforcement(self, Fv,Fh): return the area needed
         for the main reinforcement according to clause 64.1.2.1.1 of EHE-08.

        :param Fv: Vertical load on the corbel, positive downwards (N).
        :param Fh: Horizontal load on the corbel, positive outwards (N).
        '''
        return self.getMainReinforcementTension(Fv,Fh)/min(self.steel.fyd(),400e6)
    @staticmethod
    def getStirrupsTension(Fv):
        ''' getStirrupsTension(Fv): return the tension in the stirrups
         to clause 64.1.2.1.1 of EHE-08.

        :param Fv: Vertical load on the corbel, positive downwards (N).
        '''
        return 0.2*Fv

    def getAreaNecCercos(self,Fv):
        '''getAreaNecCercos(self,Fv): return the area needed for the 
         stirrup reinforcements bars in the corbel according to 
         clause 64.1.2.1.1 of EHE-08.

        :param Fv: Vertical load on the corbel, positive downwards (N).
        '''
        return self.getStirrupsTension(Fv)/min(self.steel.fyd(),400e6)

    def getAreaNecApoyo(self,Fv):
        '''getAreaNecApoyo(self,Fv): return the area needed for the
         support according to clause 64.1.2.1.2 of EHE-08.

        :param Fv: Vertical load on the corbel, positive downwards (N).
        '''
        return Fv/0.7/-self.concrete.fcd()

EC2table48_x= [12.0e6,16.0e6,20.0e6,25.0e6,30.0e6,35.0e6,40.0e6,45.0e6,50.0e6]
EC2table48_y= [0.18,0.22,0.26,0.30,0.34,0.37,0.41,0.44,0.48]
EC2table48= scipy.interpolate.interp1d(EC2table48_x,EC2table48_y)


def shearBetweenWebAndFlangesStrength(fck,gammac,hf,Asf,Sf,fyd):
    ''' Return the shear strength (kN/m) in the flange web contact by unit 
    length according to clause 4.3.2.5 of Eurocode 2

    Parameters:
    :param fck: concrete characteristic compressive strength (Pa).
    :param gammac: Partial safety factor for concrete.
    :param hf: flange thickness (m)
    :param Asf: reinforcement that cross the section by unit length (m2)
    :param Sf: spacement of the rebars that cross the section (m)
    :param fyd: design yield strength (Pa)
    '''
    hf=hf*1000     #Flange thickness in mm
    #shear strength at failure due to diagonal compression in the web.
    fcd=fck/gammac
    VRd2=0.2*fcd*hf
    #Section tensile strength
    taoRd= EC2table48(fck)
    VRd3=2.5*taoRd*hf+Asf/Sf*fyd
    return min(VRd2,VRd3)

#Example:
#  esfRasMax= shearBetweenWebAndFlangesStrength(25e6,1.5,0.3,565e-6,0.2,500e6)


# Checking of solid block members under concentrated loads, according
# to clause 61 of EHE-08


class BlockMember(object):
    ''' Checking of solid block members under concentrated loads, according
        to clause 61 of EHE-08.

    :ivar a: side length (see figure 61.1.a of EHE-08).
    :ivar a1: length of the side of the loaded area parallel to a (see figure 61.1.a of EHE-08).
    :ivar b: side length (see figure 61.1.a of EHE-08).
    :ivar b1: length of the side of the loaded area parallel to b (see figure 61.1.a of EHE-08).
    :ivar concrete: concrete material.
    :ivar steel: steel material.
    '''
    def __init__(self, a, b, a1, b1, concrete, steel):
        '''
        Constructor.

        Parameters:
          :param a: side length (see figure 61.1.a of EHE-08).
          :param b: side length (see figure 61.1.a of EHE-08).
          :param a1: length of the side of the loaded area parallel to a (see figure 61.1.a of EHE-08).
          :param b1: length of the side of the loaded area parallel to b (see figure 61.1.a of EHE-08).
          :param concrete: concrete material.
          :param steel: reinforcing steel material.
        '''
        self.a= a
        self.a1= a1
        self.b= b
        self.b1= b1
        self.concrete= concrete
        self.steel= steel
      
    def getAc(self):
        '''Return area of the block member.'''
        return self.a*self.b
  
    def getAc1(self):
        '''Return block member loaded area.'''
        return self.a1*self.b1
  
    def getF3cd(self):
        '''Return the value of f3cd according to clause 61.2 of EHE-08.'''
        return min(math.sqrt(self.getAc()/self.getAc1()),3.3)*self.concrete.fcd()
  
    def getNuConcentratedLoad(self):
        ''' Return the the maximum compressive force that can be obtained in the
        Ultimate Limit State of on a restricted surface (see figure 61.1.a on
        page 302 of EHE-08), of area Ac1 , concentrically and homothetically 
        situated on another area, Ac.

        '''
        return self.getAc1()*self.getF3cd()

    def getConcreteEfficiency(self, Nd):
        ''' Return the efficiency of the concrete strength.

        :param Nd: concentrated load.
        '''
        return Nd/(-self.getNuConcentratedLoad())
        
    def getUad(self, Nd):
        '''
        Return the design tension for the transverse reinforcement in
        a direction parallel to side a (see figure 61.1.a page 302 EHE-08).

        Parameters:
          :param Nd: concentrated load.
        '''
        return 0.25*((self.a-self.a1)/self.a)*Nd
    
    def getReinforcementAreaAd(self, Nd):
        '''
        Return the area of the reinforcement parallel to side a
        (see figure 61.1.a page 302 EHE-08)

          :param Nd: concentrated load.
        '''
        return self.getUad(Nd)/min(self.steel.fyd(), 400e6)
    
    def getUbd(self, Nd):
        '''
        Return the design tension for the transverse reinforcement in
        a direction parallel to side b (see figure 61.1.a page 302 EHE-08).

          :param Nd: concentrated load.
        '''
        return 0.25*((self.b-self.b1)/self.b)*Nd
    
    def getReinforcementAreaBd(self, Nd):
        '''
        Return the area of the reinforcement parallel to side b
        (see figure 61.1.a page 302 EHE-08)

          :param Nd: concentrated load.
          :param fyd: steel yield strength.
        '''
        return self.getUbd(Nd)/min(self.steel.fyd(), 400e6)


class LongShearJoints(object):
    '''Verification of ultimate limit state due to longitudinal shear stress 
       at joints between concretes according to clause 47 of EHE.
    
    :ivar concrete: weakest EHE concrete type (ex: EHE_materials.HA25)
    :ivar reinfsteel: EHE reinforcing steel (ex: EHE_materials.B500S)
    :ivar contactSurf: Contact surface per unit length. This shall not extend 
                      to zones where the penetrating width is less than 20 mm 
                      or the maximum diameter of the edge or with a cover of 
                      less than 30 mm
    :ivar roughness: roughness of surface ('L' for low degree of roughness,
                     'H' for high degree of roughness). Defaults to 'L'
    :ivar dynamic: low fatigue or dynamic stresses consideration
                   ('Y' if considered, 'N' for not considered). Defaults to 'N'
    :ivar sigma_cd: External design tensile stress perpendicular to the plane 
                    of the joint. (<0 for compression tensions) 
                    (if sigma_cd > 0, beta, fct_d=0). Defaults to 0.
    :ivar Ast: Cross-section of effectively anchored steel bars, closing 
               the joint (area of 1 those rebars, spacement is given also as a 
               parameter). Defaults to None = no reinforcement, in this case 
               all parameters attached to reinforcement are ignored .
    :ivar spacement: distance between the closing bars along the joint plane.
                     (defaults to None)
    :ivar angRebars: Angle formed by the joining bars with the plane of the 
                     joint (degrees). Reinforcements with α > 135° or α < 45° 
                     shall not be incorporated. Defaults to 90º
    '''

    def __init__(self,concrete,reinfsteel,contactSurf,roughness='L',dynamic='N',sigma_cd=0,Ast=None,spacement=None,angRebars=90):
        ''' Constructor.

        :param concrete: weakest EHE concrete type (ex: EHE_materials.HA25)
        :param reinfsteel: EHE reinforcing steel (ex: EHE_materials.B500S)
        :param contactSurf: Contact surface per unit length. This shall not 
                            extend to zones where the penetrating width is 
                            less than 20 mm or the maximum diameter of 
                            the edge or with a cover of less than 30 mm
        :param roughness: roughness of surface ('L' for low degree of roughness,
                         'H' for high degree of roughness). Defaults to 'L'
        :param dynamic: low fatigue or dynamic stresses consideration
                        ('Y' if considered, 'N' for not considered). 
                        Defaults to 'N'
        :param sigma_cd: External design tensile stress perpendicular to the
                         plane of the joint. (<0 for compression tensions) 
                         (if sigma_cd > 0, beta, fct_d=0). Defaults to 0.
        :param Ast: Cross-section of effectively anchored steel bars, closing 
                   the joint -area of 1 those rebars, spacement is given also 
                   as a parameter-. Defaults to None = no reinforcement, in 
                   this case  all parameters attached to reinforcement are
                   ignored.
        :param spacement: distance between the closing bars along the joint
                          plane (defaults to None).
        :param angRebars: Angle formed by the joining bars with the plane of 
                          the joint (degrees). Reinforcements with α > 135° 
                          or α < 45° shall not be incorporated. Defaults to 90º
        '''
        self.concrete=concrete
        self.reinfsteel=reinfsteel
        self.contactSurf=contactSurf
        self.roughness=roughness
        self.dynamic=dynamic
        self.sigma_cd=sigma_cd
        self.Ast=Ast
        self.spacement=spacement
        self.angRebars=angRebars

    def getBetaCoef(self):
        ''' Return beta coefficient depending on the roughness of contact 
            surfaces (clause 47.2.1 EHE and table 47.2.2.2):

              - 0.80 in rough contact surfaces of composite sections that are
                interconnected so that one composite section may not overhang
                the other, for example, are dovetailed, and if the surface is 
                open and rough, e.g. like joists as left by a floor laying 
                machine.
              - 0.40 in intentionally rough surfaces with a high degree of 
                roughness.
              - 0.20 in unintentionally rough surfaces with a low degree of 
                roughness.
              - at low fatigue or dynamic stresses beta shall be reduced 
                by 50%.

        '''
        if self.sigma_cd > 0: #tension
            beta=0
        else:
            if self.roughness[0].lower()=='l':
                beta=0.2
            else:
                beta=0.8
            if self.dynamic[0].lower()=='y':
                beta*=0.5
        return beta
    
    def getUltShearStressWithoutReinf(self):
        '''Return the ultimate longitudinal shear stress in a section 
        without any transverse reinforcement
        '''
        beta=self.getBetaCoef()
        tao_ru_1=beta*(1.30-0.30*abs(self.concrete.fck*1e-6)/25.)*self.concrete.fctd()
        tao_ru_2=0.7*beta*self.concrete.fctd()
        return max(tao_ru_1,tao_ru_2)

    def getMuCoefCase1(self):
        ''' Return mu coefficient depending of roughness, for calculation 
            of ultimate shear stress at a joint in case 1 (clause 47.2.1 EHE, 
            table 47.2.2.2).
        '''
        if self.roughness[0].lower()=='l':
            mu=0.3
        else:
            mu=0.6
        return mu

    def getMuCoefCase2(self):
        ''' Return mu coefficient depending of roughness, for calculation 
            of ultimate shear  stress at a joint in case 2
            (clause 47.2.1 EHE, table 47.2.2.2):
        '''
        if self.roughness[0].lower()=='l':
            mu=0.6
        else:
            mu=0.9
        return mu

    def getUltShearStressWithReinf(self, tao_rd):
        '''Return the ultimate longitudinal shear stress at a joint between
           concrete sections with transverse reinforcement according to clause
           47.2.2 of EHE-08.

        :param tao_rd: design longitudinal shear stress at a joint between
                       concrete.
        '''
        beta=self.getBetaCoef()
        compVal= 2.5*beta*(1.30-0.30*abs(self.concrete.fck*1e-6)/25.)*self.concrete.fctd()
        alpha= math.radians(self.angRebars)
        f_yalphd=min(self.reinfsteel.fyd(),400e6)
        if(tao_rd <= compVal):  #case 1 (clause 47.2.2.1)
            mu= self.getMuCoefCase1() # table 47.2.2.2
        else: #case 2 (clause 47.2.2.2)
            mu= self.getMuCoefCase2() # table 47.2.2.2
        term_aux=mu*math.sin(alpha)+math.cos(alpha)
        tao_ru=self.Ast/(self.spacement*self.contactSurf)*f_yalphd*term_aux-mu*self.sigma_cd
        if tao_rd <= compVal:
            tao_ru+=compVal/2.5
        tao_ru_max= 0.25*abs(self.concrete.fcd())
        return min(tao_ru, tao_ru_max)

    def checkShearStressJoints(self,tao_rd):
        '''Verify the ultimate limit state due to longitudinal shear stress.

        :param tao_rd: design longitudinal shear stress at a joint between
                       concrete.
        '''
        if self.Ast > 0:
            tao_ru=self.getUltShearStressWithReinf(tao_rd)
        else:
            tao_ru=self.getUltShearStressWithoutReinf()
        if tao_rd<=tao_ru:
            lmsg.log("OK!")

##################
# Rebar families.#
##################

class EHERebarFamily(rf.RebarFamily):
    ''' Family or reinforcement bars with checking according to EHE-08.

       :ivar pos: reinforcement position according to clause 69.5.1
                  of EHE-08 (I: good adherence, II: poor adherence).
    '''
    def __init__(self,steel, diam, spacing, concreteCover, pos= 'II'):
        ''' Constructor.

        :param steel: reinforcing steel material.
        :param diam: diameter of the bars.
        :param spacing: spacing of the bars.
        :param concreteCover: concrete cover of the bars.
        :param pos: reinforcement position according to clause 69.5.1
                   of EHE-08 (I: good adherence, II: poor adherence).
        '''
        super(EHERebarFamily,self).__init__(steel,diam,spacing,concreteCover)
        self.pos= pos

    def getCopy(self):
        return EHERebarFamily(steel= self.steel, diam= self.diam, spacing= self.spacing, concreteCover= self.concreteCover, pos= self.pos)
      
    def getRebarController(self):
        return RebarController(concreteCover= self.concreteCover, spacing= self.spacing, pos= self.pos)

    def getBasicAnchorageLength(self,concrete):
      ''' Return the basic anchorage length of the bars.'''
      rebarController= self.getRebarController()
      return rebarController.getBasicAnchorageLength(concrete,self.getDiam(),self.steel)
  
    def getMinReinfAreaInBending(self, concrete, thickness, b= 1.0, steelStressLimit= 450e6, memberType= None):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under flexion per unit length 
           according to clause 42.3.5. 

        :param concrete: concrete material.
        :param thickness: gross thickness of concrete section (doesn't include 
                          the area of the voids).
        :param b: width of concrete section.
        :param memberType: member type; slab, wall, beam, column, etc.
        :param steelStressLimit: maximum stress permitted in the reinforcement 
                                 immediately after formation of the crack. This
                                 may be taken as the yield strength of the 
                                 reinforcement, fyk. A lower value may, 
                                 however, be needed to satisfy the crack width 
                                 limits according to the maximum bar size or
                                 spacing.
        '''
        retval= 4e-3*thickness*b
        fy= self.steel.fyk
        limit= min(450e6, steelStressLimit)
        if(memberType=='slab'):
            retval= thickness*b
            if(fy<limit):
                retval*= 2e-3
            else:
                retval*= 1.8e-3
        elif(memberType=='wall' or memberType=='short_wall'):
            retval= thickness*b
            if(fy<limit):
                retval*= 1.2e-3
            else:
                retval*= 0.98e-3
        elif(memberType=='beam'):
            retval= thickness*b
            if(fy<limit):
                retval*= 3.3e-3
            else:
                retval*= 2.8e-3
        elif(memberType=='column'):
            retval= 4e-3*thickness*b
        elif(memberType=='footing'): # see remark (1) in table 42.3.5
            retval= thickness*b
            if(fy<limit):
                retval*= 1e-3
            else:
                retval*= 0.9e-3
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= className+'.'+methodName+"; member type: " + str(memberType) + " not implemented.\n"
            lmsg.error(errMsg)
        return retval

    def getMinReinfAreaInTension(self, concrete, thickness, b= 1.0, memberType= None):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under tension.

        :param concrete: concrete material.
        :param thickness: gross thickness of concrete section.
        :param b: width of concrete section.
        :param memberType: member type; slab, wall, beam, column, etc.
        '''
        retval= min(thickness,0.5)*b # see table 42.3.5 remarks.
        fy= self.steel.fyk
        limit= 450e6
        if(fy<limit):
            retval*= 4e-3
        else:
            retval*= 3.2e-3
        return retval

    def getVR(self,concrete,Nd,Md,b,thickness):
        '''Return the approximated shear resistance carried by the concrete 
           on a (b x thickness) rectangular section.

        :param concrete: concrete material.
        :param Nd: design axial force (IGNORED).
        :param Md: design bending moment (IGNORED).
        :param b: width of the rectangular section.
        :param thickness: height of the rectangular section.
        '''
        retval= getFcvEH91(concrete.fcd())*b*0.9*thickness
        return retval


def define_rebar_families(steel, cover, diameters= [8e-3, 10e-3, 12e-3, 14e-3, 16e-3, 20e-3, 25e-3, 32e-3], spacings= [0.1, 0.15, 0.2]):
    ''' Creates a dictionary with predefined rebar families.

    :param steel: rebars steel material.
    :param cover: concrete cover for the families.
    :param diameters: list of diameters.
    :param spacings: list of spacings between bars. 
    '''
    retval= dict()
    for diameter in diameters:
        diameterText= str(int(diameter*1e3))
        for spacing in spacings:
            spacingText= str(int(spacing*1e2))
            familyName= 'A'+diameterText+'_'+spacingText
            retval[familyName]= EHERebarFamily(steel= steel, diam= diameter, spacing= spacing, concreteCover= cover)
    return retval

def read_buckling_analysis_results(bucklingAnalysisResultsFileName, setCalc=None):
    '''Extracts element and combination identifiers from the internal
    forces JSON file. Return elementTags, idCombs , internal forces values
    and buckling parameters values.
    
    :param bucklingAnalysisResultsFileName: name of the file containing the internal
                                forces obtained for each element for 
                                the combinations analyzed
    :param setCalc: set of elements to be analyzed (defaults to None which 
                    means that all the elements in the file of internal forces
                    results are analyzed)
    '''
    elementTags= set()
    idCombs= set()
    with open(bucklingAnalysisResultsFileName) as json_file:
        bucklingAnalysisResultsDict= json.load(json_file)
    json_file.close()
    internalForcesDict= bucklingAnalysisResultsDict['internal_forces']
    eheBucklingParametersDict= bucklingAnalysisResultsDict['ehe_buckling_parameters']
    bucklingLoadFactors= eheBucklingParametersDict['buckling_load_factors']
    eheElementBucklingParameters= eheBucklingParametersDict['element_parameters']
    
    internalForcesValues= defaultdict(list)
    bucklingParametersValues= dict()
    
    if(not setCalc):
        for comb in internalForcesDict.keys():
            idComb= str(comb)
            idCombs.add(idComb)
            elements= internalForcesDict[comb]
            for elemId in elements.keys():
                tagElem= eval(elemId)
                elementData= elements[elemId]
                #elementType= elementData['type']
                internalForces= elementData['internalForces']
                for k in internalForces.keys():
                    idSection= eval(k)
                    elementTags.add(tagElem)
                    crossSectionInternalForces= lsd.get_cross_section_internal_forces(internalForces= internalForces, idComb= idComb, tagElem= tagElem, key= k, vonMisesStressId= None)
                    internalForcesValues[tagElem].append(crossSectionInternalForces)
                bucklingParametersValues[tagElem]=  eheElementBucklingParameters[str(tagElem)]
    else:
        setElTags= frozenset(setCalc.getElementTags()) # We construct a frozen set to accelerate searching.
        for idComb in internalForcesDict.keys():
            idCombs.add(idComb)
            elements= internalForcesDict[idComb]
            for elemId in elements.keys():
                tagElem= eval(elemId)
                if(tagElem in setElTags): # search element tag
                    elementData= elements[elemId]
                    #elementType= elementData['type']
                    internalForces= elementData['internalForces']
                    for k in internalForces.keys():
                        idSection= eval(k)
                        elementTags.add(tagElem)
                        crossSectionInternalForces= lsd.get_cross_section_internal_forces(internalForces= internalForces, idComb= idComb, tagElem= tagElem, key= k, vonMisesStressId= None)
                        internalForcesValues[tagElem].append(crossSectionInternalForces)
                    bucklingParametersValues[tagElem]=  eheElementBucklingParameters[str(tagElem)]
    return (elementTags,idCombs,internalForcesValues, bucklingParametersValues, bucklingLoadFactors)
