# -*- coding: utf-8 -*-
from __future__ import print_function

''' Classes and functions for limit state checking according to Eurocode 2. '''

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

import sys
import math
from materials import limit_state_checking_base as lscb
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import fiber_sets
from postprocess import control_vars as cv
from misc_utils import log_messages as lmsg
from materials import concrete_base
from materials.sections import rebar_family as rf

class RebarController(lscb.RebarController):
    '''Control of some parameters as development length 
       minimum reinforcement and so on.

    :ivar alphaCoefs: coefficients for anchorage length calculation
                   given in Table 8.2 of EC2:2004.
    :ivar eta1: coefficient related to the quality of the bond condition 
                 and the position of the bar during concreting.
                 eta1= 1,0 when 'good' conditions are obtained and
                 eta1= 0,7 for all other cases.
    :ivar compression: true if reinforcement is compressed.
    '''

    def __init__(self, concreteCover= 35e-3, spacing= 150e-3, alphaCoefs=(1.0, None, 1.0, 1.0, 1.0), eta1= 0.7, compression= True):
        '''Constructor.

        :param alphaCoefs: coefficients for anchorage length calculation
                       given in Table 8.2 of EC2:2004.
        :param eta1: coefficient related to the quality of the bond condition 
                     and the position of the bar during concreting.
                     eta1= 1,0 when 'good' conditions are obtained and
                     eta1= 0,7 for all other cases.
        :param compression: true if reinforcement is compressed.
        '''
        super(RebarController,self).__init__(concreteCover= concreteCover, spacing= spacing)
        self.alphaCoefs= alphaCoefs
        self.eta1= eta1
        self.compression= compression

    def getBasicAnchorageLength(self, concrete, rebarDiameter, steel, steelEfficiency= 1.0):
        '''Returns basic required anchorage length in tension according to 
           clause 8.4.3 of EC2:2004 (expression 8.3).

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of the bar.
        :param steel: reinforcement steel.
        :param steelEfficiency: ratio between the stress on the reinforcement
                                and the yield stress of the 
                                steel: (sigma_sd/fyd).
        '''
        sigma_sd= steelEfficiency*steel.fyd()
        return steel.getBasicAnchorageLength(concrete= concrete, rebarDiameter= rebarDiameter, eta1= self.eta1, sigma_sd= sigma_sd)

    def getConcreteMinimumCoverEffect(self, rebarDiameter, barShape= 'bent', lateralConcreteCover= None):
        ''' Return the value of the alpha_2 factors that introduces the effect
            of concrete minimum cover according to figure 8.3 and table 8.2
            of EC2:2004.

        :param rebarDiameter: nominal diameter of the bar.
        :param barShape: 'straight' or 'bent' or 'looped'.
        :param lateralConcreteCover: lateral concrete cover (c1 in figure 8.3
                                     of EC2:2004). If None make it equal to
                                     the regular concrete cover.
        '''
        retval= 1.0
        if(not self.compression):
            if(lateralConcreteCover is None):
                lateralConcreteCover= self.concreteCover
            if(barShape=='straight'):
                cd= min(self.spacing/2.0, lateralConcreteCover, self.concreteCover)
                retval-= 0.15*(cd-rebarDiameter)/rebarDiameter
                retval= max(retval, 0.7)
            elif(barShape=='bent'):
                cd= min(self.spacing/2.0, lateralConcreteCover)
                retval-= 0.15*(cd-3*rebarDiameter)/rebarDiameter
                retval= max(retval, 0.7)
            elif(barShape=='looped'):
                cd= min(self.spacing/2.0, self.concreteCover)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_nameS
                lmsg.error(className+'.'+methodName+'; unknown bar shape: '+str(barShape)+'.')
        retval= min(retval, 1.0)
        return retval
        
    
    def getDesignAnchorageLength(self, concrete, rebarDiameter, steel, steelEfficiency= 1.0, barShape= 'bent', lateralConcreteCover= None):
        '''Returns design  anchorage length according to clause 8.4.4
           of EC2:2004 (expression 8.4).

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of the bar.
        :param steel: reinforcement steel.
        :param steelEfficiency: ratio between the stress on the reinforcement
                                and the yield stress of the 
                                steel: (sigma_sd/fyd).
        :param barShape: 'straight' or 'bent' or 'looped'.
        :param lateralConcreteCover: lateral concrete cover (c1 in figure 8.3
                                     of EC2:2004). If None make it equal to
                                     the regular concrete cover.
        '''
        sigma_sd= steelEfficiency*steel.fyd()
        if(self.alphaCoefs[1] is None): # alpha_2 factor not set.
            alpha_2= self.getConcreteMinimumCoverEffect(rebarDiameter, barShape= barShape, lateralConcreteCover= lateralConcreteCover)
            aCoefs= (self.alphaCoefs[0], alpha_2,  self.alphaCoefs[2], self.alphaCoefs[3], self.alphaCoefs[4])
        return steel.getDesignAnchorageLength(concrete= concrete, rebarDiameter= rebarDiameter, alphaCoefs= aCoefs, eta1= self.eta1, sigma_sd= sigma_sd, compression= self.compression)


class CrackStraightController(lscb.LimitStateControllerBase):
    '''Definition of variables involved in the verification of the cracking
    serviceability limit state according to EC2 when considering a concrete
    stress-strain diagram that takes account of the effects of tension 
    stiffening.

    :ivar k1: coefficient that takes account of the bound properties of the 
          bonded reinforcement. k1=0.8 for high bond bars, k1=1.0 for bars with
          effectively plain surface (e.g. prestressing tendons). Defaults to 0.8
    :ivar k3: defaults to the value recommended by EC2 and in EHE k3=3.4
    :ivar k4: defaults to the value recommended by EC2 and EHE k4=0.425
    '''
    ControlVars= cv.RCCrackStraightControlVars
    def __init__(self, limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(CrackStraightController,self).__init__(limitStateLabel,fakeSection= False)
        self.k1=0.8
        self.k3=3.4
        self.k4=0.425

    def EC2_k2(self,eps1,eps2):
        '''Return the coefficient k2 involved in the calculation of the mean 
        crack distance according to EC2. This coefficient represents the effect of 
        the tension diagram in the section.

        :param eps1: maximum deformation calculated in the section at the limits 
           of the tension zone
        :param eps2: minimum deformation calculated in the section at the limits 
           of the tension zone
        '''
        k2= (eps1+eps2)/(2.0*eps1)
        return k2

    def EC2_hceff(self,h,d,x):
        '''Return the maximum height to be considered in the calculation of the 
        concrete effective area in tension.
    
        :param width: section width
        :param h: lever arm
        :param d: effective depth of the cross-section 
        :param x: depth of the neutral fiber.

        '''
        hceff=min(2.5*(h-d),abs(h+x)/3.,h/2.)
        return hceff

    def check(self,elements,nmbComb):
        ''' For each element in the set 'elememts' passed as first parameter and 
        the resulting internal forces for the load combination 'nmbComb'  
        passed as second parameter, this method calculates all the variables 
        involved in the crack-SLS checking and obtains the crack width.
        In the case that the calculated crack width is greater than the 
        biggest obtained for the element in previous load combinations, this value
        is saved in the element results record. 

        Elements processed are those belonging to the phantom model, that is to 
        say, of type xc.ZeroLengthSection. As we have defined the variable 
        fakeSection as False, a reinfoced concrete fiber section is generated
        for each of these elements. 
        '''
        if(self.verbose):
          lmsg.log('Postprocessing combination: '+nmbComb)
        for e in elements:
            Aceff=0  #init. value
            R=e.getResistingForce()
            sct=e.getSection()
            sctCrkProp=lscb.fibSectLSProperties(sct)
            sctCrkProp.setupStrghCrackDist()
            hceff=self.EC2_hceff(sctCrkProp.h,sctCrkProp.d,sctCrkProp.x)
            # Acgross=sct.getGrossEffectiveConcreteArea(hceff)
            Aceff=sct.getNetEffectiveConcreteArea(hceff,'tensSetFb',15.0)
            concrete=EC2_materials.concrOfName[sctCrkProp.concrName]
            rfSteel=EC2_materials.steelOfName[sctCrkProp.rsteelName]
            k2=self.EC2_k2(sctCrkProp.eps1,sctCrkProp.eps2)
            # print('elem= ',e.tag, ' Aceff= ',Aceff)
            if(Aceff<=0):
                s_rmax=0
            else:
                ro_s_eff=sctCrkProp.As/Aceff #effective ratio of reinforcement
                s_rmax=self.k3*sctCrkProp.cover+self.k1*k2*self.k4*sctCrkProp.fiEqu/ro_s_eff
                #Parameters for tension stiffening of concrete
                paramTS= concrete_base.paramTensStiffness(concrMat=concrete,reinfMat=rfSteel,reinfRatio=ro_s_eff,diagType='K')
                concrete.tensionStiffparam=paramTS #parameters for tension
                #stiffening are assigned to concrete
                ftdiag=concrete.tensionStiffparam.pointOnsetCracking()['ft']                    #stress at the adopted point for concrete onset cracking
                Etsdiag=abs(concrete.tensionStiffparam.regresLine()['slope'])
                fiber_sets.redefTensStiffConcr(setOfTenStffConcrFibSect=sctCrkProp.setsRC.concrFibers,ft=ftdiag,Ets=Etsdiag)
            e.setProp('ResF',R)   #vector resisting force
            e.setProp('s_rmax',s_rmax)  #maximum crack distance
        self.preprocessor.getDomain.revertToStart()
        self.solutionProcedure.solveComb(nmbComb)
        for e in elements:
            sct=e.getSection()
            rfset=sct.getFiberSets()['reinfSetFb']
            eps_sm=rfset.getStrainMax()
            srmax=e.getProp('s_rmax')
#            eps_cm=concrete.fctm()/2.0/concrete.E0()
#            wk=srmax*(eps_sm-eps_cm)
            wk=srmax*eps_sm
#            print(' eps_sm= ',eps_sm, ' srmax= ', srmax, ' wk= ',wk)
#            print('e.getProp(self.limitStateLabel).wk', e.getProp(self.limitStateLabel).wk)
            if (wk>e.getProp(self.limitStateLabel).wk):
                R=e.getProp('ResF')
                e.setProp(self.limitStateLabel, self.ControlVars(idSection=e.getProp('idSection'),combName=nmbComb,N=-R[0],My=-R[4],Mz=-R[5],s_rmax=srmax,eps_sm=eps_sm,wk=wk))
                
###################
# Rebar families. #
###################

class EC2RebarFamily(rf.RebarFamily):
    ''' Family or reinforcement bars with checking according to EC2.

       :ivar pos: reinforcement position according to clause 66.5.1
                  of EC2.
    '''
    def __init__(self,steel, diam, spacing, concreteCover, pos= 'II'):
        ''' Constructor.

        :param steel: reinforcing steel material.
        :param diam: diameter of the bars.
        :param spacing: spacing of the bars.
        :param concreteCover: concrete cover of the bars.
        :param pos: reinforcement position according to clause 66.5.1
                   of EC2.
        '''
        super(EC2RebarFamily,self).__init__(steel,diam,spacing,concreteCover)
        self.pos= pos

    def getCopy(self):
        return EC2RebarFamily(steel= self.steel, diam= self.diam, spacing= self.spacing, concreteCover= self.concreteCover, pos= self.pos)



#These functions  should disappear               
def h_c_eff(depth_tot,depht_eff,depth_neutral_axis):
    '''
    Returns the depth of the effective area of concrete in tension surrounding
    the reinforcement or prestressing tendons, according to EC2

    :param depth_tot: overall depth of the cross-section [h]
    :param depht_eff: effective depth of the cross-section [d]
    :param depth_neutral_axis: depht of the neutral axis[x]
    '''
    h,d,x=depth_tot,depht_eff,depth_neutral_axis
    retval=min(2.5*(h-d),(h-x)/3.,h/2.)
    return retval

def ro_eff(A_s,width,h_c_eff):
    '''
    Returns the effective reinforcement ratio [A_s/A_ceff]depth of the effective area of concrete in tension surrounding
    the reinforcement or prestressing tendons, according to EC2

    :param A_s: area of reinforcment steel
    :param width: width of the RC cross-section
    :param ,h_c_eff: depth of the effective area of concrete in 
    tension surrounding the reinforcement or prestressing tendons
    '''
    retval=A_s/width/h_c_eff
    return retval

def s_r_max(k1,k2,k3,k4,cover,fiReinf,ro_eff):
    '''
    Returns the maximum crack spacing, according to EC2
 
    :param k1: coefficient which takes account of the bond properties
               of the bonded reinforcement:

               - =0.8 for high bond bars
               - =1.6 for bars with an effectively plain surface (e.g. prestressing tendons)

    :param k2: coefficient that takes account of the distribution of strain:

               - =0.5 for bending
               - =1.0 for pure tension
               - for cases of eccentric tension or for local areas, intermediate values of k2 should be used (see clause 7.3.4 EC2)
    :param k3: recommended k3=3.4
    :param k4: recommended k4=0.425 
    :param cover: cover of the longitudinal reinforcement
    :param fiReinf: bar diameter. Where a mixture of bar diameters is used in a
                    section, an equivalent diameter is used (see clause 7.3.4 EC2)
    :param ro_eff: effective reinforcement ratio
    '''
    retval=k3*cover+k1*k2*k4*fiReinf/ro_eff
    return retval

# EC2:2004 6.2 Shear

# EC2:2004 6.2.2 Members not requiring design shear reinforcement
def getShearResistanceCrackedNoShearReinf(concrete, NEd, Ac, Asl, bw, d, nationalAnnex= None):
    ''' Return the design value of the shear resistance VRdc for cracked 
        sections subjected to bending moment, according to expressions 6.2.a 
        and 6.2.b of EC2:2004.

    :param concrete: concrete material.
    :param NEd: axial force in the cross-section due to loading or prestressing.
    :param Ac: area of concrete cross-section. 
    :param Asl: area of the tensile reinforcement, which extends beyond
                the section considered (see clause 6.2.2 of EC2:2004).
    :param bw: smallest width of the cross-section in the tensile area.
    :param d: effective depth of the cross-section.
    :param nationalAnnex: identifier of the national annex.
    '''
    k= min(1.0+math.sqrt(0.2/d),2) # d in meters.
    ro_l= min(Asl/bw/d,.02)
    fcdMPa= -concrete.fcd()/1e6 # design value of concrete compressive strength (MPa).
    sigma_cp= min(-NEd/Ac/1e6,.2*fcdMPa)
    gamma_c= concrete.gmmC
    CRdc= 0.18/gamma_c # Recommended value and Spanish national annex.
    k1= 0.15
    fckMPa= -concrete.fck/1e6 # concrete characteristic compressive strength (MPa).
    if(nationalAnnex=='Spain'):
        v_min= 0.075/gamma_c*math.pow(k,1.5)*math.sqrt(min(fckMPa,60))
    else:
        v_min= 0.035*math.pow(k,1.5)*math.sqrt(fckMPa)
    
    VRdc_62a= (CRdc*k*math.pow(100.0*ro_l*fckMPa,1/3.0)+k1*sigma_cp)*bw*d*1e6
    VRdc_62b= (v_min+k1*sigma_cp)*bw*d*1e6
    return max(VRdc_62a, VRdc_62b)
    
def getShearResistanceNonCrackedNoShearReinf(concrete, I, S, NEd, Ac, bw, alpha_l= 1.0):
    ''' Return the design value of the shear resistance VRdc for non-cracked 
        sections subjected to bending moment, according to expression 6.4 of
        EC2:2004.

    :param concrete: concrete material.
    :param I: second moment of area.
    :param S: first moment of area above and about the centroidal axis.
    :param NEd: axial force in the cross-section due to loading or prestressing.
    :param Ac: area of concrete cross-section. 
    :param bw: smallest width of the cross-section in the tensile area.
    :param alpha_l: see expression 6.4 in EC2:2004.
    '''
    sigma_cp= -NEd/Ac/1e6 # concrete compressive stress at the centroidal
                          # axis due to axial loading and/or prestressing
    fctdMPa= concrete.fctd()/1e6 #design tensile strength (MPa)
    return (I/S*bw)*math.sqrt(fctdMPa**2+alpha_l*sigma_cp*fctdMPa)*1e6

# EC2:2004 6.2.3 Members requiring design shear reinforcement.
def getWebStrutAngleForSimultaneousCollapse(concrete, bw, s, Asw, shearReinfSteel, shearReinfAngle= math.pi/2.0, nationalAnnex= None):
    ''' Return the web strut angle that makes web concrete collapse at the same
        time that the shear reinforcement (V_{Rd,s}= V_{Rd,max})

    :param concrete: concrete material.
    :param bw: smallest width of the cross-section in the tensile area.
    :param s: spacing of the stirrups.
    :param Asw: cross-sectional area of the shear reinforcement.
    :param shearReinfSteel: reinforcing steel material.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    # nu1: strength reduction factor for concrete cracked in shear
    nu= concrete.getShearStrengthReductionFactor(nationalAnnex)
    fcd= -concrete.fcd() # design value of concrete compressive strength (MPa).
    fywd= shearReinfSteel.fyd() # design yield strength of the shear reinforcement
    cotgTheta= math.sqrt((bw*s*nu*fcd)/(Asw*fywd*math.sin(shearReinfAngle))-1)
    return math.atan(1.0/cotgTheta)   

def getMaximumShearWebStrutCrushing(concrete, NEd, Ac, bw, z, shearReinfAngle= math.pi/2.0, webStrutAngle= math.pi/4.0, nationalAnnex= None):
    ''' Return the maximum shear force due to diagonal compression in the web
        (strut crushing) according to expression 6.14 and 6.9 of EC2:2004.

    :param concrete: concrete material.
    :param NEd: axial force in the cross-section due to loading or prestressing.
    :param Ac: area of concrete cross-section. 
    :param bw: smallest width of the cross-section in the tensile area.
    :param z: internal lever arm.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param webStrutAngle: (theta) angle between the concrete compression strut and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    # alpha_cw: coefficient taking account of the state of the stress in the compression chord
    alpha_cw= concrete.getAlphaCw(NEd, Ac, nationalAnnex)
    # nu1: strength reduction factor for concrete cracked in shear
    nu1= concrete.getShearStrengthReductionFactor(nationalAnnex)
    fcd= -concrete.fcd() # design value of concrete compressive strength (MPa).
    webStrutAngle= checkWebStrutAngleLimits(webStrutAngle, nationalAnnex)
    cotgTheta= 1/math.tan(webStrutAngle)
    cotgAlpha= 1/math.tan(shearReinfAngle)
    return alpha_cw*bw*z*nu1*fcd*(cotgTheta+cotgAlpha)/(1+cotgTheta**2)

def getMaximumEffectiveShearReinforcement(concrete, NEd, Ac, bw, s, shearReinfSteel, shearReinfAngle= math.pi/2.0, nationalAnnex= None):
    ''' Return the maximum effective shear reinforcement according to expression
        6.15 and 6.12 of EC2:2004.

    :param concrete: concrete material.
    :param NEd: axial force in the cross-section due to loading or prestressing.
    :param Ac: area of concrete cross-section. 
    :param bw: smallest width of the cross-section in the tensile area.
    :param s: spacing of the stirrups.
    :param Asw: cross-sectional area of the shear reinforcement.
    :param shearReinfSteel: reinforcing steel material.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    # alpha_cw: coefficient taking account of the state of the stress in the compression chord
    alpha_cw= concrete.getAlphaCw(NEd, Ac, nationalAnnex)
    # nu1: strength reduction factor for concrete cracked in shear
    nu1= concrete.getShearStrengthReductionFactor(nationalAnnex)
    fcd= -concrete.fcd() # design value of concrete compressive strength (MPa).
    fywd= shearReinfSteel.fyd()
    return 0.5*alpha_cw*nu1*fcd/fywd*bw*s

def getWebStrutAngleLimits(nationalAnnex= None):
    ''' Return the limits specified in the expression 6.7N of EC2:2004
        for the web strut angle.

    :param nationalAnnex: identifier of the national annex.
    '''
    if(nationalAnnex=='Spain'): #EN 1992-1-1:2004 Apartado 6.2.3 (2)
        limSup= math.atan(1/0.5)
        limInf= math.atan(1/2.0)
    else:
        limSup= math.pi/4.0 # math.atan(1.0)
        limInf= math.atan(1/2.5)
    return limInf, limSup

def checkWebStrutAngleLimits(webStrutAngle, nationalAnnex= None):
    ''' Check that the strut angle is inside the limits specified
        in the expression 6.7N of EC2:2004. Otherwise, issue a
        warning and return a suitable strut angle.

    :param webStrutAngle: (theta) angle between the concrete compression web strut and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    retval= webStrutAngle
    limInf, limSup= getWebStrutAngleLimits(nationalAnnex)
    if((webStrutAngle<limInf) or (webStrutAngle>limSup)): # eq 6.7N
        methodName= sys._getframe(0).f_code.co_name
        errString= methodName+'; strut angle: '+str(math.degrees(webStrutAngle))+' out of range: ['+str(math.degrees(limInf))+','+str(math.degrees(limSup))+']'
        if(webStrutAngle<limInf):
            retval= limInf
        elif(webStrutAngle>limSup):
            retval= limSup
        errString+= ' using '+str(math.degrees(retval))
        lmsg.warning(errString)
    return retval

def getShearResistanceShearReinf(concrete, NEd, Ac, bw, Asw, s, z, shearReinfSteel, shearReinfAngle= math.pi/2.0, webStrutAngle= math.pi/4.0, nationalAnnex= None):
    ''' Return the design value of the shear resistance VRds for shear 
        reinforced members according to expressions 6.7N, 6.13 and 6.14 of
        EC2:2004.

    :param concrete: concrete material.
    :param NEd: axial force in the cross-section due to loading or prestressing.
    :param Ac: area of concrete cross-section. 
    :param bw: smallest width of the cross-section in the tensile area.
    :param Asw: cross-sectional area of the shear reinforcement.
    :param s: spacing of the stirrups.
    :param z: inner lever arm, for a member with constant depth, corresponding to the bending moment in the element under consideration.
    :param shearReinfSteel: reinforcing steel material.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param webStrutAngle: (theta) angle between the concrete compression web strut and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    webStrutAngle= checkWebStrutAngleLimits(webStrutAngle, nationalAnnex)
    cotgTheta= 1/math.tan(webStrutAngle)
    cotgAlpha= 1/math.tan(shearReinfAngle)
    sinAlpha= math.sin(shearReinfAngle)
    fywd= shearReinfSteel.fyd()
    cotgThetaPluscotgAlpha= cotgTheta+cotgAlpha
    VRds= Asw/s*z*fywd*cotgThetaPluscotgAlpha*sinAlpha
    # nu1: strength reduction factor for concrete cracked in shear
    nu1= concrete.getShearStrengthReductionFactor(nationalAnnex)
    
    # if(sigma_reinf<0.8*shearReinfSteel.fyk):
    #     if(fckMPa<=60):
    #         nu1= 0.6
    #     else:
    #         nu1= max(0.9-fckMPa/200,0.5)
    
    # alpha_cw: coefficient taking account of the state of the stress in the compression chord
    alpha_cw= concrete.getAlphaCw(NEd, Ac, nationalAnnex)
    fcdMPa= -concrete.fcd()/1e6 # design value of concrete compressive strength (MPa).
    VRdmax= alpha_cw*bw*z*nu1*fcdMPa*cotgThetaPluscotgAlpha/(1+cotgTheta**2)*1e6
    return min(VRds, VRdmax)

def getMinShearReinforcementArea(concrete, shearReinfSteel, s, bw, shearReinfAngle= math.pi/2.0, nationalAnnex= None):
    ''' Return the cross-sectional area of the shear reinforcement 
        according to expression 9.4 of EC2:2004.

    :param concrete: concrete material.
    :param shearReinfSteel: reinforcing steel material.
    :param s: spacing of the stirrups.
    :param bw: smallest width of the cross-section in the tensile area.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    # minimum shear reinforcement ratio
    ro_w= concrete.getMinShearReinfRatio(shearReinfSteel, nationalAnnex)
    return ro_w*s*bw*math.sin(shearReinfAngle)

def getAdditionalTensileForceMainReinf(VEd, shearReinfAngle= math.pi/2.0, webStrutAngle= math.pi/4.0):
    ''' Return the additional tensile force, in the longitudinal reinforcement
        due to shear VEd according to expression 6.18 of EC2:2004.

    :param VEd: design value of the applied shear force.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param webStrutAngle: (theta) angle between the concrete web compression strut and the beam axis perpendicular to the shear force.
    '''
    return 0.5*VEd*(1.0/math.tan(webStrutAngle)-1.0/math.tan(shearReinfAngle))

# EC2:2004 6.2.4 Shear between web and flanges.
def getFlangeStrutAngleLimits(compressionFlange= True, nationalAnnex= None):
    ''' Return the limits specified in the clause 6.2.4(4) of EC2:2004 for
        the angle of the struts in the flange.

    :param compressionFlange: true if flange is compressed.
    :param nationalAnnex: identifier of the national annex.
    '''
    if(compressionFlange):
        limSup= math.pi/4.0 # math.atan(1.0)
        limInf= math.atan(1/2.0) # 26.5º
    else:
        limSup= math.pi/4.0 # math.atan(1.0)
        limInf= math.atan(1/1.25) # 38.6º
    return limInf, limSup

def checkFlangeStrutAngleLimits(flangeStrutAngle, compressionFlange= True, nationalAnnex= None):
    ''' Check that the strut angle is inside the limits specified
        in the expression 6.7N of EC2:2004. Otherwise, issue a
        warning and return a suitable strut angle.

    :param flangeStrutAngle: (theta_f) angle between the concrete flange compression strut and the shear plane (see figure 6.7 on EC2:2004).
    :param compressionFlange: true if flange is compressed.
    :param nationalAnnex: identifier of the national annex.
    '''
    retval= flangeStrutAngle
    limInf, limSup= getFlangeStrutAngleLimits(compressionFlange, nationalAnnex)
    if((flangeStrutAngle<limInf) or (flangeStrutAngle>limSup)): # eq 6.7N
        methodName= sys._getframe(0).f_code.co_name
        errString= methodName+'; flange strut angle: '+str(math.degrees(flangeStrutAngle))+' out of range: ['+str(math.degrees(limInf))+','+str(math.degrees(limSup))+']'
        if(flangeStrutAngle<limInf):
            retval= limInf
        elif(flangeStrutAngle>limSup):
            retval= limSup
        errString+= ' using '+str(math.degrees(retval))
        lmsg.warning(errString)
    return retval

def getMaximumShearFlangeStrutCrushingStress(concrete, flangeStrutAngle= math.pi/4.0, compressionFlange= True, nationalAnnex= None):
    ''' Return the maximum shear force due to diagonal compression in the web
        (strut crushing) according to expression 6.22 of EC2:2004.

    :param concrete: concrete material.
    :param flangeStrutAngle: (theta_f) angle between the concrete flange compression strut and the shear plane (see figure 6.7 on EC2:2004).
    :param compressionFlange: true if flange is compressed.
    :param nationalAnnex: identifier of the national annex.
    '''
    # nu1: strength reduction factor for concrete cracked in shear
    nu1= concrete.getShearStrengthReductionFactor(nationalAnnex)
    fcd= -concrete.fcd() # design value of concrete compressive strength (MPa).
    flangeStrutAngle= checkFlangeStrutAngleLimits(flangeStrutAngle, compressionFlange, nationalAnnex)
    return nu1*fcd*math.sin(flangeStrutAngle)*math.cos(flangeStrutAngle)

def getConcreteFlangeShearStrength(concrete, hf, DeltaX, nationalAnnex= None):
    ''' Return the shear stress resisted by plain concrete according to
        clause 6.2.4 (6) of EC2:2004.

    :param concrete: concrete material.
    :param hf: flange thickness at the shear plane.
    :param DeltaX: length under consideration (see figure 6.7 on EC2:2004).
    :param nationalAnnex: identifier of the national annex.
    '''
    return concrete.getConcreteFlangeShearStressStrength(nationalAnnex)*DeltaX*hf

def getFlangeShearResistanceShearReinfStress(concrete, hf, Asf, sf, shearReinfSteel, flangeStrutAngle= math.pi/4.0, compressionFlange= True, nationalAnnex= None):
    ''' Return the design value of the flange shear resistance  
        according to expressions 6.21 of EC2:2004.

    :param concrete: concrete material.
    :param hf: flange thickness at the shear plane.
    :param Asf: cross-sectional area of the flange transverse reinforcement.
    :param sf: spacing of the reinforcement.
    :param shearReinfSteel: reinforcing steel material.
    :param flangeStrutAngle: (theta_f) angle between the concrete flange compression strut and the shear plane (see figure 6.7 on EC2:2004).
    :param compressionFlange: true if flange is compressed.
    :param nationalAnnex: identifier of the national annex.
    '''
    flangeStrutAngle= checkFlangeStrutAngleLimits(flangeStrutAngle, compressionFlange, nationalAnnex)
    cotgThetaF= 1/math.tan(flangeStrutAngle)
    fyd= shearReinfSteel.fyd()
    vRds= Asf/sf*fyd*cotgThetaF/hf # expression 6.21 of EC2:2004
    vRdmax= getMaximumShearFlangeStrutCrushingStress(concrete, flangeStrutAngle, compressionFlange, nationalAnnex) # expression 6.22 of EC2:2004
    return min(vRds, vRdmax)

# 7.3.2 Minimum reinforcement areas

def getAsMinCrackControl(concrete, reinfSteel, h, Act, stressDistribution):
    ''' Return the minimum area of reinforcing steel within the tensile zone
        according to expression 7.1 of EC2:2004.

    :param concrete: concrete material.
    :param reinfSteel: reinforcing steel material.
    :param h: section depth.
    :param Act: area of concrete within tensile zone. The tensile zone is 
                that part of the section which is calculated to be in tension 
                just before formation of the first crack.
    :param stressDistribution: string indentifying the stress distribution
                               (bending or pure tension) of the cross-section.
    '''
    kc= 0.4 # coefficient which takes account of the stress distribution
            # within the section immediately prior to cracking and of the
            # change of the lever arm
    if(stressDistribution=='simple_tension'):
        kc= 1.0
    k= 1.0 # coefficient which allows for the effect of non-uniform
           # self-equilibrating stresses, which lead to a reduction of
           # restraint forces.
    if(h>=0.8):
        k= 0.65
    elif(h>0.3):
        k= -0.7*h+1.21 # linear interpolation.
    fctef= concrete.fctm() # mean value of the tensile strength of the
                           # concrete effective at the time when the cracks
                           # may first be expected to occur.
    sigma_s= reinfSteel.fyk
    return kc*k*fctef/sigma_s*Act

def getAsMinBeams(concrete, reinfSteel, h, z, bt, d, nationalAnnex= None):
    ''' Return the minimum area of reinforcing steel within the tensile zone
        according to expression 9.1N of EC2:2004.

    :param concrete: concrete material.
    :param reinfSteel: reinforcing steel material.
    :param h: section depth.
    :param d: effective depth.
    :param z: inner lever arm.
    :param bt: denotes the mean width of the tension zone.
    :param nationalAnnex: identifier of the national annex.
    '''
    if(nationalAnnex=='Spain'):
        W= bt*h**2/6.0
        fctmfl= max(1.6-h,1.0)*concrete.fctm()
        return W/z*fctmfl/reinfSteel.fyd()
    else:
        return max(0.26*concrete.fctm()/reinfSteel.fyk,0.0013)*bt*d
    
def getAsMaxBeams(Ac, nationalAnnex= None):
    ''' Return the minimum area of reinforcing steel within the tensile zone
        according to expression 9.1N of EC2:2004.

    :param Ac: area of concrete cross-section. 
    :param nationalAnnex: identifier of the national annex.
    '''
    return .04*Ac    

